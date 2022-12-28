/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
Comic_OneKnobAudioProcessor::Comic_OneKnobAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),

distortion(distortionValue), clipping(levelValue)

#endif
{
}

Comic_OneKnobAudioProcessor::~Comic_OneKnobAudioProcessor()
{
}


//==============================================================================
const juce::String Comic_OneKnobAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Comic_OneKnobAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Comic_OneKnobAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Comic_OneKnobAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Comic_OneKnobAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Comic_OneKnobAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Comic_OneKnobAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Comic_OneKnobAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Comic_OneKnobAudioProcessor::getProgramName (int index)
{
    return {};
}

void Comic_OneKnobAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}



//==============================================================================
void Comic_OneKnobAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
        // Use this method as the place to do any pre-playback
    // initialisation that you need..
    distortion.prepare(sampleRate);
    clipping.prepare(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = 2;
    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    
    compressor.prepare(spec);
    
    updateFilters();
    
   
}

void Comic_OneKnobAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Comic_OneKnobAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Comic_OneKnobAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    
    


    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    distortion.setKnob(distortionValue);
    clipping.setKnob(levelValue);

    // Filter Update
    updateFilters();
    
            for (int channel = 0; channel < totalNumInputChannels; ++channel)
            {
                auto* channelData = buffer.getWritePointer(channel);
                
                for (int n = 0; n < buffer.getNumSamples(); ++n)
                {
                    int x = buffer.getReadPointer(channel)[n];
                    
                    auto input = channelData[n];
                    auto cleanOut = channelData[n];
                    
                    
                    if (dbChoice == 1)
                    {
                        input *= 0.7;
                    }
                    if (dbChoice == 2) // 3db
                    {
                        input *= 1.0;
                    }

                    if (dbChoice == 2) // 6db
                    {
                        input *= 1 ;
                    }
                    if (dbChoice == 3)
                    {
                        input = mix ;
                    }
                    
                    
                    if (menuChoice == 1)    // Normal
                    {
                        buffer.getWritePointer(channel)[n] = x;
                    }
                        
                    if (menuChoice == 2)      // H File Dist Algo
                    {
                        
                    float ds = distortion.processSample(x);
                        //float o = tone.processSample(dis);
                        buffer.getWritePointer(channel)[n] = clipping.processSample(ds);
                        
                        if (input > thresh)
                        {
                            input = 1.0f - expf(-input);
                        }
                        else
                        {
                            input = -1.0f + expf(input);
                        }
                    }
                    
                    
                    if (menuChoice == 3)    // Hard Clip
                    {
                        if (input > thresh)
                        {
                            input = thresh;// Limiter +1
                        }
                        else if (input < -thresh)
                        {
                            input = -thresh;    // Limit -1
                        }
                        else
                        {
                            input = input;
                        }
          
                    }
                     
                    
                    if (menuChoice == 4)    // Soft Clip
                        //Soft Clipping Exp
                    {
                        if (input > thresh)
                        {
                            input = 1.0f - expf(-input);
                        }
                        else
                        {
                            input = -1.0f + expf(input);
                        }
                    }
                        
                    if (menuChoice == 5)
                        //Half-Wave Rectifier
                    {
                        if (input > thresh)
                        {
                            input = input;
                        }
                        else
                        {
                            input = 0;
                        }
                    }
                
                    
                    if (menuChoice == 6)
                        
                    {
                        distki.setGain(mix);
                        distki.setThreshold(thresh);
                        distki.process(buffer.getWritePointer(1), buffer.getNumSamples());
                        distki.process(buffer.getWritePointer(0), buffer.getNumSamples());
                    }
                    
                    if (menuChoice == 7)
                    {
                        overdrive.setGain(mix);
                        overdrive.setThreshold(thresh);
                        overdrive.process(buffer.getWritePointer(1), buffer.getNumSamples());
                        overdrive.process(buffer.getWritePointer(0), buffer.getNumSamples());
                        
                    }
                    
//                    float sample = buffer.getSample(totalNumOutputChannels, n);
                    
                    

                    // MIX
                    
                    channelData[n] = ((1 - mix) * cleanOut) + (mix * input);
                    
                    
                }
            }
    


    

    
    // clean those high frequences
    juce::dsp::AudioBlock<float> block(buffer);

//    phaserki.process(block);

    
    juce::dsp::ProcessContextReplacing<float> context(block);



    
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    
    
//    if(menuChoice == 8)
//        
//    {
//    phaserki.setWetDryMix(mix);
////    phaserki.setFeedback(0);
////    phaserki.setDepth(0);
////    phaserki.setRate(0);
//    phaserki.setLfoWaveform(3);
//    phaserki.setLfoFrequency(thresh);
//    phaserki.setFilterType(2);
////    phaserki.process(context);
//    }
    
    
    
    compressor.setRatio(20);
    compressor.setThreshold(compmix);
    compressor.setAttack(0.1);
    compressor.setRelease(0.1);
    

    compressor.process(context);
    
    
