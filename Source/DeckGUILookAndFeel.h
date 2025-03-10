#pragma once
#include <JuceHeader.h>
/*
 * Custom look and feel class that defines visual styling for the deck components.
 * Overrides standard JUCE drawing methods to create DJ-specific visual elements.
 */
class DeckGUILookAndFeel : public LookAndFeel_V4 {
public:
    DeckGUILookAndFeel();
    ~DeckGUILookAndFeel() override;
    // Override to customize knob and dial appearance
    void drawRotarySlider(Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float rotaryStartAngle,
        float rotaryEndAngle,
        Slider&
    ) override;

    // Override to customize button backgrounds
    void drawButtonBackground(Graphics& g,
        Button& button,
        const Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown
    ) override;

    // Override to customize button text and draw play/pause symbols
    void drawButtonText(
        Graphics& g,
        TextButton& button,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown
    ) override;

    // Helper method for drawing vinyl-styled rotary control
    void drawRotatoryVinyl(
        Graphics& g,
        int x, int y, int width, int height,
        float sliderPos,
        const float rotaryStartAngle,
        const float rotaryEndAngle,
        Slider& slider);

    // Override to customize vertical and horizontal sliders
    void drawLinearSlider(Graphics& g,
        int x, int y, int width, int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        const Slider::SliderStyle style,
        Slider& slider) override;


};