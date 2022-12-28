/*
  ==============================================================================

    Phaser.h
    Created: 27 Dec 2022 3:56:38pm
    Author:  Kai

  ==============================================================================
*/

#pragma once

#include <cmath>
#include <JuceHeader.h>
#include <juce_audio_utils/juce_audio_utils.h>

class PhaserKI
{
public:
    // Constructor
    PhaserKI()
        : rate (1.0f), depth (1.0f), feedback (0.5f), wetDryMix (0.5f), phase (1.0f), sampleRate (44100.0f),
          lfoWaveform (0), lfoFrequency (0.05f), filterType (0), lfo (0.05f), filterCoeff (1.0f)
    {
        lastOutput.setSize (2, 44100); // Reserve space for 2 channels and 44100 samples
    }
    

    // Sets the rate of the phaser effect
    void setRate (float newRate)
    {
        rate = newRate;
    }

    // Sets the depth of the phaser effect
    void setDepth (float newDepth)
    {
        depth = newDepth;
    }

    // Sets the feedback of the phaser effect
    void setFeedback (float newFeedback)
    {
        feedback = newFeedback;
    }

    // Sets the wet/dry mix of the phaser effect
    void setWetDryMix (float newWetDryMix)
    {
        wetDryMix = newWetDryMix;
    }

    // Sets the LFO waveform of the phaser effect
    void setLfoWaveform (int newLfoWaveform)
    {
        lfoWaveform = newLfoWaveform;
    }

    // Sets the LFO frequency of the phaser effect
    void setLfoFrequency (float newLfoFrequency)
    {
        lfoFrequency = newLfoFrequency;
    }

    // Sets the filter type of the phaser effect
    void setFilterType (int newFilterType)
    {
        filterType = newFilterType;
    }
    
    template <typename ProcessContext>
    // Processes an audio block through the phaser effect
    void process (const ProcessContext& context)
    {
                // Copy the input samples to the output
        context.getOutputBlock().copyFrom (context.getInputBlock());

        // Calculate the LFO value based on the waveform and frequency
        switch (lfoWaveform)
        {
            case 0: // Sine wave
                lfo = std::sin (phase);
                break;
            case 1: // Triangle wave
                lfo = std::abs (std::fmod (phase / juce::MathConstants<float>::pi, 4.0f) - 2.0f) - 1.0f;
                break;
            case 2: // Square wave
                lfo = std::signbit (std::sin (phase)) ? -1.0f : 1.0f;
                break;
            case 3: // Sawtooth wave
                lfo = std::fmod (phase / juce::MathConstants<float>::pi, 2.0f) - 1.0f;
                break;
        }

        // Calculate the filter coefficient based on the filter type
        switch (filterType)
        {
            case 0: // Low-pass filter
                filterCoeff = 1.0f - depth * lfo;
                break;
            case 1: // High-pass filter
                filterCoeff = depth * lfo;
                break;
            case 2: // Band-pass filter
                filterCoeff = depth * (lfo + 1.0f);
                break;
        }

        // Loop through the channels in the audio block
        for (int channel = 0; channel < context.getOutputBlock().getNumChannels(); ++channel)
        {
            // Get a reference to the channel data
            float* channelData = context.getOutputBlock().getChannelPointer (channel);

            // Loop through the samples in the channel
            for (int i = 0; i < context.getOutputBlock().getNumSamples(); ++i)
            {
                 // Apply the phaser effect to the sample
                float x = channelData[i];
                float y = filterCoeff * x + feedback * lastOutput.getWritePointer (channel)[i];
                float output = y * filterCoeff + lastOutput.getWritePointer (channel)[i];
                channelData[i] = wetDryMix * output + (1.0f - wetDryMix) * x;

                
//                // Mix the wet and dry signals
//                channelData[i] = (1 - wetDryMix) * x + wetDryMix * output;
//
                
                // Update the last output sample for this channel
                lastOutput.getWritePointer (channel)[i] = output;
                
                

            }
        }
            
        

        // Update the phase
        phase += lfoFrequency * 2 * juce::MathConstants<float>::pi / sampleRate;
        if (phase > 2 * juce::MathConstants<float>::pi)
            phase -= 2 * juce::MathConstants<float>::pi;
    }

private:
    // Rate of the phaser effect
    float rate;

    // Depth of the phaser effect
    float depth;

    // Feedback of the phaser effect
    float feedback;

    // Wet/dry mix of the phaser effect
    float wetDryMix;

    // Phase of the phaser effect
    float phase;

    // Sample rate of the audio signal
    float sampleRate;

    // LFO waveform of the phaser effect (0 = sine, 1 = triangle, 2 = square, 3 = sawtooth)
    int lfoWaveform;

    // LFO frequency of the phaser effect
    float lfoFrequency;
    
    int filterType;

    // LFO value of the phaser effect
    float lfo;

    // Filter coefficient of the phaser effect
    float filterCoeff;

    // Last output sample of the phaser effect for each channel
    juce::AudioBuffer<float> lastOutput;



    
};
