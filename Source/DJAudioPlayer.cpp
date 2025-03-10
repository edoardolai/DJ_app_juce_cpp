#include "DJAudioPlayer.h"
using namespace std;

DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager)
    : formatManager(_formatManager)
{
    // No other initialization needed - set up of the audio chain will be done when prepareToPlay is called
}

DJAudioPlayer::~DJAudioPlayer()
{
    // Smart pointers handle cleanup of the audio sources
}

void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double _sampleRate)
{
    // Pass preparation call down the audio source chain
    transportSource.prepareToPlay(samplesPerBlockExpected, _sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, _sampleRate);

    // Store sample rate for filter calculations
    sampleRate = _sampleRate;

    // Set up the EQ filters with crossover frequencies
    updateFilters();
    filtersInitialized = true;
}

void DJAudioPlayer::updateFilters()
{
    // Set up 3-band EQ with standard DJ-style crossover points

    // Low band: everything below 300 Hz (bass)
    lowFilter.setCoefficients(IIRCoefficients::makeLowPass(sampleRate, 300));

    // Mid band: centered around 1200 Hz with Q factor 0.7
    midFilter.setCoefficients(IIRCoefficients::makeBandPass(sampleRate, 1200, 0.7));

    // High band: everything above 2500 Hz (treble)
    highFilter.setCoefficients(IIRCoefficients::makeHighPass(sampleRate, 2500));
}

void DJAudioPlayer::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    // First get the audio from the resampler (which handles our speed control)
    resampleSource.getNextAudioBlock(bufferToFill);

    // Skip EQ processing if filters aren't initialized or if all bands are neutral (allows value reset when disabling EQ)
    if (!filtersInitialized || (lowGain == 1.0 && midGain == 1.0 && highGain == 1.0))
        return;

    // Get reference to the current audio buffer
    AudioBuffer<float>& buffer = *bufferToFill.buffer;

    // Process each channel separately (for stereo or mono)
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        // Create temporary buffers for each frequency band
        AudioBuffer<float> lowBand(1, buffer.getNumSamples());
        AudioBuffer<float> midBand(1, buffer.getNumSamples());
        AudioBuffer<float> highBand(1, buffer.getNumSamples());

        // Copy source audio to each band buffer
        lowBand.copyFrom(0, 0, buffer, channel, bufferToFill.startSample, buffer.getNumSamples());
        midBand.copyFrom(0, 0, buffer, channel, bufferToFill.startSample, buffer.getNumSamples());
        highBand.copyFrom(0, 0, buffer, channel, bufferToFill.startSample, buffer.getNumSamples());

        // Apply the appropriate filter to each band
        lowFilter.processSamples(lowBand.getWritePointer(0), lowBand.getNumSamples());
        midFilter.processSamples(midBand.getWritePointer(0), midBand.getNumSamples());
        highFilter.processSamples(highBand.getWritePointer(0), highBand.getNumSamples());

        // Apply gain adjustment to each band
        lowBand.applyGain(lowGain);
        midBand.applyGain(midGain);
        highBand.applyGain(highGain);

        // Clear the original channel processed bands can be mixed back in
        buffer.clear(channel, bufferToFill.startSample, buffer.getNumSamples());

        // Mix the filtered bands back into the original buffer
        buffer.addFrom(channel, bufferToFill.startSample, lowBand, 0, 0, buffer.getNumSamples());
        buffer.addFrom(channel, bufferToFill.startSample, midBand, 0, 0, buffer.getNumSamples());
        buffer.addFrom(channel, bufferToFill.startSample, highBand, 0, 0, buffer.getNumSamples());
    }
}

void DJAudioPlayer::releaseResources()
{
    // Clean up when playback stops
    resampleSource.releaseResources();
}

void DJAudioPlayer::loadURL(URL audioURL)
{
    // Try to create a reader for the given audio file
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr)
    {
        // Set up a new source for the audio file
        unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));

        // Pass it to the transport source, which handles playback
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);

        // Store the source and take ownership of it
        readerSource.reset(newSource.release());

        // Reset EQ to neutral when loading a new track
        resetEQ();
    }
}

void DJAudioPlayer::setGain(double gain)
{
    // Validate input range
    if (gain < 0 || gain > 1)
    {
        DBG("DJAudioPlayer::setGain gain should be between 0 and 1");
    }
    else {
        transportSource.setGain(gain);
    }
}

void DJAudioPlayer::setSpeed(double ratio)
{
    // Validate input range
    if (ratio < 0.0 || ratio > 10.0)
    {
        DBG("DJAudioPlayer::setSpeed ratio value should be between 0 and 100");
    }
    else {
        // Apply speed change using the resampler
        resampleSource.setResamplingRatio(ratio);
    }
}

void DJAudioPlayer::setPosition(double posInSecs)
{
    // Set playback position in seconds
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos)
{
    // Validate input range
    if (pos < 0.0 || pos > 1.0)
    {
        DBG("DJAudioPlayer::setPositionRelative relative position value should be between 0 and 1");
    }
    else {
        // Convert relative position (0-1) to actual time in seconds
        double posInSeconds = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSeconds);
    }
}

void DJAudioPlayer::start()
{
    transportSource.start();
}

void DJAudioPlayer::stop()
{
    transportSource.stop();
}

double DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

bool DJAudioPlayer::playing()
{
    return transportSource.isPlaying();
}

// Limit bands to reasonable range and store them
void DJAudioPlayer::setHighGain(double gain)
{
    highGain = jlimit(0.0, 2.0, gain);
}

void DJAudioPlayer::setMidGain(double gain)
{
    midGain = jlimit(0.0, 2.0, gain);
}

void DJAudioPlayer::setLowGain(double gain)
{
    lowGain = jlimit(0.0, 2.0, gain);
}

void DJAudioPlayer::resetEQ()
{
    // Reset all EQ bands to neutral position (no boost/cut)
    lowGain = 1.0;
    midGain = 1.0;
    highGain = 1.0;
}