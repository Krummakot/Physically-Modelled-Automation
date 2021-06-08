/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "VisualString.hpp"
#include "AutoString.hpp"

//==============================================================================
/**
*/
class PMA_MiniprojectAudioProcessorEditor  : public juce::AudioProcessorEditor, public Slider::Listener, public Timer
{
public:
    PMA_MiniprojectAudioProcessorEditor (PMA_MiniprojectAudioProcessor&);
    ~PMA_MiniprojectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(Slider *slider) override;
    
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    float getOutputPosition();
    
    void excitationMenuChanged();
    void timerCallback() override;
    
    void setPluckLoc(float pluckLocParam);
    
//    bool mousePressed, playOnce;
    bool paintString = false;
    int noCalls = 0;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PMA_MiniprojectAudioProcessor& audioProcessor;
    
    
    Slider s0DampingSlider;
    Slider s1DampingSlider;
    Slider pluckLocSlider;
    Slider exciteWidthSlider;
    Slider tensionSlider;
    Slider outputPosSlider;
//    -----------------------
    Label s0DampingSliderLabel;
    Label s1DampingSliderLabel;
    Label pluckLocLabel;
    Label exciteWidthLabel;
    Label tensionLabel;
    Label outputPosLabel;
    
    Label excitationLabel;
    ComboBox excitationMenu;
    
    Path stringPath, boundaryPath;
    
    Colour backgroundColour = juce::Colours::ghostwhite;
    Colour textColour = Colour(18,18,17);
    Colour outlineColour = backgroundColour;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PMA_MiniprojectAudioProcessorEditor)
};
