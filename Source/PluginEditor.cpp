#include "PluginProcessor.h"
#include "PluginEditor.h"

int unit = 20;
int knobSize = 6*unit;
int winWidth = 20.5*unit;
int winHeight = 10.5*unit;

StereoDigitalDelayAudioProcessorEditor::StereoDigitalDelayAudioProcessorEditor(StereoDigitalDelayAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(winWidth, winHeight);
    setResizable(false, false);
    
    levelSliderAttach = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, LEVEL_ID, levelKnob);
    bpmSliderAttach = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, BPM_ID, timeKnob);
    feedbackSliderAttach = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, FEEDBACK_ID, feedbackKnob);
    
    levelKnob.setSize(knobSize, knobSize);
    levelKnob.setTopLeftPosition(unit, 2*unit);
    levelKnob.setSliderStyle(Slider::Rotary);
    levelKnob.setRange(0, 1, 0.001);
    levelKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    levelKnob.addListener(this);
    addAndMakeVisible(levelKnob);
    
    timeKnob.setSize(knobSize + unit, knobSize + unit);
    timeKnob.setTopLeftPosition(13*unit, unit);
    timeKnob.setSliderStyle(Slider::Rotary);
    timeKnob.setRange(30, 200, 1);
    timeKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxAbove, false, 3*unit, unit);
    timeKnob.addListener(this);
    addAndMakeVisible(timeKnob);
    
    feedbackKnob.setSize(knobSize, knobSize);
    feedbackKnob.setTopLeftPosition(7*unit, 2*unit);
    feedbackKnob.setSliderStyle(Slider::Rotary);
    feedbackKnob.setRange(0, 0.8, 0.001);
    feedbackKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    feedbackKnob.addListener(this);
    addAndMakeVisible(feedbackKnob);
    
    syncButton.setSize(unit, unit);
    syncButton.setTopLeftPosition(10.5*unit, 0.75*unit);
    syncButton.setColour(TextButton::buttonColourId, getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    syncButton.addListener(this);
    addAndMakeVisible(syncButton);
}

StereoDigitalDelayAudioProcessorEditor::~StereoDigitalDelayAudioProcessorEditor()
{
    levelSliderAttach.~ScopedPointer();
    bpmSliderAttach.~ScopedPointer();
    feedbackSliderAttach.~ScopedPointer();
}

void StereoDigitalDelayAudioProcessorEditor::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    
    g.setColour(getLookAndFeel().findColour(Slider::thumbColourId));
    g.setFont(unit);
    
    g.drawText("LEVEL", unit, 8*unit, knobSize, 2*unit, Justification::centred);
    g.drawText("FEEDBACK", 7*unit, 8*unit, knobSize, 2*unit, Justification::centred);
    g.drawText("BPM", 13.5*unit, 8*unit, knobSize, 2*unit, Justification::centred);
    g.drawText("TEMPO SYNC", unit, 0.25*unit, 2*knobSize, 2*unit, Justification::centred);
    
    
    g.setFont(8.0F);
    g.drawText("V. 1.1.0", 375, 195, 35, 15, Justification::centred);
}

void StereoDigitalDelayAudioProcessorEditor::resized()
{
}

void StereoDigitalDelayAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
    processor.level = processor.logPot(levelKnob.getValue());
    processor.bpm = timeKnob.getValue();
    processor.feedback = feedbackKnob.getValue();
}

void StereoDigitalDelayAudioProcessorEditor::buttonClicked(Button *button)
{
    timeKnob.setValue(processor.bpmHost);
}





