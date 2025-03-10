#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay(AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& thumbCacheToUse) : audioThumb(1000, formatManagerToUse, thumbCacheToUse), fileLoaded(false), position(0.0)
{
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint(Graphics& g)
{
    g.setColour(Colour(0xFF0B0F13)); // Darker background
    g.fillAll();

    if (fileLoaded)
    {
        // Create a slightly inset area for the waveform
        auto bounds = getLocalBounds().reduced(2);
        auto midPoint = bounds.getCentreY();

        // Draw the main waveform using AudioThumbnail
        g.setColour(Colour(0xFFf5a623).withAlpha(0.7f));
        audioThumb.drawChannel(g, bounds, 0.0, audioThumb.getTotalLength(), 0, 0.8f); // Scaling factor 0.8f for less extreme

        // Add a stylized effect underneath for visual richness
        drawStylizedWaveformBase(g, bounds);

        // Calculate playhead position
        int playheadX = bounds.getX() + position * bounds.getWidth();

        // Draw playhead line (vertical line showing current position)
        g.setColour(Colours::white);
        g.drawLine(playheadX, bounds.getY(), playheadX, bounds.getBottom(), 2.0f);

        // Draw playhead indicator at the top (small triangle)
        Path triangle;
        float triangleSize = 8.0f;
        triangle.addTriangle(playheadX - triangleSize / 2, bounds.getY(),
                            playheadX + triangleSize / 2, bounds.getY(),
                            playheadX, bounds.getY() + triangleSize);
        g.fillPath(triangle);

        // Draw time indicators
        g.setColour(Colour(0xFFaaaaaa));
        g.setFont(12.0f);
        g.drawText("0:00", 5, bounds.getBottom() - 15, 50, 15, Justification::left, false);

        // Calculate and display total time
        double totalSeconds = audioThumb.getTotalLength();
        int minutes = (int)(totalSeconds / 60);
        int seconds = (int)(totalSeconds) % 60;
        String timeString = String::formatted("%d:%02d", minutes, seconds);
        g.drawText(timeString, bounds.getWidth() - 50, bounds.getBottom() - 15, 45, 15, Justification::right, false);
    }
    else {
        // Message when no file is loaded
        g.setColour(Colour(0xFFaaaaaa)); // Light gray text
        g.setFont(16.0f);
        g.drawText("No track loaded", getLocalBounds(), Justification::centred, true);
    }
}

void WaveformDisplay::drawStylizedWaveformBase(Graphics& g, Rectangle<int> bounds)
{
    // Draw a smooth base under the actual waveform
    float midPoint = bounds.getCentreY();
    int width = bounds.getWidth();

    // Create path for the base waveform
    Path basePath;
    basePath.startNewSubPath(bounds.getX(), midPoint);

    // Draw a subtle sine-wave-like pattern under the actual waveform
    for (int i = 0; i <= width; i += 3)
    {
        float x = bounds.getX() + i;

        // Simple sine wave pattern with random variation
        float yOffset = std::sin(i * 0.05f) * bounds.getHeight() * 0.05f;
        basePath.lineTo(x, midPoint + yOffset);
    }

    // Connect back to starting point via bottom of bounds
    basePath.lineTo(bounds.getRight(), bounds.getBottom());
    basePath.lineTo(bounds.getX(), bounds.getBottom());
    basePath.closeSubPath();

    // Fill with a subtle color
    g.setColour(Colour(0xFFf5a623).withAlpha(0.15f));
    g.fillPath(basePath);
}

void WaveformDisplay::resized()
{
}

void WaveformDisplay::loadURL(URL audioURL)
{
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));
    if (fileLoaded)
    {
        position = 0.0;
    }

    repaint();
}

void WaveformDisplay::changeListenerCallback(ChangeBroadcaster* source)
{
    repaint();
}

void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position && !std::isnan(pos))
    {
        position = pos;
        repaint();
    }
}