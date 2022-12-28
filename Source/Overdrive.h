/*
  =======================================================================
    Ove
    Created: 27 Dec 2022 3
    Autho
  ============================================================================*/

#pragma onec

#include <JuceHeader.h>
#include <cmath>

class Overdrive
{
public:
    // Constructor
    Overdrive()
        : gain (1.0f), threshold (1.0f)
    {
    }

    // Sets the gain of the overdrive effect
    void setGain (float newGain)
    {
        gain = newGain;
    }

    // Sets the threshold of the overdrive effect
    void setThreshold (float newThreshold)
    {
        threshold = newThreshold;
    }

    // Processes an audio block through the overdrive effect
    void process (float* buffer, int numSamples)
    {
        // Loop through the samples in the audio block
        for (int i = 0; i < numSamples; ++i)
        {
            // Apply the overdrive effect to the sample
            buffer[i] = overdrive (buffer[i]);
        }
    }

private:
    // Applies the overdrive effect to a single sample
    float overdrive (float x)
    {
        // Apply the gain to the input sample
        x *= gain;

        // Clip the input sample to the threshold level
        if (x > threshold)
            x = threshold + (x - threshold) / (1 + std::abs (x - threshold));
        else if (x < -threshold)
            x = -threshold + (x + threshold) / (1 + std::abs (x + threshold));

        // Return the distorted sample
        return x;
    }

    // Gain of the overdrive effect
    float gain;

    // Threshold level of the overdrive effect
    float threshold;    
};






;
