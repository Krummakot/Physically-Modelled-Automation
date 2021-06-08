/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AutoString.hpp"

//==============================================================================
PMA_MiniprojectAudioProcessorEditor::PMA_MiniprojectAudioProcessorEditor (PMA_MiniprojectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (700, 350);
}

PMA_MiniprojectAudioProcessorEditor::~PMA_MiniprojectAudioProcessorEditor()
{
}

//==============================================================================
void PMA_MiniprojectAudioProcessorEditor::paint (juce::Graphics& g)
{
    addAndMakeVisible(audioProcessor.myVisualString.get());
    
    if(noCalls < 5)
        noCalls++;
    
    g.fillAll (Colour(255,255,247));
    // output loc circle
    g.setColour(juce::Colours::black);
    g.drawEllipse(((float)getWidth()*0.73 * outputPosSlider.getValue()) + 1.f, (float)getHeight()*0.3, 2.f, 2.f, 2.f);
    
    // pluck loc circle
    g.setColour(juce::Colours::silver);
    g.drawEllipse(((float)getWidth()*0.73 * pluckLocSlider.getValue()) + 1.f, (float)getHeight()*0.32, 2.f, 2.f, 2.f);

    if(paintString) {
        resized();
        paintString = false;
    }

    startTimerHz(60);

    if(noCalls < 2) {
        
        // EXCITE WIDTH SLIDER
        exciteWidthSlider.addListener(this);
        exciteWidthSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        exciteWidthSlider.setColour(Slider::rotarySliderFillColourId, Colour(18,18,17));
        exciteWidthSlider.setColour(Slider::rotarySliderOutlineColourId, Colour(18,18,17));
        exciteWidthSlider.setColour(Slider::thumbColourId, Colour(68,102,187));
        exciteWidthSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 30);
        exciteWidthSlider.setRange(2, 10, 1);
        exciteWidthSlider.setColour(Slider::textBoxTextColourId, textColour);
        exciteWidthSlider.setColour(Slider::textBoxOutlineColourId, outlineColour);
        exciteWidthSlider.setValue(6);
        addAndMakeVisible(exciteWidthSlider);
        
        exciteWidthLabel.setText("Excite Width", dontSendNotification);
        exciteWidthLabel.attachToComponent(&exciteWidthSlider, false);
        exciteWidthLabel.setJustificationType(Justification::centred);
        exciteWidthLabel.setColour(Label::textColourId, textColour);
        addAndMakeVisible(exciteWidthLabel);
        
        // S0 DAMPING SLIDER
        s0DampingSlider.addListener(this);
        s0DampingSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        s0DampingSlider.setColour(Slider::rotarySliderFillColourId, Colour(18,18,17));
        s0DampingSlider.setColour(Slider::rotarySliderOutlineColourId, Colour(18,18,17));
        s0DampingSlider.setColour(Slider::thumbColourId, Colour(68,102,187));
        s0DampingSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 30);
        s0DampingSlider.setRange(0.001, 15.f, 0.0001f);
        s0DampingSlider.setColour(Slider::textBoxTextColourId, textColour);
        s0DampingSlider.setColour(Slider::textBoxOutlineColourId, outlineColour);
        s0DampingSlider.setValue(1.f);
        addAndMakeVisible(s0DampingSlider);
                
        s0DampingSliderLabel.setText("S0 Damping", dontSendNotification);
        s0DampingSliderLabel.attachToComponent(&s0DampingSlider, false);
        s0DampingSliderLabel.setJustificationType(Justification::centred);
        s0DampingSliderLabel.setColour(Label::textColourId, textColour);
        addAndMakeVisible(s0DampingSliderLabel);
        
        // S1 DAMPING SLIDER
        s1DampingSlider.addListener(this);
        s1DampingSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        s1DampingSlider.setColour(Slider::rotarySliderFillColourId, Colour(18,18,17));
        s1DampingSlider.setColour(Slider::rotarySliderOutlineColourId, Colour(18,18,17));
        s1DampingSlider.setColour(Slider::thumbColourId, Colour(68,102,187));
        s1DampingSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 30);
        s1DampingSlider.setRange(0.0001, 0.15, 0.00001f);
        s1DampingSlider.setColour(Slider::textBoxTextColourId, textColour);
        s1DampingSlider.setColour(Slider::textBoxOutlineColourId, outlineColour);
        s1DampingSlider.setValue(0.05f);
        addAndMakeVisible(s1DampingSlider);
                
        s1DampingSliderLabel.setText("S1 Damping", dontSendNotification);
        s1DampingSliderLabel.attachToComponent(&s1DampingSlider, false);
        s1DampingSliderLabel.setJustificationType(Justification::centred);
        s1DampingSliderLabel.setColour(Label::textColourId, textColour);
        addAndMakeVisible(s1DampingSliderLabel);
        
        // PLUCK POSITION SLIDER
        pluckLocSlider.addListener(this);
        pluckLocSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        pluckLocSlider.setColour(Slider::rotarySliderFillColourId, Colour(18,18,17));
        pluckLocSlider.setColour(Slider::rotarySliderOutlineColourId, Colour(18,18,17));
        pluckLocSlider.setColour(Slider::thumbColourId, Colour(68,102,187));
        pluckLocSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 30);
        pluckLocSlider.setRange(0.12, 0.88, 0.01f);
        pluckLocSlider.setColour(Slider::textBoxTextColourId, textColour);
        pluckLocSlider.setColour(Slider::textBoxOutlineColourId, outlineColour);
        pluckLocSlider.setValue(0.5f);
        addAndMakeVisible(pluckLocSlider);
            
        pluckLocLabel.setText("Pluck Location", dontSendNotification);
        pluckLocLabel.attachToComponent(&pluckLocSlider, false);
        pluckLocLabel.setJustificationType(Justification::centred);
        pluckLocLabel.setColour(Label::textColourId, textColour);
        addAndMakeVisible(pluckLocLabel);

        
        // OUTPUT POSITION SLIDER
        outputPosSlider.addListener(this);
        outputPosSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        outputPosSlider.setColour(Slider::rotarySliderFillColourId, Colour(18,18,17));
        outputPosSlider.setColour(Slider::rotarySliderOutlineColourId, Colour(18,18,17));
        outputPosSlider.setColour(Slider::thumbColourId, Colour(68,102,187));
        outputPosSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 30);
        outputPosSlider.setRange(0.12, 0.9, 0.01f);
        outputPosSlider.setColour(Slider::textBoxTextColourId, textColour);
        outputPosSlider.setColour(Slider::textBoxOutlineColourId, outlineColour);
        outputPosSlider.setValue(0.5f);
        addAndMakeVisible(outputPosSlider);
            
        outputPosLabel.setText("Output Pos", dontSendNotification);
        outputPosLabel.attachToComponent(&outputPosSlider, false);
        outputPosLabel.setJustificationType(Justification::centred);
        outputPosLabel.setColour(Label::textColourId, textColour);
        addAndMakeVisible(outputPosLabel);
        
        g.setColour(juce::Colours::black);
        g.drawEllipse((float)getWidth()*0.4, (float)getHeight()*0.6, 4.f, 4.f, 2.f);
        
        // TENSION SLIDER
        tensionSlider.addListener(this);
        tensionSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        tensionSlider.setColour(Slider::rotarySliderFillColourId, Colour(18,18,17));
        tensionSlider.setColour(Slider::rotarySliderOutlineColourId, Colour(18,18,17));
        tensionSlider.setColour(Slider::thumbColourId, Colour(68,102,187));
        tensionSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 30);
        tensionSlider.setRange(100.f, 310.0, 1.f);
        tensionSlider.setColour(Slider::textBoxTextColourId, textColour);
        tensionSlider.setColour(Slider::textBoxOutlineColourId, outlineColour);
        tensionSlider.setValue(105.f);
        addAndMakeVisible(tensionSlider);
            
        tensionLabel.setText("Tension", dontSendNotification);
        tensionLabel.attachToComponent(&tensionSlider, false);
        tensionLabel.setJustificationType(Justification::centred);
        tensionLabel.setColour(Label::textColourId, textColour);
        addAndMakeVisible(tensionLabel);
        
    //    EXCITATION MENU
        excitationMenu.addItem("Hanning", 1);
        excitationMenu.addItem("Triangle", 2);
        excitationMenu.onChange = [this] {excitationMenuChanged(); };
        excitationMenu.setSelectedId(2);
        addAndMakeVisible(excitationMenu);

        excitationLabel.setText("Excitaiton", dontSendNotification);
        excitationLabel.attachToComponent(&excitationMenu, false);
        excitationLabel.setJustificationType(Justification::centred);
        excitationLabel.setColour(Label::textColourId, Colour(juce::Colours::black));
        addAndMakeVisible(excitationLabel);
    }
}

