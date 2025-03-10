/*
  ==============================================================================
    Main component containing the DJ decks, crossfader, and playlist.
  ==============================================================================
*/
using namespace std;
#include "MainComponent.h"
//==============================================================================
MainComponent::MainComponent() : AudioAppComponent::AudioAppComponent(), Slider::Listener()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize(1200, 800);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired(RuntimePermissions::recordAudio)
        && !RuntimePermissions::isGranted(RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request(RuntimePermissions::recordAudio,
                                     [&](bool granted) { if (granted)  setAudioChannels(2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(0, 2, nullptr);
    }
    // Set deck IDs for visual distinction
    deck1.setDeckId(0);  // Left deck
    deck2.setDeckId(1);  // Right deck

    // Add components to the UI
    addAndMakeVisible(deck1);
    addAndMakeVisible(deck2);
    addAndMakeVisible(playlistComponent);
    addAndMakeVisible(crossfader);

    // Configure crossfader
    crossfader.setSliderStyle(Slider::LinearHorizontal);
    crossfader.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    crossfader.setRange(0.0, 1.0);  // 0 = full left deck, 1 = full right deck
    crossfader.setValue(0.5);
    crossfader.addListener(this);
    crossfader.setLookAndFeel(&crossfaderLookAndFeel);
    crossfader.setName("crossfader");
    addAndMakeVisible(crossfaderLabel);
    crossfaderLabel.setText("CROSSFADER", dontSendNotification);
    crossfaderLabel.setFont(Font(14.0f, Font::bold));
    crossfaderLabel.setJustificationType(Justification::centred);
    crossfaderLabel.setColour(Label::textColourId, Colours::white.withAlpha(0.7f));

    // Register basic audio formats for playback
    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    // Clean up look and feel
    crossfader.setLookAndFeel(nullptr);
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Add both deck players to the mixer
    mixerAudioSource.addInputSource(&player1, false);
    mixerAudioSource.addInputSource(&player2, false);
    mixerAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    // play audio loaded in the transport source
    mixerAudioSource.getNextAudioBlock(bufferToFill);

}



void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    mixerAudioSource.removeAllInputs();
    mixerAudioSource.releaseResources();
    player1.releaseResources();
    player2.releaseResources();
}

//==============================================================================
void MainComponent::paint(Graphics& g)
{
    // Draw dark gradient background
    ColourGradient gradient(Colour(0xFF252525), 0, 0,
                           Colour(0xFF101010), 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();

    // Add subtle texture with horizontal lines
    g.setOpacity(0.05f);
    for (int y = 0; y < getHeight(); y += 4) {
        g.setColour(Colours::white.withAlpha(0.1f));
        g.drawHorizontalLine(y, 0, getWidth());
    }

    // Borders and dividers
    g.setColour(Colours::white.withAlpha(0.2f));
    g.drawVerticalLine(getWidth() / 2, 0, deck1.getBottom());
    g.setColour(Colours::white.withAlpha(0.1f));
    g.drawHorizontalLine(deck1.getBottom(), 0, getWidth());
    g.drawHorizontalLine(crossfader.getBottom() + 5, 0, getWidth());
}
void MainComponent::resized()
{
    // Position the decks side by side at the top
    int deckHeight = getHeight() / 4 * 3 - 100;
    deck1.setBounds(0, 0, getWidth() / 2, deckHeight);
    deck2.setBounds(getWidth() / 2, 0, getWidth() / 2, deckHeight);

    // Position crossfader centered below the decks
    int labelHeight = 20;
    int crossfaderHeight = 20;
    int crossfaderWidth = getWidth() / 2;
    int crossfaderX = (getWidth() - crossfaderWidth) / 2; // Center horizontally
    int crossfaderY = deckHeight + 10;

    // Position crossfader below its label
    crossfaderLabel.setBounds(crossfaderX, crossfaderY, crossfaderWidth, labelHeight);
    crossfader.setBounds(crossfaderX, crossfaderY + labelHeight, crossfaderWidth, crossfaderHeight);

    // Position playlist after crossfader
    int playlistY = crossfader.getBottom() + 5;
    int playlistHeight = getHeight() - playlistY;
    playlistComponent.setBounds(0, playlistY, getWidth(), playlistHeight);
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    if (slider == &crossfader)
    {
        double value = crossfader.getValue();

        // Calculate gains for each deck based on crossfader position
        double gain1 = 1.0 - value; // Left deck (full at 0, silent at 1)
        double gain2 = value;       // Right deck (silent at 0, full at 1)

        // Set gains
        player1.setGain(gain1);
        player2.setGain(gain2);
    }
}