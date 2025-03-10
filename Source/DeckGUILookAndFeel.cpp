#include "DeckGUILookAndFeel.h"

DeckGUILookAndFeel::DeckGUILookAndFeel() {
    // Constructor - no initialization needed

}

DeckGUILookAndFeel::~DeckGUILookAndFeel() {
    // Destructor - no cleanup needed

}

void DeckGUILookAndFeel::drawRotatoryVinyl(Graphics& g,
    int x, int y, int width, int height,
    float sliderPos,
    const float rotaryStartAngle,
    const float rotaryEndAngle,
    Slider& slider)
{
    // Safety check for valid dimensions
    if (width <= 0 || height <= 0)
        return;

    // Ensure position is valid (even when no track is loaded)
    if (std::isnan(sliderPos))
        sliderPos = 0.0f;
    else
        sliderPos = jlimit(0.0f, 1.0f, sliderPos);

    // Calculate dimensions for the vinyl disc
    auto radius = (float)jmin(width / 2, height / 2) - 4.0f;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centreY = (float)y + (float)height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;

    // Calculate rotation angle - Start from top (North) position
    const float startingAngle = MathConstants<float>::pi;
    auto angle = startingAngle + sliderPos * MathConstants<float>::twoPi;

    // Draw vinyl record base (black disc)
    g.setColour(Colours::black);
    g.fillEllipse(rx, ry, rw, rw);

    // Draw outer ring
    g.setColour(Colour(0xFF3d4148));
    g.drawEllipse(rx, ry, rw, rw, 5.0f);

    // Draw record grooves (concentric circles)
    g.setColour(Colour(0xFF3d4148).withAlpha(0.7f));

    // Create multiple grooves with varying opacity for realistic appearance
    for (float i = 0.9f; i > 0.35f; i -= 0.035f) {
        float grooveRadius = radius * i;
        // Slightly vary opacity for more realistic look
        float opacity = 0.4f + (i - 0.35f) * 0.1f;
        g.setColour(Colour(0xFF3d4148).withAlpha(opacity));
        g.drawEllipse(centreX - grooveRadius, centreY - grooveRadius,
                     grooveRadius * 2.0f, grooveRadius * 2.0f,
                     0.8f);
    }

    // Draw center label (colored area in middle of vinyl)
    float labelRadius = radius * 0.35f;
    g.setColour(Colours::darkgrey);
    g.fillEllipse(centreX - labelRadius, centreY - labelRadius,
                 labelRadius * 2.0f, labelRadius * 2.0f);

    // Draw spindle hole
    float holeRadius = radius * 0.1f;
    g.setColour(Colours::black);
    g.fillEllipse(centreX - holeRadius, centreY - holeRadius,
                 holeRadius * 2.0f, holeRadius * 2.0f);

    Path p;
    auto pointerThickness = 3.0f;
    float outerPointerStart = -radius;
    float outerPointerLength = radius * 0.65f; // Stops before the label

    p.addRectangle(-pointerThickness * 0.5f, outerPointerStart,
                  pointerThickness, outerPointerLength);

    p.addRectangle(-pointerThickness * 0.5f, labelRadius,
                  pointerThickness, outerPointerLength);

    // Rotate line to current playback position
    p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));

    // Draw white position indicator
    g.setColour(Colours::white);
    g.fillPath(p);
}

// Main rotary slider drawing method - handles both standard knobs and vinyl control
void DeckGUILookAndFeel::drawRotarySlider(
   Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPos,
    const float rotaryStartAngle,
    const float rotaryEndAngle,
    Slider& slider
) {
    // Special handling for vinyl control
    if (slider.getName() == "vinyl")
    {
        drawRotatoryVinyl(g, x, y, width, height, sliderPos, rotaryStartAngle, rotaryEndAngle, slider);
        return;
    }
    // Standard rotary control drawing (for volume, speed, etc.)

    auto radius = (float)jmin(width / 2, height / 2) - 4.0f;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centreY = (float)y + (float)height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Draw knob background with gradient
    Colour baseColour = Colour(0xFF505050);

    ColourGradient gradient(
        baseColour.darker(0.2f), centreX, centreY - radius,
        baseColour.darker(0.8f), centreX, centreY + radius,
        false
    );
    g.setGradientFill(gradient);
    g.fillEllipse(rx, ry, rw, rw);

    // Draw knob border
    g.setColour(baseColour.darker(0.6f));
    g.drawEllipse(rx, ry, rw, rw, 1.0f);


    // Draw pointer to show current position
    Path p;
    auto pointerLength = radius * 0.7f;
    auto pointerThickness = 2.0f;
    p.addRectangle(-pointerThickness * 0.5f, -radius * 0.9f, pointerThickness, pointerLength);
    p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));

    g.setColour(Colour(0xFFd58c1c));
    g.fillPath(p);
    g.strokePath(p, PathStrokeType(1.0f));
}