void PMA_MiniprojectAudioProcessorEditor::resized()
{
    
    //    MY STRING VISUALS
    if(audioProcessor.myVisualString != nullptr) {
        audioProcessor.myVisualString->setBounds((float)getWidth()* 0.08,(float)getHeight()*0.2,(float)getWidth()*0.6, (float)getHeight()*0.45);
    }
    
//    stringLengthSlider.setBounds((float)getWidth()*0.733, (float)getHeight()*0.1428, (float)getWidth()*0.2, (float)getHeight()*0.43);
    
    exciteWidthSlider.setBounds((float)getWidth()*0.1, (float)getHeight()*0.75, (float)getWidth()*0.1066, (float)getHeight()*0.229);
    
    s0DampingSlider.setBounds((float)getWidth()*0.8, (float)getHeight()*0.5, (float)getWidth()*0.1066, (float)getHeight()*0.229);
    
    s1DampingSlider.setBounds((float)getWidth()*0.8, (float)getHeight()*0.1828, (float)getWidth()*0.1066, (float)getHeight()*0.229);
    
    pluckLocSlider.setBounds((float)getWidth()*0.25, (float)getHeight()*0.75, (float)getWidth()*0.1066, (float)getHeight()*0.229);
    
    outputPosSlider.setBounds((float)getWidth()*0.4, (float)getHeight()*0.75, (float)getWidth()*0.1066, (float)getHeight()*0.229);
    
    tensionSlider.setBounds((float)getWidth()*0.55, (float)getHeight()*0.75, (float)getWidth()*0.1066, (float)getHeight()*0.229);
    
    excitationMenu.setBounds((float)getWidth()*0.70, (float)getHeight()*0.75, (float)getWidth()*0.133, (float)getHeight()*0.057);
    
}

