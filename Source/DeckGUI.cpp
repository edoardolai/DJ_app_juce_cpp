#include "DeckGUI.h"
#include "JuceHeader.h"
DeckGUI::DeckGUI(DJAudioPlayer* _player, AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& thumbCacheToUse)
    : player(_player), waveformDisplay(formatManagerToUse, thumbCacheToUse)
{

    // ===== COMPONENT INITIALIZATION AND VISIBILITY =====
    // Make the deck label visible and style it
    addAndMakeVisible(deckLabel);
    deckLabel.setFont(Font(18.0f, Font::bold));
    deckLabel.setJustificationType(Justification::centred);
    deckLabel.setColour(Label::textColourId, Colour(0xFFf5a623));

    // Add all components to the deck's visual hierarchy
    addAndMakeVisible(waveformDisplay);  // Waveform visualization
    addAndMakeVisible(playPauseButton);  // Transport controls
    addAndMakeVisible(loadButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(volSlider);        // Audio controls
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(positionSlider);
    addAndMakeVisible(vinylSlider);      // Vinyl emulation
    addAndMakeVisible(volLabel);         // Labels
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(highEQSlider);     // EQ controls
    addAndMakeVisible(midEQSlider);
    addAndMakeVisible(lowEQSlider);
    addAndMakeVisible(eqToggleButton);
    addAndMakeVisible(highLabel);        // EQ labels
    addAndMakeVisible(midLabel);
    addAndMakeVisible(lowLabel);

    // ===== TRANSPORT CONTROLS SETUP =====
    // Set up the play/pause button
    playPauseButton.setState(Button::buttonNormal);
    playPauseButton.setLookAndFeel(&djDeckLookAndFeel);
    playPauseButton.addListener(this);

    // Configure load button
    loadButton.setLookAndFeel(&djDeckLookAndFeel);
    loadButton.addListener(this);

    // Set up loop button with toggle behavior
    loopButton.setLookAndFeel(&djDeckLookAndFeel);
    loopButton.addListener(this);
    loopButton.setClickingTogglesState(true);

    // ===== VOLUME AND SPEED CONTROLS =====
    // Configure rotary volume control
    volSlider.setSliderStyle(Slider::Rotary);
    volSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    volSlider.setRotaryParameters(float_Pi * 1.2f, float_Pi * 2.8f, true);
    volSlider.setDoubleClickReturnValue(true, 0.5);  // Double-click resets to 50% volume
    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(0.5);
    volSlider.setLookAndFeel(&djDeckLookAndFeel);
    volSlider.addListener(this);

    // Style volume label
    volLabel.setFont(Font(14.0f));
    volLabel.setColour(Label::textColourId, Colour(0xFFaaaaaa));

    // Configure rotary speed control
    speedSlider.setSliderStyle(Slider::Rotary);
    speedSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    speedSlider.setRotaryParameters(float_Pi * 1.2f, float_Pi * 2.8f, true);
    speedSlider.setDoubleClickReturnValue(true, 1.0);  // Double-click resets to normal speed
    speedSlider.setRange(0.0, 3.0);  // 0x to 3x speed
    speedSlider.setValue(1.0);
    speedSlider.setLookAndFeel(&djDeckLookAndFeel);
    speedSlider.addListener(this);

    // Style speed label
    speedLabel.setFont(Font(14.0f));
    speedLabel.setColour(Label::textColourId, Colour(0xFFaaaaaa));

    // Configure position slider
    positionSlider.setRange(0.0, 1.0);
    positionSlider.addListener(this);

    // ===== VINYL CONTROL SETUP =====
    // Configure vinyl-style turntable control
    vinylSlider.setSliderStyle(Slider::Rotary);
    vinylSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    vinylSlider.setRotaryParameters(0.0f, MathConstants<float>::twoPi, true);
    vinylSlider.setName("vinyl");  // Used by look and feel for special rendering
    vinylSlider.setRange(0.0, 1.0, 0.001);  // Fine-grained control
    vinylSlider.setValue(0.0, dontSendNotification);
    vinylSlider.setVisible(true);
    vinylSlider.setLookAndFeel(&djDeckLookAndFeel);
    vinylSlider.addListener(this);

    // ===== EQ SECTION SETUP =====
    // Configure EQ toggle button
    eqToggleButton.setClickingTogglesState(true);
    eqToggleButton.setLookAndFeel(&djDeckLookAndFeel);
    eqToggleButton.addListener(this);
    eqToggleButton.setToggleState(true, dontSendNotification);  // EQ enabled by default

    // Set up the three-band EQ sliders (high, mid, low)
    // High frequency EQ
    highEQSlider.setSliderStyle(Slider::LinearVertical);
    highEQSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    highEQSlider.setRange(0.0, 2.0);  // 0=cut, 1=neutral, 2=boost
    highEQSlider.setValue(1.0);  // Start at neutral position
    highEQSlider.setDoubleClickReturnValue(true, 1.0);
    highEQSlider.setLookAndFeel(&djDeckLookAndFeel);
    highEQSlider.addListener(this);

    // Mid frequency EQ
    midEQSlider.setSliderStyle(Slider::LinearVertical);
    midEQSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    midEQSlider.setRange(0.0, 2.0);
    midEQSlider.setValue(1.0);
    midEQSlider.setDoubleClickReturnValue(true, 1.0);
    midEQSlider.setLookAndFeel(&djDeckLookAndFeel);
    midEQSlider.addListener(this);

    // Low frequency EQ
    lowEQSlider.setSliderStyle(Slider::LinearVertical);
    lowEQSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    lowEQSlider.setRange(0.0, 2.0);
    lowEQSlider.setValue(1.0);
    lowEQSlider.setDoubleClickReturnValue(true, 1.0);
    lowEQSlider.setLookAndFeel(&djDeckLookAndFeel);
    lowEQSlider.addListener(this);

    // Style EQ labels
    highLabel.setFont(Font(14.0f));
    midLabel.setFont(Font(14.0f));
    lowLabel.setFont(Font(14.0f));

    highLabel.setJustificationType(Justification::centred);
    midLabel.setJustificationType(Justification::centred);
    lowLabel.setJustificationType(Justification::centred);

    highLabel.setColour(Label::textColourId, Colour(0xFFaaaaaa));
    midLabel.setColour(Label::textColourId, Colour(0xFFaaaaaa));
    lowLabel.setColour(Label::textColourId, Colour(0xFFaaaaaa));

    // ===== INITIALIZE TIMER =====
    // Start timer to update waveform position and vinyl rotation (~60fps)
    startTimer(16);
}
DeckGUI::~DeckGUI() {
    stopTimer();
}

void DeckGUI::paint(Graphics& g) {
    //Gradient background for more realistic DJ deck look
    ColourGradient gradient(
        Colour(0xFF202020), 0, 0,
        Colour(0xFF101010), 0, getHeight(),
        false
    );
    g.setGradientFill(gradient);
    g.fillAll();

    // Add overlay pattern
    g.setOpacity(0.05f);
    for (int y = 0; y < getHeight(); y += 4) {
        g.setColour(Colours::white.withAlpha(0.1f));
        g.drawHorizontalLine(y, 0, getWidth());
    }

    // Tint for distinguishing decks
    if (deckId == 0) {
        // Left deck - very slight blue tint
        g.setColour(Colours::blue.withAlpha(0.02f));
    }
    else {
        // Right deck - very slight red tint
        g.setColour(Colours::red.withAlpha(0.02f));
    }
    g.fillAll();
}

void DeckGUI::resized()
{
    // Create area with margins
    auto area = getLocalBounds().reduced(10);

    // Position waveform
    waveformDisplay.setBounds(area.removeFromTop(120));
    area.removeFromTop(10); // Spacing

    // Reserve button area
    auto buttonArea = area.removeFromBottom(60);

    // Create columns for controls
    int leftWidth = area.getWidth() / 6;
    int rightWidth = area.getWidth() / 5;

    auto leftColumn = area.removeFromLeft(leftWidth);
    auto rightColumn = area.removeFromRight(rightWidth);
    // Center area remains for vinyl

    // Position vinyl control in center
    int vinylSize = jmin(area.getWidth() - 40, area.getHeight() - 40);
    vinylSlider.setBounds(
        area.getCentreX() - vinylSize / 2,
        area.getCentreY() - vinylSize / 2,
        vinylSize, vinylSize
    );

    // Position controls in left column
    auto originalLeftColumn = leftColumn;
    leftColumn.removeFromTop(100); // Align with vinyl

    // Position deck label
    int labelHeight = 30;
    deckLabel.setBounds(originalLeftColumn.getX(), originalLeftColumn.getY() + 10,
                       leftWidth, labelHeight);

    // Size for controls
    int controlHeight = jmin(95, leftColumn.getHeight() / 3);
    int labelHeight2 = 30;

    // Position volume control
    volSlider.setBounds(leftColumn.removeFromTop(controlHeight).reduced(8, 5));
    volLabel.setBounds(volSlider.getX(), volSlider.getBottom() + 5,
                      volSlider.getWidth(), labelHeight2);
    volLabel.setJustificationType(Justification::centred);

    leftColumn.removeFromTop(25); // Spacing

    // Position speed control
    speedSlider.setBounds(leftColumn.removeFromTop(controlHeight).reduced(8, 5));
    speedLabel.setBounds(speedSlider.getX(), speedSlider.getBottom() + 5,
                        speedSlider.getWidth(), labelHeight2);
    speedLabel.setJustificationType(Justification::centred);

    // Set up EQ section
    int eqWidth = rightColumn.getWidth() / 4;
    int eqLabelHeight = 25;
    int eqPadding = (eqWidth - 15) / 2;

    // Position EQ sliders and labels
    auto highArea = rightColumn.removeFromLeft(eqWidth);
    highEQSlider.setBounds(highArea.withTrimmedBottom(eqLabelHeight).reduced(eqPadding, 0));
    highLabel.setBounds(highArea.getX(), highArea.getBottom() - eqLabelHeight,
                       highArea.getWidth(), eqLabelHeight);

    auto midArea = rightColumn.removeFromLeft(eqWidth);
    midEQSlider.setBounds(midArea.withTrimmedBottom(eqLabelHeight).reduced(eqPadding, 0));
    midLabel.setBounds(midArea.getX(), midArea.getBottom() - eqLabelHeight,
                      midArea.getWidth(), eqLabelHeight);

    auto lowArea = rightColumn.removeFromLeft(eqWidth);
    lowEQSlider.setBounds(lowArea.withTrimmedBottom(eqLabelHeight).reduced(eqPadding, 0));
    lowLabel.setBounds(lowArea.getX(), lowArea.getBottom() - eqLabelHeight,
                      lowArea.getWidth(), eqLabelHeight);

    // Position transport buttons
    int leftOffset = 50;
    int transportWidth = buttonArea.getWidth() * 0.8;

    Rectangle<int> adjustedButtonArea(
        buttonArea.getX() + leftOffset,
        buttonArea.getY(),
        transportWidth - leftOffset,
        buttonArea.getHeight()
    );

    int buttonGap = 3;
    int buttonWidth = (adjustedButtonArea.getWidth() - (buttonGap * 3)) / 4;
    int xPos = adjustedButtonArea.getX();

    // Position buttons in sequence
    loadButton.setBounds(xPos, adjustedButtonArea.getY(), buttonWidth, adjustedButtonArea.getHeight());
    xPos += buttonWidth + buttonGap;

    loopButton.setBounds(xPos, adjustedButtonArea.getY(), buttonWidth, adjustedButtonArea.getHeight());
    xPos += buttonWidth + buttonGap;

    playPauseButton.setBounds(xPos, adjustedButtonArea.getY(), buttonWidth, adjustedButtonArea.getHeight());
    xPos += buttonWidth + buttonGap;

    eqToggleButton.setBounds(xPos, adjustedButtonArea.getY(), buttonWidth, adjustedButtonArea.getHeight());
}



void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playPauseButton) {
        if (player->playing()) {
            player->stop();
        }
        else {
            player->start();
        }
        button->setToggleState(player->playing(), dontSendNotification);

        // Force vinyl slider repaint when play state changes
        vinylSlider.repaint();
    }

    if (button == &loadButton)
    {
        // Open file chooser to load a track
        auto fileChooserFlags = FileBrowserComponent::canSelectFiles;

        fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
        {
            player->loadURL(URL{ chooser.getResult() });
            waveformDisplay.loadURL(URL{ chooser.getResult() });
        });
    }


    if (button == &eqToggleButton)
    {
        bool eqEnabled = eqToggleButton.getToggleState();

        // Enable/disable the EQ sliders
        highEQSlider.setEnabled(eqEnabled);
        midEQSlider.setEnabled(eqEnabled);
        lowEQSlider.setEnabled(eqEnabled);

        // Change opacity to give visual feedback
        highEQSlider.setAlpha(eqEnabled ? 1.0f : 0.5f);
        midEQSlider.setAlpha(eqEnabled ? 1.0f : 0.5f);
        lowEQSlider.setAlpha(eqEnabled ? 1.0f : 0.5f);

        // If turning off EQ, reset the sliders and EQ settings
        if (!eqEnabled)
        {
            highEQSlider.setValue(1.0, dontSendNotification);
            midEQSlider.setValue(1.0, dontSendNotification);
            lowEQSlider.setValue(1.0, dontSendNotification);

            // Reset audio processing (it is very difficult to find the right values manually)
            player->resetEQ();
        }
    }
    if (button == &loopButton)
    {
        isLooping = loopButton.getToggleState();
    }


}

