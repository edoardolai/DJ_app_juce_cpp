#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "DeckGUILookAndFeel.h"

/*
* DeckGUI class represents a single deck in theour DJ application.
* Handles UI, user interactions, and communicates with the audio player.
*/


class DeckGUI : public Component,
    public Button::Listener,
    public Slider::Listener,
    public FileDragAndDropTarget,
    public Timer
{
public:
    /* Constructor takes pointers to:
       * - DJAudioPlayer: to control audio playback
       * - AudioFormatManager: to handle various audio file formats
       * - AudioThumbnailCache: for waveform generation
       */
    DeckGUI(DJAudioPlayer* player,
        AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& thumbCacheToUse);
    ~DeckGUI() override;

    void paint(Graphics& g) override;
    void resized() override;

    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider* slider) override;

    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

    // Tracks if the vinyl disc is being dragged
    bool isVinylBeingDragged = false;
    bool isLooping = false;
    void timerCallback() override;

    // Sets the deck ID (0 for Deck A, 1 for Deck B), needed mainly for styling
    void setDeckId(int id);

    // Made public so playlist can access it when loading tracks
    WaveformDisplay waveformDisplay;
private:
    DeckGUILookAndFeel djDeckLookAndFeel;

    // Transport controls
    TextButton playPauseButton{ "PLAY" };
    TextButton loadButton{ "LOAD" };
    TextButton loopButton{ "LOOP" };
    TextButton eqToggleButton{ "EQ" };
    FileChooser fChooser{ "Select a file..." };
    int deckId = 0;
    Slider volSlider;
    Slider speedSlider;
    Slider positionSlider;
    Slider vinylSlider;

    // Audio control sliders
    Label volLabel{ "volLabel", "VOLUME" };
    Label speedLabel{ "speedLabel", "TEMPO" };

    // EQ sliders
    Slider highEQSlider;
    Slider midEQSlider;
    Slider lowEQSlider;


    // EQ Labels
    Label highLabel{ "highLabel", "HIGH" };
    Label midLabel{ "midLabel", "MID" };
    Label lowLabel{ "lowLabel", "LOW" };


    Label deckLabel{ "deckLabel", "" };
    DJAudioPlayer* player;
    void sliderDragStarted(Slider* slider) override;
    void sliderDragEnded(Slider* slider) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};
