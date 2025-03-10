#include "PlaylistComponent.h"

PlaylistComponent::PlaylistComponent(DJAudioPlayer* _player1, DJAudioPlayer* _player2, DeckGUI* _deck1, DeckGUI* _deck2)
    : player1(_player1), player2(_player2), deck1(_deck1), deck2(_deck2)
{
    // Initialize format manager
    formatManager.registerBasicFormats();

    // Set up the table
    addAndMakeVisible(tableComponent);
    tableComponent.setModel(this);
    tableComponent.getHeader().addColumn("Track Title", 1, 250);
    tableComponent.getHeader().addColumn("Artist", 2, 180);
    tableComponent.getHeader().addColumn("Duration", 3, 80);
    tableComponent.getHeader().addColumn("Load", 4, 120);

    // Configure table appearance
    tableComponent.setColour(TableListBox::backgroundColourId, Colour(0xFF2d3035));
    tableComponent.setColour(TableListBox::outlineColourId, Colour(0xFF3d4148));
    tableComponent.setOutlineThickness(1);

    // Add button setup
    addAndMakeVisible(addButton);
    addButton.addListener(this);
    addButton.setLookAndFeel(&playlistLookAndFeel);
    addButton.setColour(TextButton::buttonColourId, Colour(0xFFf5a623));
    addButton.setColour(TextButton::buttonOnColourId, Colour(0xFFffcc00));
    addButton.setColour(TextButton::textColourOffId, Colours::white);
    addButton.setColour(TextButton::textColourOnId, Colours::black);
    // Deck selector setup
    addAndMakeVisible(deckSelector);
    deckSelector.addItem("Deck A", 1);
    deckSelector.addItem("Deck B", 2);
    deckSelector.setSelectedId(1);
    deckSelector.addListener(this);
    deckSelector.setColour(ComboBox::backgroundColourId, Colour(0xFF2d3035));
    deckSelector.setColour(ComboBox::outlineColourId, Colour(0xFF3d4148));
    deckSelector.setColour(ComboBox::textColourId, Colours::white);
    // Label for deck selector
    addAndMakeVisible(deckSelectorLabel);
    deckSelectorLabel.setFont(Font(14.0f));
    deckSelectorLabel.setColour(Label::textColourId, Colour(0xFFaaaaaa));
    deckSelectorLabel.setJustificationType(Justification::right);
}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint(Graphics& g)
{
    // Background
    g.fillAll(Colour(0xFF1e2024));

    // Title text
    g.setColour(Colour(0xFFf5a623));
    g.setFont(Font(18.0f).boldened());
    // remove from left the amount of space for the add button
    float titleX = getLocalBounds().removeFromLeft(40).getWidth();
    g.drawText("Playlist", titleX - 20, 0, getWidth() - titleX, 40, Justification::centred);
    g.drawText("Add song", titleX, 0, getWidth(), 40, Justification::left);
}

void PlaylistComponent::resized()
{
    auto area = getLocalBounds();

    // Top control area
    auto topArea = area.removeFromTop(40);

    // Position deck selector on the right and its label to the left of it
    deckSelector.setBounds(topArea.removeFromRight(100).reduced(5));
    deckSelectorLabel.setBounds(topArea.removeFromRight(120).reduced(5));

    addButton.setBounds(topArea.removeFromLeft(40).reduced(5));
    tableComponent.setBounds(area);
}

int PlaylistComponent::getNumRows()
{
    return tracks.size();
}

void PlaylistComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    // Alternating row colors
    if (rowIsSelected)
    {
        g.fillAll(Colour(0xFFf5a623).withAlpha(0.3f));
    }
    else if (rowNumber % 2)
    {
        g.fillAll(Colour(0xFF1a1c20));
    }
    else
    {
        g.fillAll(Colour(0xFF2d3035));
    }
}

void PlaylistComponent::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (rowNumber < getNumRows())
    {
        // Set text color - white for selected rows, gray for others

        g.setColour(rowIsSelected ? Colours::white : Colour(0xFFaaaaaa));
        g.setFont(14.0f);

        String text;

        switch (columnId)
        {
        case 1:
            text = tracks[rowNumber].title;
            break;

        case 2:
            text = tracks[rowNumber].artist;
            break;

        case 3:
            text = getTimeString(tracks[rowNumber].duration);
            break;

        default:
            break;
        }

        g.drawText(text, 2, 0, width - 4, height, Justification::centredLeft, true);
    }
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    // Only add button components to the "Load" column (column 4)
    if (columnId == 4)
    {
        if (existingComponentToUpdate == nullptr)
        {
            // Create a new button for loading this track
            TextButton* btn = new TextButton{ "Load" };

            // Store row number in the button's ID for identifying which track to load
            String id = String(rowNumber);
            btn->setComponentID(id);
            btn->addListener(this);

            // Style the button to match the app's look
            btn->setColour(TextButton::buttonColourId, Colour(0xFF2d3035));
            btn->setColour(TextButton::buttonOnColourId, Colour(0xFF3d4148));
            btn->setColour(TextButton::textColourOffId, Colour(0xFFf5a623));

            existingComponentToUpdate = btn;
        }

        // Return nullptr for columns that don't need a custom component

        return existingComponentToUpdate;
    }

    return nullptr;
}

void PlaylistComponent::buttonClicked(Button* button)
{
    if (button == &addButton)
    {
        // "+" button clicked - add new track to playlist
        addToPlaylist();
    }
    else
    {
        // This is a row "Load" button - get the row number from component ID
        int id = button->getComponentID().getIntValue();

        // Make sure the ID is valid
        if (id >= 0 && id < tracks.size())
        {
            // Get the selected deck
            int deckId = deckSelector.getSelectedId();
            DJAudioPlayer* playerToLoad = (deckId == 1) ? player1 : player2;
            DeckGUI* deckToLoad = (deckId == 1) ? deck1 : deck2;
            // Load the track to the selected deck
            if (playerToLoad != nullptr)
            {
                playerToLoad->loadURL(tracks[id].fileURL);
                deckToLoad->waveformDisplay.loadURL(tracks[id].fileURL);
                // Select the row to show which track is loaded
                tableComponent.selectRow(id);
            }
        }
    }
}

void PlaylistComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    // Handle changes to deck selection dropdown
// (Currently no action needed)

}

void PlaylistComponent::addToPlaylist()
{
    // Configure the file chooser dialog
    auto fileChooserFlags = FileBrowserComponent::canSelectFiles;

    // Launch asynchronously using the same pattern as in DeckGUI
    fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
    {
        auto result = chooser.getResult();

        if (result.existsAsFile())
        {
            URL fileURL = URL{ result };

            // Try to get metadata
            String title = result.getFileNameWithoutExtension();
            String artist = "Unknown Artist";

            // Get file duration
            double duration = 0.0;
            auto* reader = formatManager.createReaderFor(result);
            if (reader != nullptr)
            {
                duration = reader->lengthInSamples / reader->sampleRate;
                delete reader;
            }

            // Add to our tracks vector
            tracks.push_back(TrackInfo(title, artist, fileURL, duration));

            // Update the table
            tableComponent.updateContent();
            tableComponent.repaint();
        }
    });
}

String PlaylistComponent::getTimeString(double seconds)
{
    int minutes = (int)(seconds / 60);
    int remainingSeconds = (int)seconds % 60;

    return String::formatted("%02d:%02d", minutes, remainingSeconds);
}