void PMA_MiniprojectAudioProcessorEditor::sliderValueChanged(Slider * slider) {
    
    if(slider == & outputPosSlider) {
        audioProcessor.setOutPosition(outputPosSlider.getValue());
    }
    
    if(slider == & pluckLocSlider) {
        audioProcessor.setPluckLocParam(pluckLocSlider.getValue());
    }
    
    if(slider == & exciteWidthSlider) {
        audioProcessor.setWidth(exciteWidthSlider.getValue());
        audioProcessor.updateStringClassCoefficients();
    }
    
    if(slider == & s0DampingSlider) {
        audioProcessor.setS0(s0DampingSlider.getValue());
        audioProcessor.updateStringClassCoefficients();
    }
    
    if(slider == & s1DampingSlider) {
        audioProcessor.setS1(s1DampingSlider.getValue());
        audioProcessor.updateStringClassCoefficients();
    }
    
    
    if(slider == & tensionSlider) {
        audioProcessor.setTension(tensionSlider.getValue());
        audioProcessor.updateStringClassCoefficients();
    }
}

void PMA_MiniprojectAudioProcessorEditor::mouseDown(const MouseEvent &e) {
    audioProcessor.mousePressed = true;
}

void PMA_MiniprojectAudioProcessorEditor::mouseUp(const MouseEvent &e) {
    audioProcessor.mousePressed = false;
    audioProcessor.playOnce = true;
}

void PMA_MiniprojectAudioProcessorEditor::excitationMenuChanged() {
    switch (excitationMenu.getSelectedId()) {
        case 1: audioProcessor.excitationSelection = 1; break;
        case 2: audioProcessor.excitationSelection = 2; break;
            
        default:
            break;
    }
}

void PMA_MiniprojectAudioProcessorEditor::timerCallback() {
    paintString = true;
    repaint();
    pluckLocSlider.setValue(audioProcessor.getPluckLocParam());
}