// Custom button background drawing for all buttons
void DeckGUILookAndFeel::drawButtonBackground(Graphics& g,
    Button& button,
    const Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    Rectangle<float> bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    float cornerSize = 4.0f;
    Colour baseColour = Colour(0xFF505050);


    // Adjust color based on state
    if (shouldDrawButtonAsDown) {
        baseColour = baseColour.darker(0.2f);
    }
    else if (shouldDrawButtonAsHighlighted) {
        baseColour = baseColour.brighter(0.2f);
    }

    // Draw button with gradient
    ColourGradient gradient(baseColour.brighter(0.2f), 0, bounds.getY(),
                           baseColour.darker(0.2f), 0, bounds.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, cornerSize);

    // Draw border
    g.setColour(baseColour.darker(0.4f));
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);

}

// Special handling for button text and icons
void DeckGUILookAndFeel::drawButtonText(Graphics& g,
    TextButton& button,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    // Get button dimensions
    auto bounds = button.getLocalBounds().toFloat();
    auto diameter = jmin(bounds.getWidth(), bounds.getHeight());
    auto radius = diameter / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();

    // Special handling for play/pause button
    if (button.getButtonText() == "PLAY")
    {
        // Play/Pause button
        bool isPlaying = button.getToggleState();
        g.setColour(Colour(0xFF4cd964));

        if (isPlaying)
        {
            // Draw pause symbol (two vertical bars)
            float barWidth = radius * 0.3f;
            float barHeight = radius * 0.8f;
            float spacing = radius * 0.15f;

            g.fillRect(centreX - spacing - barWidth, centreY - barHeight / 2,
                      barWidth, barHeight);
            g.fillRect(centreX + spacing, centreY - barHeight / 2,
                      barWidth, barHeight);
        }
        else
        {
            // Draw play symbol (triangle pointing right)
            float triangleSize = radius;
            Path playSymbol;
            playSymbol.addTriangle(centreX - triangleSize * 0.3f, centreY - triangleSize * 0.5f,
                                  centreX - triangleSize * 0.3f, centreY + triangleSize * 0.5f,
                                  centreX + triangleSize * 0.5f, centreY);

            g.fillPath(playSymbol);
        }
    }
    else
    {
        // All other buttons get consistent styling
        bool isToggled = button.getToggleState();

        // Use orange when active, white when inactive
        g.setColour(isToggled ? Colour(0xFFf5a623) : Colours::white);

        // Draw text centered in button
        g.setFont(16.0f);
        g.drawText(button.getButtonText(), button.getLocalBounds(),
                  Justification::centred, true);
    }
}

// Custom drawing for linear sliders (e.g., EQ controls)
void DeckGUILookAndFeel::drawLinearSlider(Graphics& g,
    int x, int y, int width, int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    const Slider::SliderStyle style,
    Slider& slider)
{
    if (style == Slider::LinearVertical)
    {
        // Vertical sliders (like EQ)

        // Draw track background
        g.setColour(Colour(0xFF2d3035));
        g.fillRect(x, y, width, height);

        // Draw track outline
        g.setColour(Colour(0xFF3d4148));
        g.drawRect(x, y, width, height, 1);

        // Calculate thumb position and size
        float thumbHeight = width * 1.5f;
        float thumbY = sliderPos - thumbHeight / 2.0f;

        g.setColour(Colour(0xFFf5a623));
        g.fillRect((float)x, thumbY, (float)width, thumbHeight);
        g.setColour(Colour(0xFFe09c1c));
        g.drawRect((float)x, thumbY, (float)width, thumbHeight, 1.0f);
    }
    else // LinearHorizontal (e.g., crossfader)
    {
        // Draw track
        g.setColour(Colour(0xFF2d3035));
        g.fillRect(x, y, width, height);

        // Draw track outline
        g.setColour(Colour(0xFF3d4148));
        g.drawRect(x, y, width, height, 1);

        // Calculate thumb position and size
        float thumbWidth = height * 1.5f;
        float thumbX = sliderPos - thumbWidth / 2.0f;

        g.setColour(Colour(0xFFf5a623));
        g.fillRect(thumbX, (float)y, thumbWidth, (float)height);
        g.setColour(Colour(0xFFe09c1c));
        g.drawRect(thumbX, (float)y, thumbWidth, (float)height, 1.0f);
    }
}
