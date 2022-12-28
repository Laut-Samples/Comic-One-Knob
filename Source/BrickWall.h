

class BrickWallLimiter
{
public:
    // Constructor
    BrickWallLimiter()
        : threshold (1.0f)
    {
    }

    // Sets the threshold level of the limiter
    void setThreshold (float newThreshold)
    {
        threshold = newThreshold;
    }

    // Processes an audio block throug  h the limiter
    void process (float* buffer, int numSamples)
    {
        // Loop through the samples in the audio block
        for (int i = 0; i < numSamples; ++i)
        {
            // Apply the limiter effect to the sample
            buffer[i] = limit (buffer[i]);
        }
    }

private:
    // Applies the limiter effect to a single sample
    float limit (float x)
    {
        // Clip the input sample to the threshold level
        if (x > threshold)
            x = threshold;
        else if (x < -threshold)
            x = -threshold;

        // Return the limited sample
        return x;
    }

    // Threshold level of the limiter
    float threshold;
};
