#include "PluginProcessor.h"
#include "PluginEditor.h"


StereoDigitalDelayAudioProcessor::StereoDigitalDelayAudioProcessor()
                                : AudioProcessor (BusesProperties()
                                                  .withInput ("Stereo", AudioChannelSet::stereo(), true)
                                                  .withOutput ("Stereo", AudioChannelSet::stereo(), true)
                                                ),
                                                level(0.5),
                                                bpm(80),
                                                feedback(0.3),
                                                parameters(*this, nullptr)
 {
     NormalisableRange<float> levelRange (0.0f, 1.0f);
     NormalisableRange<float> bpmRange (30.0f, 200.0f);
     NormalisableRange<float> feedbackRange (0.0f, 0.8f);
     
     parameters.createAndAddParameter(LEVEL_ID, LEVEL_NAME, LEVEL_NAME, levelRange, 0.5, nullptr, nullptr);
     parameters.createAndAddParameter(BPM_ID, BPM_NAME, BPM_NAME, bpmRange, 80, nullptr, nullptr);
     parameters.createAndAddParameter(FEEDBACK_ID, FEEDBACK_NAME, FEEDBACK_NAME, feedbackRange, 0.3, nullptr, nullptr);
     
     parameters.state = ValueTree("savedParams");
 }


StereoDigitalDelayAudioProcessor::~StereoDigitalDelayAudioProcessor() { }

// MIDI Setup Methods
bool StereoDigitalDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StereoDigitalDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StereoDigitalDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StereoDigitalDelayAudioProcessor::getTailLengthSeconds() const { return 0.0; }

// Getter & Setter
const String StereoDigitalDelayAudioProcessor::getName() const { return JucePlugin_Name; }
int StereoDigitalDelayAudioProcessor::getNumPrograms() { return 1; }
int StereoDigitalDelayAudioProcessor::getCurrentProgram() { return 0; }
void StereoDigitalDelayAudioProcessor::setCurrentProgram (int index) { }
const String StereoDigitalDelayAudioProcessor::getProgramName (int index) { return {}; }
void StereoDigitalDelayAudioProcessor::changeProgramName (int index, const String& newName) { }

// Get and Set Plugin State
void StereoDigitalDelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ScopedPointer<XmlElement> xml (parameters.state.createXml());    
    copyXmlToBinary(*xml, destData);
}

void StereoDigitalDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> params (getXmlFromBinary(data, sizeInBytes));
    
    if (params != nullptr)
    {
        if (params -> hasTagName(parameters.state.getType()))
        {
            parameters.state = ValueTree::fromXml(*params);
        }
    }
}

//================================= DEFAULT METHODS =============================================

void StereoDigitalDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const int numOutputChannel = getTotalNumOutputChannels();
    const int delayBufferSize = maxDelay*(sampleRate + samplesPerBlock);
    
    delayBuffer.setSize(numOutputChannel, delayBufferSize);
    wetBuffer.setSize(numOutputChannel, samplesPerBlock);
    
    localSampleRate = sampleRate;
}

void StereoDigitalDelayAudioProcessor::releaseResources() { }


bool StereoDigitalDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // we only support stereo and mono
    if (layouts.getMainInputChannels() == 0 || layouts.getMainInputChannels() > 2)
        return false;
    
    if (layouts.getMainOutputChannels() != 2)
        return false;
    
    // we don't allow the narrowing the number of channels
    if (layouts.getMainInputChannels() > layouts.getMainOutputChannels())
        return false;
    
    return true;
}


AudioProcessorEditor* StereoDigitalDelayAudioProcessor::createEditor() { return new StereoDigitalDelayAudioProcessorEditor (*this); }
bool StereoDigitalDelayAudioProcessor::hasEditor() const { return true; }
AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new StereoDigitalDelayAudioProcessor(); }


//================================= SIGNAL PROCESSING METHODS =============================================

void StereoDigitalDelayAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    const int bufferLength = buffer.getNumSamples();
    const int delayBufferLength = delayBuffer.getNumSamples();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, bufferLength);
    
    if (totalNumInputChannels == 1)
    {
        buffer.copyFrom(1, 0, buffer.getReadPointer(0), bufferLength);
    }
    
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        const float* bufferData = buffer.getReadPointer(channel);
        const float* delayBufferData = delayBuffer.getReadPointer(channel);
        const float* wetBufferData = wetBuffer.getReadPointer(channel);
        
        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, wetBufferData, delayBufferData);
        feedBackDelay(channel, bufferLength, delayBufferLength, wetBufferData);
    }
    
    writePosition += bufferLength;
    writePosition %= delayBufferLength;
}


void StereoDigitalDelayAudioProcessor::fillDelayBuffer (int channel, const int bufferLength, const int delayBufferLength,
                                                        const float* bufferData, const float* delayBufferData)
{
    
    if(delayBufferLength > writePosition + bufferLength)
    {
        delayBuffer.copyFromWithRamp(channel, writePosition, bufferData, bufferLength, level, level);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - writePosition;
        delayBuffer.copyFromWithRamp(channel, writePosition, bufferData, bufferRemaining, level, level);
        delayBuffer.copyFromWithRamp(channel, 0, bufferData + bufferRemaining, bufferLength-bufferRemaining, level, level);
    }
}

void StereoDigitalDelayAudioProcessor::getFromDelayBuffer (AudioBuffer<float>& buffer, int channel, const int bufferLength,
                                                           const int delayBufferLength, const float* wetBufferData,
                                                           const float* delayBufferData)
{
    float delayTime = (60.0/bpm)*(1-channel) + (60.0/(2*bpm))*(channel); 
    const int readPosition = static_cast<int>(delayBufferLength + writePosition - (delayTime*localSampleRate)) % delayBufferLength;
    
    if ( delayBufferLength > readPosition + bufferLength )
    {
        wetBuffer.copyFrom(channel, 0, delayBufferData + readPosition, bufferLength);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - readPosition;
        wetBuffer.copyFrom(channel, 0, delayBufferData + readPosition, bufferRemaining);
        wetBuffer.copyFrom(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining);
    }
        
    buffer.addFrom(channel, 0, wetBufferData, bufferLength);
}

void StereoDigitalDelayAudioProcessor::feedBackDelay(int channel, const int bufferLength, const int delayBufferLength, const float* wetBufferData)
{

    if (delayBufferLength > writePosition + bufferLength)
    {
        delayBuffer.addFromWithRamp(channel, writePosition, wetBufferData, bufferLength, feedback, feedback);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - writePosition;
        delayBuffer.addFromWithRamp(channel, writePosition, wetBufferData, bufferRemaining, feedback, feedback);
        delayBuffer.addFromWithRamp(channel, 0, wetBufferData + bufferRemaining, bufferLength-bufferRemaining, feedback, feedback);
    }
}