//    juce::dsp::ProcessContextReplacing<float> context(block);
    
    
    leftChain.process(leftContext);
    rightChain.process(rightContext);
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            channelData[sample] *= gain;
    }
    
    brickwall.process(buffer.getWritePointer(1), buffer.getNumSamples());
    brickwall.process(buffer.getWritePointer(0), buffer.getNumSamples());
    
}
    
    
    
    
    
    


//==============================================================================
bool Comic_OneKnobAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Comic_OneKnobAudioProcessor::createEditor()
{
    return new Comic_OneKnobAudioProcessorEditor (*this);
//        return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void Comic_OneKnobAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void Comic_OneKnobAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
     auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if( tree.isValid() )
    {
        apvts.replaceState(tree);
        updateFilters();
    }
}



juce::AudioProcessorValueTreeState::ParameterLayout Comic_OneKnobAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    //layout.add(std::make_unique<juce::AudioParameterChoice>("Distortion", "Distortion", distArray, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCut Freq",
                                                           "LowCut Freq",
                                                           juce::NormalisableRange<float>(20.f,20000.f, 1.f, 0.25f),
                                                           20.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCut Freq",
                                                           "HighCut Freq",
                                                           juce::NormalisableRange<float>(20.f,20000.f, 1.f, 0.25f),
                                                           20000.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Freq",
                                                           "Peak Freq",
                                                           juce::NormalisableRange<float>(20.f,20000.f, 1.f, 0.25f),
                                                           750.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Gain",
                                                           "Peak Gain",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                           0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Quality",
                                                           "Peak Quality",
                                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                                                           1.f));
    
    
    
    // FilterTypes
    
    juce::StringArray stringArray;
    for( int i = 0; i < 4; ++i )
    {
        juce::String str;
        str << (12 + i*12);
        str << " db/Oct";
        stringArray.add(str);
    
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut Slope", "LowCut Slope", stringArray, 0 ));
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut Slope", "HighCut Slope", stringArray, 0 ));
    
    // IDEE
    
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    std::make_unique<juce::AudioParameterChoice>("HighCutSlope",
                                                 "HighCut Slope",
                                                 juce::StringArray {"12 db/Oct","24 db/Oct","36 db/Oct","48 db/Oct",}, 0);
    
    
    juce::AudioParameterChoice("Filter Slope",
                                                 "filter slope",
                                                 juce::StringArray {"HighCut","LowCut","Peak",}, 0);

    
    layout.add(std::make_unique<juce::AudioParameterBool>("Reverb Bypassed", "Reverb Bypassed", false));
    
    return layout;
    
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Comic_OneKnobAudioProcessor();
}


ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;
    
    settings.lowCutFreq = apvts.getRawParameterValue("LowCut Freq")->load();
    settings.highCutFreq = apvts.getRawParameterValue("HighCut Freq")->load();
    settings.peakFreq = apvts.getRawParameterValue("Peak Freq")->load();
    settings.peakGainInDecibels = apvts.getRawParameterValue("Peak Gain")->load();
    settings.peakQuality = apvts.getRawParameterValue("Peak Quality")->load();
    settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("LowCut Slope")->load());
    settings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("HighCut Slope")->load());
    
    settings.reverbBypassed = apvts.getRawParameterValue("Reverb Bypassed")->load() > 0.5f;
    return settings;
}


// PEAK

void Comic_OneKnobAudioProcessor::updatePeakFilter(const ChainSettings &chainSettings)
{
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                chainSettings.peakFreq,
                                                                                chainSettings.peakQuality,
                                                                                juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels));
    
    updateCoefficients(leftChain.template get<ChainPositions::Peak>().coefficients, peakCoefficients);
    updateCoefficients(rightChain.template get<ChainPositions::Peak>().coefficients, peakCoefficients);
}

void Comic_OneKnobAudioProcessor::updateCoefficients(Coefficients& old, const Coefficients& replacements)
{
    *old = *replacements;
}

//LOWCUT

void Comic_OneKnobAudioProcessor::updateLowCutFilters(const ChainSettings &chainSettings)
{
    auto cutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq, getSampleRate(), 2 * (chainSettings.lowCutSlope +1));
    
    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();
    
    updateCutFilter(leftLowCut, cutCoefficients, chainSettings. lowCutSlope);
    updateCutFilter(rightLowCut, cutCoefficients, chainSettings.lowCutSlope);

    
}


// HIGHCUT

    void Comic_OneKnobAudioProcessor::updateHighCutFilters(const ChainSettings &chainSettings)
{
    
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq,
                                                                                                          getSampleRate(),
                                                                                                          2 * (chainSettings.highCutSlope +1));
    
    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();
    
    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);
    updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);
    
}


// Update Filters

void Comic_OneKnobAudioProcessor::updateFilters()
{
    auto chainSettings = getChainSettings(apvts);
    
    updateLowCutFilters(chainSettings);
    updatePeakFilter(chainSettings);
    updateHighCutFilters(chainSettings);
}

