/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Distortion.h"
#include "Clipping.h"
#include "DistKI.h"
#include "Overdrive.h"
#include "BrickWall.h"
#include "PhaserKI.h"
#include "CompressorKI.h"
#include "Distortionviator.h"


enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48
    
};

// Chain Settings

struct ChainSettings
{
    float peakFreq { 0 }, peakGainInDecibels{0}, peakQuality {1.f};
    float lowCutFreq { 0 }, highCutFreq { 0 };
    
    Slope lowCutSlope {Slope::Slope_12}, highCutSlope {Slope::Slope_12};
    
    bool reverbBypassed { false};
};


ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);


//==============================================================================
/**
*/
class Comic_OneKnobAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Comic_OneKnobAudioProcessor();
    ~Comic_OneKnobAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::AudioProcessorValueTreeState::ComboBoxAttachment createParameter();
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};
    
    
    
    // Filter Process Initialise
    using Filter = juce::dsp::IIR::Filter<float>;           // use Filter
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;    // Use chain with IIR Filter add more if you want
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>; // use Monochain with cutfilter
    MonoChain leftChain, rightChain;                        // Initialiser left right
    
    float gain = 1.0f;
    
    int menuChoice, dbChoice;
    float distortionValue = 0.5f;
    float levelValue = 0.0f;
    bool  effectOn = false;
    
    float thresh = 0.0f; // Limit
    float mix = 0.0f;
    
    float button = 0;
    
    float compmix;
    
//    float gain;
    
    int numPhaserStages;
    
    void updatePhaseDelays(int numPhaserStages, double sampleRate);
    
private:

    juce::Reverb reverb;
    
    DistKI distki;
    Distortion distortion;
    Clipping clipping;
    PhaserKI phaserki;
//    viator_dsp::Distortionviator<SampleType>::distviat();
    

    juce::dsp::Compressor<float> compressor;
    
    Overdrive overdrive;
    
    BrickWallLimiter brickwall;
    
 
    
    enum ChainPositions
    {
        LowCut,
        Peak,
        HighCut
    };
    
    
    void updatePeakFilter(const ChainSettings& chainSettings);
    
    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients(Coefficients& old, const Coefficients& replacements);
    
    template<int Index, typename ChainType, typename CoefficientsType>
    
    void update(ChainType& chain, const CoefficientsType& coefficients)
    {
        updateCoefficients (chain.template get<Index>().coefficients, coefficients[Index]);
        chain.template setBypassed<Index>(false);
    }
    
    template<typename ChainType, typename CoefficientsType>
    
    
    void updateCutFilter(ChainType& leftLowCut,
                         const CoefficientsType& cutCoefficients,
                         const Slope& lowCutSlope)
    {
            leftLowCut.template setBypassed<0>(true);
            leftLowCut.template setBypassed<1>(true);
            leftLowCut.template setBypassed<2>(true);
            leftLowCut.template setBypassed<3>(true);
        
            switch( lowCutSlope )
            {
                case Slope_48:
                {
                    update<3>(leftLowCut, cutCoefficients);
                }
                case Slope_36:
                {
                    update<2>(leftLowCut, cutCoefficients);
                }
                case Slope_24:
                {
                    update<1>(leftLowCut, cutCoefficients);
                }
                case Slope_12:
                {
                    update<0>(leftLowCut, cutCoefficients);
                }
            }
    }
    
    void updateLowCutFilters(const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);
    
    void updateFilters();
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Comic_OneKnobAudioProcessor)
};
