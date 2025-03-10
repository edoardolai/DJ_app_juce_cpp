#pragma once
using namespace std;
#include "../JuceLibraryCode/JuceHeader.h"

/**
 * Handles audio playback with DJ-style controls including
 * speed adjustment, volume control, and 3-band EQ.
 * Inherits from AudioSource to integrate with JUCE's audio pipeline.
 */
class DJAudioPlayer : public AudioSource {

public:
    // Constructor takes a reference to format manager for loading different audio formats
    DJAudioPlayer(AudioFormatManager& _formatManager);
    ~DJAudioPlayer() override;

    // ==== AudioSource interface methods ====
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // ==== Track loading and playback control ====
    void loadURL(URL audioURL);
    void setGain(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);

    // ==== EQ control functions ====
    // Adjust band gains (1.0 = neutral, <1.0 = cut, >1.0 = boost)
    void setHighGain(double gain);
    void setMidGain(double gain);
    void setLowGain(double gain);

    // ==== Playback state control ====
    void start();
    void stop();
    double getPositionRelative();
    bool playing();
    // Reset all EQ bands to neutral (1.0)
    void resetEQ();

private:
    // Reference to the format manager for loading audio files
    AudioFormatManager& formatManager;

    // Audio source chain for playback
    unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;  // Handles playback, seeking, etc.
    ResamplingAudioSource resampleSource{ &transportSource, false, 2 };  // For speed control

    // EQ filters for 3-band equalization
    IIRFilter lowFilter;
    IIRFilter midFilter;
    IIRFilter highFilter;

    // Audio parameters
    double sampleRate = 44100.0;  // Default sample rate
    bool filtersInitialized = false;

    double lowGain = 1.0;
    double midGain = 1.0;
    double highGain = 1.0;

    // Helper method to initialize filters with current sample rate
    void updateFilters();
};