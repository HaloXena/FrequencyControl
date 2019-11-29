#include "mbed.h"
#include "Ticker.h"
#include "C12832.h"
C12832 lcd(D11, D13, D12, D7, D10);


class Potentiometer
{
protected:
    AnalogIn inputSignal;
    float VDD, currentSamplePC, currentSampleVolts;
    float frequency;
public:
    Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v)
    {
        currentSamplePC = 0.0;
        currentSampleVolts = 0.0;
        frequency=0.0;
    }
    float amplitudeVolts(void)
    {
        return (inputSignal.read()*VDD);
    }
    float amplitudePC(void)
    {
        return inputSignal.read();
    }
    void sample(void)
    {
        currentSamplePC = inputSignal.read();
        currentSampleVolts = currentSamplePC * VDD;
    }
    float getCurrentSampleVolts(void)
    {
        return currentSampleVolts;
    }
    float getCurrentSamplePC(void)
    {
        return currentSamplePC;
    }
};


class SamplingPotentiometer : public Potentiometer
{
private:
    Ticker tkr;

public:
    SamplingPotentiometer(PinName pin, float frequencyin, float v) : Potentiometer(pin, v)
    {
        frequency = frequencyin;
        tkr.attach(callback(this, &Potentiometer::sample), (1/frequency));          //required to add frequency variable to parent class. private should be changed to protected
    };
};





class PwmSpeaker
{
private:
    Potentiometer* pot;
    PwmOut outputSignal;
    Timeout to;
    float period; // in sec
    float frequency; // in Hz
    float minFreq, maxFreq, frac; // min/max in Hz

public:
    PwmSpeaker(PinName pin, float freq, Potentiometer* potential, float ymn, float ymx) : outputSignal(pin), pot(potential), minFreq(ymn), maxFreq(ymx)
    {
        frequency=freq;
        period=(1/frequency);
        frac=pot->getCurrentSamplePC();
        updateFreq(minFreq, maxFreq, frac);
        
        to.attach(callback(this, &PwmSpeaker::updateRate), 0.001);
    };

    void setFreq(float f)
    {
        frequency = f;
    }
    void setPeriod(float p)
    {
        period = p;
    }

    float getFreq(void)
    {
        return frequency;
    }
    float getPeriod(void)
    {
        return period;
    }

    void updateFreq(float f)
    {
        frequency=f;
        outputSignal.period(1/f);
        outputSignal=0.5;
    };
    void updateFreq(float fn, float fx, float fc)
    {
        minFreq=fn;
        maxFreq=fx;
        frac=pot->getCurrentSamplePC();
        frequency= frac*(maxFreq-minFreq)+minFreq;
    };

    void updateRate(void)
    {   updateFreq(minFreq, maxFreq, frac);
        outputSignal.period(1/frequency);
        outputSignal=0.5;
        to.attach(callback(this, &PwmSpeaker::updateRate), period);
    }

};





int main()
{

    SamplingPotentiometer* pot = new SamplingPotentiometer(A0, 200,3.3);
    PwmSpeaker psp(D6, 500, pot, 500, 1000);

    while(1) {
    }
}
