/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ModulationDistortionPulserAudioProcessorEditor::ModulationDistortionPulserAudioProcessorEditor (ModulationDistortionPulserAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Slider setup
      addAndMakeVisible (modFreqSlider);
      modFreqSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
      modFreqSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
      modFreqSlider.setNumDecimalPlacesToDisplay (1);
    
      addAndMakeVisible (driveDbSlider);
      driveDbSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
      driveDbSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
      driveDbSlider.setNumDecimalPlacesToDisplay (1);

      addAndMakeVisible (pulseFreqSlider);
      pulseFreqSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
      pulseFreqSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
      pulseFreqSlider.setNumDecimalPlacesToDisplay (1);
    
    // Labels
     addAndMakeVisible (modFreqLabel);
     modFreqLabel.setText ("Mod Freq", juce::dontSendNotification);
     modFreqLabel.setJustificationType (juce::Justification::centred);

     addAndMakeVisible (driveDbLabel);
     driveDbLabel.setText ("Drive", juce::dontSendNotification);
     driveDbLabel.setJustificationType (juce::Justification::centred);

     addAndMakeVisible (pulseFreqLabel);
     pulseFreqLabel.setText ("Pulse Freq", juce::dontSendNotification);
     pulseFreqLabel.setJustificationType (juce::Justification::centred);
    
    // ComboBoxes
      addAndMakeVisible (modTypeBox);
      modTypeBox.addItem ("AM", 1);
      modTypeBox.addItem ("RM", 2);
      modTypeBox.setSelectedId (1);

      addAndMakeVisible (distTypeBox);
      distTypeBox.addItem ("Soft", 1);
      distTypeBox.addItem ("Hard", 2);
      distTypeBox.setSelectedId (1);

      // Attach slider to APVTS parameter 
      modFreqAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "modFreq", modFreqSlider);
      modTypeAttachment  = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts, "modType",   modTypeBox);
      driveDbAttachment  = std::make_unique<SliderAttachment>  (audioProcessor.apvts, "driveDb",   driveDbSlider);
      distTypeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts, "distType",  distTypeBox);
      pulseFreqAttachment= std::make_unique<SliderAttachment>  (audioProcessor.apvts, "pulseFreq", pulseFreqSlider);


      setResizable (true, true);
      setResizeLimits (300, 300, 800, 800);
      setSize (400, 400);
}

ModulationDistortionPulserAudioProcessorEditor::~ModulationDistortionPulserAudioProcessorEditor()
{
}

//==============================================================================
void ModulationDistortionPulserAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::darkred);

    g.setColour (juce::Colours::whitesmoke);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Motion Distortion Pulser AM/RM", getLocalBounds().removeFromTop (30), juce::Justification::centred, 1);
}

void ModulationDistortionPulserAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);

    // Push combos down from title text (adjust if needed)
    area.removeFromTop (20);

    // Top row: combo boxes

    auto topRow = area.removeFromTop (30);
    modTypeBox.setBounds  (topRow.removeFromLeft (topRow.getWidth() / 2).reduced (4));
    distTypeBox.setBounds (topRow.reduced (4));

    // Space between combos and knobs
    area.removeFromTop (20);


    // Sizes
  
    const int knobSizeMain   = 140;
    const int knobSizeBottom = 120;
    const int labelH         = 18;
    // space between label and knob
    const int labelGap       = 2;   

    // Middle row: two knobs + labels
 
    auto midRow = area.removeFromTop (knobSizeMain + labelH + labelGap);

    auto leftCell  = midRow.removeFromLeft (midRow.getWidth() / 2);
    auto rightCell = midRow;

    auto leftKnobBounds  = leftCell.withSizeKeepingCentre  (knobSizeMain, knobSizeMain);
    auto rightKnobBounds = rightCell.withSizeKeepingCentre (knobSizeMain, knobSizeMain);

    // Labels above knobs
    modFreqLabel.setBounds (
        leftKnobBounds.withY (leftKnobBounds.getY() - (labelH + labelGap))
                      .withHeight (labelH));

    driveDbLabel.setBounds (
        rightKnobBounds.withY (rightKnobBounds.getY() - (labelH + labelGap))
                       .withHeight (labelH));

    // Knobs
    modFreqSlider.setBounds  (leftKnobBounds);
    driveDbSlider.setBounds  (rightKnobBounds);

    // Space between rows
    area.removeFromTop (20);

    // Bottom knob + label
   
    auto bottomBlock = area.removeFromTop (knobSizeBottom + labelH + labelGap);

    auto pulseKnobBounds = bottomBlock.withSizeKeepingCentre (knobSizeBottom, knobSizeBottom);

    pulseFreqLabel.setBounds (
        pulseKnobBounds.withY (pulseKnobBounds.getY() - (labelH + labelGap))
                       .withHeight (labelH));

    pulseFreqSlider.setBounds (pulseKnobBounds);
    
}
    