void DeckGUI::sliderValueChanged(Slider* slider) {
    if (slider == &volSlider) {
        player->setGain(slider->getValue());
    }
    else if (slider == &speedSlider) {
        player->setSpeed(slider->getValue());
    }
    else if (slider == &positionSlider) {
        player->setPositionRelative(slider->getValue());
    }
    else if (slider == &vinylSlider) {
        // Only update position when being dragged
        if (isVinylBeingDragged) {
            player->setPositionRelative(slider->getValue());
        }
    }

    // EQ controls
    else if (slider == &highEQSlider) {
        player->setHighGain(slider->getValue());
    }
    else if (slider == &midEQSlider) {
        player->setMidGain(slider->getValue());
    }
    else if (slider == &lowEQSlider) {
        player->setLowGain(slider->getValue());
    }
}

void DeckGUI::sliderDragStarted(Slider* slider) {
    if (slider == &vinylSlider) {
        isVinylBeingDragged = true;
    }
}

void DeckGUI::sliderDragEnded(Slider* slider) {
    if (slider == &vinylSlider) {
        isVinylBeingDragged = false;
        if (player->getPositionRelative() > 0.0)
        {
            player->setPositionRelative(slider->getValue());
        }
    }
}


bool DeckGUI::isInterestedInFileDrag(const StringArray& files) {
    return true;
}

