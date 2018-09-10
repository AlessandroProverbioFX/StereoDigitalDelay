#include "PluginProcessor.h"
#include "PluginEditor.h"

int knobSize = 120;
int textHeight = 30;
int borderPad = 10;
int winWidth = 400;
int winHeight = 170;

StereoDigitalDelayAudioProcessorEditor::StereoDigitalDelayAudioProcessorEditor(StereoDigitalDelayAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(winWidth, winHeight);
    setResizable(false, false);
    
    levelSliderAttach = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, LEVEL_ID, levelKnob);
    bpmSliderAttach = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, BPM_ID, timeKnob);
    feedbackSliderAttach = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, FEEDBACK_ID, feedbackKnob);
    
    levelKnob.setSize(knobSize, knobSize);
    levelKnob.setTopLeftPosition(borderPad, borderPad);
    levelKnob.setSliderStyle(Slider::Rotary);
    levelKnob.setRange(0, 1, 0.001);
    levelKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    levelKnob.addListener(this);
    addAndMakeVisible(levelKnob);
    
    timeKnob.setSize(knobSize, knobSize);
    timeKnob.setTopLeftPosition(borderPad*2+knobSize, borderPad);
    timeKnob.setSliderStyle(Slider::Rotary);
    timeKnob.setRange(30, 200, 1);
    timeKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    timeKnob.setPopupDisplayEnabled(true, true, this);
    timeKnob.addListener(this);
    addAndMakeVisible(timeKnob);
    
    feedbackKnob.setSize(knobSize, knobSize);
    feedbackKnob.setTopLeftPosition(borderPad*3+knobSize*2, borderPad);
    feedbackKnob.setSliderStyle(Slider::Rotary);
    feedbackKnob.setRange(0, 0.8, 0.001);
    feedbackKnob.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    feedbackKnob.addListener(this);
    addAndMakeVisible(feedbackKnob);    
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
    g.setFont(20.0F);
    
    g.drawText("LEVEL", borderPad, borderPad+knobSize, knobSize, textHeight, Justification::centred);
    g.drawText("BPM", borderPad*2+knobSize, borderPad+knobSize, knobSize, textHeight, Justification::centred);
    g.drawText("FEEDBACK", borderPad*3+knobSize*2, borderPad+knobSize, knobSize, textHeight, Justification::centred);
    
    g.setFont(8.0F);
    g.drawText("V. 1.0.4", 365, 155, 35, 15, Justification::centred);
}

void StereoDigitalDelayAudioProcessorEditor::resized()
{
}

void StereoDigitalDelayAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
    processor.level = levelKnob.getValue();
    processor.bpm = timeKnob.getValue();
    processor.feedback = feedbackKnob.getValue();
}






