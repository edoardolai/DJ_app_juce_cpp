#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "DeckGUILookAndFeel.h"
#include <vector>
#include <string>

using namespace std;

// A struct to hold track information
struct TrackInfo {
    String title;
    String artist;
    URL fileURL;
    double duration = 0.0; // in seconds

    TrackInfo(String _title, String _artist, URL _fileURL, double _duration)
        : title(_title), artist(_artist), fileURL(_fileURL), duration(_duration) {}
};

class PlaylistComponent : public Component,
    public TableListBoxModel,
    public Button::Listener,
    public ComboBox::Listener
{
public:
    // Constructor takes references to both deck players and UIs
    PlaylistComponent(DJAudioPlayer* _player1, DJAudioPlayer* _player2, DeckGUI* _deck1, DeckGUI* _deck2);
    ~PlaylistComponent() override;

    // Component interface methods
    void paint(Graphics& g) override;
    void resized() override;

    // TableListBoxModel interface - for displaying tracks in a table
    int getNumRows() override;
    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

    // Event handlers
    void buttonClicked(Button* button) override;
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

    // Playlist management methods
    void addToPlaylist();  // Add a new track to the playlist
    juce::String getTimeString(double seconds);  // Format time as MM:SS


private:
    TableListBox tableComponent;
    vector<TrackInfo> tracks;

    // Reference to the deck players
    DJAudioPlayer* player1;
    DJAudioPlayer* player2;

    // Reference to the deck GUIs
    DeckGUI* deck1;
    DeckGUI* deck2;
    // UI Components
    TextButton addButton{ "+" };
    ComboBox deckSelector;
    Label deckSelectorLabel{ "", "Target Deck:" };
    DeckGUILookAndFeel playlistLookAndFeel;
    FileChooser fChooser{ "+" };
    // File operations
    AudioFormatManager formatManager;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};