void DeckGUI::filesDropped(const StringArray& files, int x, int y) {
    {
        for (String filename : files)
        {
            URL fileURL = URL{ File{filename} };
            player->loadURL(fileURL);
            waveformDisplay.loadURL(fileURL);
            return;
        }
    }
}


void DeckGUI::timerCallback() {
    double currentPosition = player->getPositionRelative();

    // Ensure currentPosition is valid (between 0 and 1), fallback to 0 if NaN
    if (std::isnan(currentPosition))
        currentPosition = 0.0f;
    else
        currentPosition = jlimit(0.0, 1.0, currentPosition);

    // Update UI components
    waveformDisplay.setPositionRelative(currentPosition);

    // Only update position slider if not being dragged
    if (!isVinylBeingDragged)
    {
        vinylSlider.setValue(currentPosition, dontSendNotification);
    }

    // Handle track end
    if (currentPosition >= 0.999)
    {
        if (isLooping)
        {
            player->setPositionRelative(0.0);
            // If not already playing, start playback
            if (!player->playing())
            {
                player->start();
                playPauseButton.setToggleState(true, dontSendNotification);
            }
        }
        else
        {
            // Stop at end if not looping
            player->setPositionRelative(0.0);
            player->stop();
            playPauseButton.setToggleState(false, dontSendNotification);
        }
    }

    // Make sure the slider is visible and force repaint
    vinylSlider.repaint();

}

void DeckGUI::setDeckId(int id)
{
    deckId = id;

    // Set appropriate deck label text
    if (deckId == 0) {
        deckLabel.setText("DECK A", dontSendNotification);
    }
    else {
        deckLabel.setText("DECK B", dontSendNotification);
    }
}