#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


class StereoDigitalDelayAudioProcessorEditor : public AudioProcessorEditor,  private Slider::Listener,
                                                                                     TextButton::Listener
{
    
public:
    
    StereoDigitalDelayAudioProcessorEditor(StereoDigitalDelayAudioProcessor&);
    ~StereoDigitalDelayAudioProcessorEditor();

    void paint(Graphics&) override;
    void resized() override;
    
    ScopedPointer <AudioProcessorValueTreeState::SliderAttachment> levelSliderAttach;
    ScopedPointer <AudioProcessorValueTreeState::SliderAttachment> bpmSliderAttach;
    ScopedPointer <AudioProcessorValueTreeState::SliderAttachment> feedbackSliderAttach;

private:
    
    StereoDigitalDelayAudioProcessor& processor;
    
    Slider levelKnob;
    Slider timeKnob;
    Slider feedbackKnob;
    Slider toneKnob;
    
    TextButton syncButton;
    
    void sliderValueChanged(Slider* slider) override;
    void buttonClicked(Button* button) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoDigitalDelayAudioProcessorEditor)
    
};
