/*
  ===========================================================================
    Compressor
    Created: 27 Dec 2022 8:00:
    Author:  
  ============================================================================
*/
#pragma on
class CompressorKI
{
public:
    CompressorKI()
    {
        threshold = 0.0;
        ratio = 1.0;
        attackTime = 0.01;
        releaseTime = 0.1;
        gainReduction = 0.0;
    }
    void setgainReduction(float newgainReduction)
    {
        gainReduction = newgainReduction;
    }
    

    void setThreshold(float newThreshold)
    {
        threshold = newThreshold;
    }

    void setRatio(float newRatio)
    {
        ratio = newRatio;
    }

    void setAttackTime(float newAttackTime)
    {
        attackTime = newAttackTime;
    }

    void setReleaseTime(float newReleaseTime)
    {
        releaseTime = newReleaseTime;
    }

    void processSample(float &sample)
    {
        // Calculate the difference between the input level and the threshold
        float difference = fabs(sample) - threshold;

        // If the input level is below the threshold, set the gain reduction to 0
        if (difference <= 0)
        {
            gainReduction = 0;
        }
        else
        {
            // Calculate the amount of gain reduction needed
            float reduction = difference / ratio;

            // Smoothly increase or decrease the gain reduction value using the attack and release times
            if (gainReduction < reduction)
                gainReduction = gainReduction + (reduction - gainReduction) * attackTime;
            else
                gainReduction = gainReduction + (reduction - gainReduction) * releaseTime;
        }

        // Apply the gain reduction to the input sample
        sample = sample / (1 + gainReduction);
    }

private:
    float threshold;
    float ratio;
    float attackTime;
    float releaseTime;
    float gainReduction;
}



;
