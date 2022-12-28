/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


//==============================================================================

struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
        
    }
};

struct LinearVerticalSlider : juce::Slider
{
    LinearVerticalSlider() : juce::Slider(juce::Slider::SliderStyle::LinearVertical,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
        
    }
};

//==============================================================================
/**
*/
class Comic_OneKnobAudioProcessorEditor  : public juce::AudioProcessorEditor,

private juce::ComboBox::Listener,
juce::Slider::Listener,
juce::Button::Listener


{
public:
    Comic_OneKnobAudioProcessorEditor (Comic_OneKnobAudioProcessor&);
    ~Comic_OneKnobAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    

   
private:
    

    
    
    Comic_OneKnobAudioProcessor& audioProcessor;
    
    // Image
    juce::ImageComponent mImageComponent;
    
    //Slider
    CustomRotarySlider highCutFreqSlider, Mix, Threshold, compmix;
    
    LinearVerticalSlider gainSlider;
    
    juce::TextButton button;
    
    ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);
        
    std::vector<juce::Component*>getComps();
        
    using APVTS = juce::AudioProcessorValueTreeState;
       
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
        
    Attachment highCutFreqSliderAttachment;
    
    
    // Dist
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override; 
    juce::ComboBox disChoice;
    juce::ComboBox dbChoice;
    
    void sliderValueChanged(juce::Slider* sliderThatHasChanged) override;
    
    void buttonClicked(juce::Button* buttonclicked) override;

    
//    // MIX Slider
//    juce::Slider Mix;
//    juce::Slider Threshold;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Comic_OneKnobAudioProcessorEditor)
};
