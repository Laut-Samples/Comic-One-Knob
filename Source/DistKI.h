#pragma once

#include <JuceHeader.h>

#include <cmath>

class DistKI
{
public:
    // Constructor
    DistKI()
  
    {
    }

    // Sets the gain of the distortion effect
    void setGain (int newGain)
    {
        gain = newGain;
    }

    // Sets the threshold of the distortion effect
    void setThreshold (int newThreshold)
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
    int distort (int x)
    {
        // Convert the input sample to a fixed-point value
        int xFixed = (int)(x * (1 << 15));

        // Clip the input sample to the threshold level
        xFixed = std::clamp (xFixed, -threshold, threshold);

        // Apply the gain to the input sample
        xFixed = (xFixed * gain) >> 15;

        // Convert the distorted sample back to a floating-point value
        float xDistorted = (float)xFixed / (float)(1 << 15);

        // Return the distorted sample
        return xDistorted;
    }

    // Gain of the distortion effect, stored as a 16-bit fixed-point value
    int gain;

    // Threshold level of the distortion effect, stored as a 16-bit fixed-point value
    int threshold;

};
