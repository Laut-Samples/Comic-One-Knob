/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Comic_OneKnobAudioProcessorEditor::Comic_OneKnobAudioProcessorEditor (Comic_OneKnobAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

highCutFreqSliderAttachment(audioProcessor.apvts, "HighCut Freq", highCutFreqSlider)

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 400);
    
       //IMAGE
    
    auto tapImage = juce::ImageCache::getFromMemory(BinaryData::ComicOneKnob_png, BinaryData::ComicOneKnob_pngSize);
    
    
    if(! tapImage.isNull())
        mImageComponent.setImage (tapImage, juce::RectanglePlacement::stretchToFit);
    else
        jassert (! tapImage.isNull());

    
    addAndMakeVisible(mImageComponent);
    
//    addAndMakeVisible(button);
    button.addListener(this);
    
    
    // Dist
    addAndMakeVisible(&disChoice);
    disChoice.addItem("Default", 1);
    disChoice.addItem("Dist Algo", 2);
    disChoice.addItem("Hard Clip", 3);
    disChoice.addItem("Soft Clip", 4);
    disChoice.addItem("Half Wave Rect", 5);
    disChoice.addItem("Dist KI", 6);
    disChoice.addItem("Overdrive", 7);
//    disChoice.addItem("Phaser", 8);
    disChoice.setSelectedId(1);
    disChoice.addListener(this);

    
    // DEZIBEL
//    addAndMakeVisible(&dbChoice);
    dbChoice.addItem("Default", 1);
    dbChoice.addItem("3dB", 2);
    dbChoice.addItem("6dB", 3);
    dbChoice.addItem("Reverb", 4);
    dbChoice.setSelectedId(2);
    dbChoice.addListener(this);
    
    
    // MIX
    
     addAndMakeVisible(&Mix);
     Mix.setRange(0.0f, 1.0f, 0.001);
     Mix.addListener(this);
    
    
    // Thesr
    addAndMakeVisible(&Threshold);
    Threshold.setRange(0.0f, 1.0f, 0.001);
    Threshold.addListener(this);


    addAndMakeVisible(gainSlider);
    gainSlider.addListener(this);
    gainSlider.setRange(0.0, 5.0, 0.001);
    gainSlider.setValue(1.0);
    
    
    addAndMakeVisible(&compmix);
    compmix.setRange(-50, 0, 1);
    compmix.addListener(this);
    

    
    
}

Comic_OneKnobAudioProcessorEditor::~Comic_OneKnobAudioProcessorEditor()
{
}

//==============================================================================
void Comic_OneKnobAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    
    

    
    
    
    // SLIDER
    
    addAndMakeVisible(highCutFreqSlider);
    
    
    for( auto* comp : getComps() )
    {
        addAndMakeVisible (comp);

    }

    
    
}

void Comic_OneKnobAudioProcessorEditor::resized()
{
    
    
    //Image
    mImageComponent.setSize(450,450);

        // Filter
    highCutFreqSlider.setBoundsRelative(0.25f, 0.25f, 0.1f, 0.1f);
    highCutFreqSlider.setCentrePosition(290, 210);
//    highCutFreqSlider.setAlpha(0.5f);
    highCutFreqSlider.setColour(juce::Slider::thumbColourId, juce::Colours::black);
    
    
    //dist
    disChoice.setCentrePosition(20, 20);
    disChoice.setSize(150, 40);
    disChoice.setAlpha(0.7f);
    disChoice.setColour(juce::Slider::thumbColourId, juce::Colours::black);
    
    
    // Thresh
    Threshold.setCentrePosition(95, 320);
    Threshold.setSize(150, 50);
//    Threshold.setAlpha(0.5f);
    Threshold.setColour(juce::Slider::thumbColourId, juce::Colours::black);
    
    // MIX
    Mix.setCentrePosition(175, 305);
    Mix.setSize(150, 50);
//    Mix.setAlpha(0.5f);
    Mix.setColour(juce::Slider::thumbColourId, juce::Colours::black);
    
    // Mix Comp
    compmix.setCentrePosition(105, 180);
    compmix.setSize(150, 90);
//    compmix.setAlpha(0.5f);
    compmix.setColour(juce::Slider::thumbColourId, juce::Colours::black);
    
//    //Dezibel
    dbChoice.setCentrePosition(300, 300);
    dbChoice.setSize(40, 40);
    dbChoice.setAlpha(0.7f);
    dbChoice.setColour(juce::Slider::thumbColourId, juce::Colours::black);
    
    button.setCentrePosition(307, 340);
    button.setSize(20, 20);
    button.setAlpha(0.7f);
    
    gainSlider.setCentrePosition(50, 175);
    gainSlider.setSize(100, 150);
    gainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    
//    button.setColour(tickDisabledColourId, juce::Colours::red);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}



    std::vector<juce::Component*>Comic_OneKnobAudioProcessorEditor::getComps()
{
    return
    {
        &highCutFreqSlider,
    };
    
}
    
// Dist
void Comic_OneKnobAudioProcessorEditor::comboBoxChanged(juce::ComboBox * comboBoxThatHasChanged)
{
    audioProcessor.menuChoice = comboBoxThatHasChanged->getSelectedId();
    audioProcessor.dbChoice = comboBoxThatHasChanged->getSelectedId();
    
}

void Comic_OneKnobAudioProcessorEditor::sliderValueChanged(juce::Slider * sliderThatHasChanged)
{
    if (&Mix == sliderThatHasChanged)
    {
        audioProcessor.mix = sliderThatHasChanged->getValue();
    }
    
    if (&Threshold == sliderThatHasChanged)
    {
        audioProcessor.thresh = sliderThatHasChanged->getValue();
    }
    if (&compmix == sliderThatHasChanged)
    {
        audioProcessor.compmix = sliderThatHasChanged->getValue();
    }
    
    if (&gainSlider == sliderThatHasChanged)
    {
        audioProcessor.gain = sliderThatHasChanged->getValue();
    }

}



void Comic_OneKnobAudioProcessorEditor::buttonClicked(juce::Button * buttonclicked)
    {
        
        if (&this->button == buttonclicked)
        {
            audioProcessor.button = buttonclicked->getState();
        }
        
    }
