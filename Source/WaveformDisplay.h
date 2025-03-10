#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
/**
 * Component that displays a waveform visualization of an audio file.
 * Shows playback position and allows for visual tracking of the current track.
 */
class WaveformDisplay : public Component,
    public ChangeListener
{
public:
    WaveformDisplay(AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& thumbCacheToUse);
    ~WaveformDisplay() override;

    void paint(Graphics& g) override;
    void resized() override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    void loadURL(URL audioURL);
    void setPositionRelative(double pos);
    // Helper method for enhanced waveform visual effect
    void drawStylizedWaveformBase(Graphics& g, Rectangle<int> bounds);
private:
    AudioThumbnail audioThumb;
    bool fileLoaded;
    double position;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};