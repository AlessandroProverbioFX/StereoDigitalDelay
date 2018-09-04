#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


class StereoDigitalDelayAudioProcessor : public AudioProcessor
{
    
public:

    StereoDigitalDelayAudioProcessor();
    ~StereoDigitalDelayAudioProcessor();
    
    // Effect Parameters, controlled by the user with the UI
    float level { 0.5 };  
    int bpm { 80 };
    float feedback { 0.3 };
    
    // Start & Stop Methods
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
   #endif

    // Main Signal Processing Method  ------------------------------------------------------------  //
    
    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;
    void fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength,
                         const float* bufferData, const float* delayBufferData);
    void getFromDelayBuffer (AudioBuffer<float>& buffer, int channel, const int bufferLength,
                             const int delayBufferLength, const float* wetBufferData,
                             const float* delayBufferData);
    void feedBackDelay (int channel, const int bufferLength,
                        const int delayBufferLength, const float* wetBuffer);
    
    // -------------------------------------------------------------------------------------------  //

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // MIDI Setup Methods
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    // Getter & Setter
    const String getName() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;
    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

private:
    
    AudioBuffer<float> delayBuffer;
    AudioBuffer<float> wetBuffer;
    int writePosition { 0 };
    int localSampleRate { 44100 };
    
    int maxDelay { 2 } ;  // 2 sec --> 30 bpm is min bpm, while max bpm is 200
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoDigitalDelayAudioProcessor)
};
