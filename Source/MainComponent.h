#pragma once
using namespace std;
#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"
#include "DeckGUILookAndFeel.h"

/**
 * Main application component that contains and manages all UI elements
 * and the audio processing chain. Acts as the central hub for the DJ app.
 */
class MainComponent : public AudioAppComponent,
  public Slider::Listener
{
public:
  MainComponent();
  ~MainComponent() override;

  // AudioAppComponent interface methods
  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
  void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
  void releaseResources() override;

  // Component interface methods
  void paint(Graphics& g) override;
  void resized() override;

  // Handle crossfader movements
  void sliderValueChanged(Slider* slider) override;

private:
  // Audio format handling
  AudioFormatManager formatManager;
  AudioThumbnailCache thumbCache{ 100 };

  // Crossfader between decks
  Slider crossfader;
  DeckGUILookAndFeel crossfaderLookAndFeel;

  // First deck (left)
  DJAudioPlayer player1{ formatManager };
  DeckGUI deck1{ &player1, formatManager, thumbCache };

  // Second deck (right)
  DJAudioPlayer player2{ formatManager };
  DeckGUI deck2{ &player2, formatManager, thumbCache };

  // Audio mixer to combine both decks
  MixerAudioSource mixerAudioSource;
  Label crossfaderLabel;
  // Playlist component with references to both decks
  PlaylistComponent playlistComponent{ &player1, &player2, &deck1, &deck2 };

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};