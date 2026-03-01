/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ModulationDistortionPulserAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ModulationDistortionPulserAudioProcessorEditor (ModulationDistortionPulserAudioProcessor&);
    ~ModulationDistortionPulserAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ModulationDistortionPulserAudioProcessor& audioProcessor;
    
    juce::Slider modFreqSlider;
    juce::ComboBox modTypeBox;
    juce::Slider driveDbSlider;
    juce::ComboBox distTypeBox;
    juce::Slider pulseFreqSlider;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    
    std::unique_ptr<SliderAttachment> modFreqAttachment;
    std::unique_ptr<ComboBoxAttachment> modTypeAttachment;
    std::unique_ptr<SliderAttachment> driveDbAttachment;
    std::unique_ptr<ComboBoxAttachment> distTypeAttachment;
    std::unique_ptr<SliderAttachment> pulseFreqAttachment;
    
    juce::Label modFreqLabel;
    juce::Label driveDbLabel;
    juce::Label pulseFreqLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulationDistortionPulserAudioProcessorEditor)
};
