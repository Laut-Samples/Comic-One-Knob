#pragma once

#include <JuceHeader.h>

#include <cmath>

class DistKI
{
public:
    // Constructor
    DistKI()
        : gain (1.0f), threshold (1.0f)
    {
    }

    // Sets the gain of the distortion effect
    void setGain (float newGain)
    {
        gain = newGain;
    }

    // Sets the threshold of the distortion effect
    void setThreshold (float newThreshold)
    {
        threshold = newThreshold;
    }

    // Processes an audio block through the distortion effect
    void process (float *buffer, int numSamples)
    {
        // Loop through the samples in the audio block
        for (int i = 0; i < numSamples; ++i)
        {
            // Apply the distortion effect to the sample
            buffer[i] = distort (buffer[i]);
        }
    }

private:
    // Applies the distortion effect to a single sample
    float distort (float x)
    {
        // Clip the input sample to the threshold level
        x = std::clamp (x, -threshold, threshold);

        // Apply the gain to the input sample
        x *= gain;

        // Return the distorted sample
        return x;
    }

    // Gain of the distortion effect
    float gain;

    // Threshold level of the distortion effect
    float threshold;
};
