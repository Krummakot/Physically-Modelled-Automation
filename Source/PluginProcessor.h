/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SoundString.hpp"
#include "VisualString.hpp"


//==============================================================================
/**
*/
class PMA_MiniprojectAudioProcessor  : public juce::AudioProcessor, juce::Timer
{
public:
    //==============================================================================
    PMA_MiniprojectAudioProcessor();
    ~PMA_MiniprojectAudioProcessor() override;

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
    
    float limit(float input);
    
    void updateCoefficients();
    void updateStringClassCoefficients();
    
    
    double getKappa();
    void setKappa(double newKappa);
    void setStringLength(int newStringLength);
    void setS0(double newS0);
    void setS1(double newS1);
    void setPluckLoc(float pluckLoc);
    void setPluckLocParam(float pluckLoc);
    float getPluckLocParam();
    void setOutPosition(double newOutPos);
    double getN();
    double getStringLength();
    void setTension(double newTension);
    void paint(juce::Graphics& g);
    void visualise(Graphics& g, double visualScaling);
    
    double getE();
    void setE(int newE);
    void setWidth(int newWidth);
    
    void timerCallback() override;

    
    int stringLength = 1;
    bool mousePressed, playOnce;
    double currentSample;
    int excitationSelection;
    int width = 10;
    AudioParameterFloat* outPutParam;
    AudioParameterFloat* pluckLocParam;
    VisualString* newVisualString;
    std::unique_ptr<SoundString> myString;
    std::unique_ptr<VisualString> myVisualString;

private:
    //==============================================================================
    
    
    double fs, f0, c, k, s0, s1, h, N, lambdaSq, muSq, pluckLoc, stringPluckRatio, kappa;

    double eMultiplier;
    
    double excitationRange, outPos;
    std::vector<double> u, uPrev, uNext;
    int expectedSamplesPerBlock = 0;
    juce::Random random;
    
    double damp, stiffness, tension, stringDiameter, stringRadius, p, A, E, I;
    
    double velocityFullRange, velocity;
//    juce::MidiOutput midiOut;
    MidiMessage midiHandling;
    
    PluginHostType pluginHost;
    

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PMA_MiniprojectAudioProcessor)
};
