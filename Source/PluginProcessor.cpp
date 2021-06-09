/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PMA_MiniprojectAudioProcessor::PMA_MiniprojectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(outPutParam = new juce::AudioParameterFloat ("outputParam",
                                                              "Output",
                                                               0.0f, 1.0f,
                                                              0.5f));
    addParameter(pluckLocParam = new juce::AudioParameterFloat("pluckLocParam", "Pluck Location", 0.1f, 0.9f, 0.8f));
}

PMA_MiniprojectAudioProcessor::~PMA_MiniprojectAudioProcessor()
{
}

//==============================================================================
const juce::String PMA_MiniprojectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PMA_MiniprojectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PMA_MiniprojectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PMA_MiniprojectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PMA_MiniprojectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PMA_MiniprojectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PMA_MiniprojectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PMA_MiniprojectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PMA_MiniprojectAudioProcessor::getProgramName (int index)
{
    return {};
}

void PMA_MiniprojectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PMA_MiniprojectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    NamedValueSet parameters, parameters2;

    double r = 0.000127;
    
    // SoundString
    parameters.set("stringLength", 1);
    parameters.set("tension", 105.f);
    parameters.set("p", 7860);
    parameters.set("A", double_Pi * (r*r));
    parameters.set("E", 2.1000e+11);
    parameters.set("I", double_Pi * r * r * r * r * 0.25);
    parameters.set("s0", 1.f);
    parameters.set("s1",  0.005);
    
    // VisualString
    parameters2.set("stringLength", 1);
    parameters2.set("tension", 105.f);
    parameters2.set("p", 7860);
    parameters2.set("A", double_Pi * (r*r));
    parameters2.set("E", 2.1000e+11);
    parameters2.set("I", double_Pi * r * r * r * r * 0.25);
    parameters2.set("s0", 1.f);
    parameters2.set("s1",  0.005);
    
    fs = getSampleRate();
    
    myString = std::make_unique<SoundString>(parameters, fs);
    myVisualString = std::make_unique<VisualString>(parameters2,fs);
    
    myString.get();
    myVisualString.get();
    startTimerHz(60);

    DBG("sample rate:");
    DBG(sampleRate);
    
//    pluginHost.isAbletonLive();
//    std::cout<<pluginHost.isAbletonLive()<<" \n";

}

void PMA_MiniprojectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PMA_MiniprojectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PMA_MiniprojectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    MidiBuffer processMidi;
    float out;

    pluckLoc = *pluckLocParam;
    for (const auto metadata : midiMessages) {
        auto message = metadata.getMessage();
//        const auto time = metadata.samplePosition;

        if(message.isNoteOn()) {
            playOnce = true;
            velocityFullRange = message.getVelocity();
            velocity = (velocityFullRange/127);
            if (playOnce){
                myString->setPluckLoc(*pluckLocParam);
                myString->excite(excitationSelection, velocity, width);
                
                myVisualString->setPluckLoc(*pluckLocParam);
                myVisualString->excite(excitationSelection, velocity, width);
                playOnce = false;
            }
        }
//        processMidi.addEvent(message, time);
    }
//    midiMessages.swapWith(processMidi);
    
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

        float* const channelData1 = buffer.getWritePointer(0);
        float* const channelData2 = buffer.getWritePointer(1);
            for (int i = 0; i < buffer.getNumSamples(); ++i) {
                myString->processScheme();
                
                myString->updateStates();
                
                myVisualString->processScheme();
                myVisualString->updateStates();
                
                out = myString->getOutput(outPos);

                channelData1[i] = limit(out);
                channelData2[i] = limit(out);
            }
}

//==============================================================================
bool PMA_MiniprojectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PMA_MiniprojectAudioProcessor::createEditor()
{
    return new PMA_MiniprojectAudioProcessorEditor (*this);
}

//==============================================================================
void PMA_MiniprojectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PMA_MiniprojectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


void PMA_MiniprojectAudioProcessor::updateStringClassCoefficients() {
    myString->setStringLength(stringLength);
    myVisualString->setStringLength(stringLength);
    
    myString->setE(E);
    myVisualString->setE(E);
    
    myString->setDamping(s0, s1);
    myVisualString->setDamping(s0, s1);
    
    myString->setTension(tension);
    myVisualString->setTension(tension);
    
    myString->updateCoefficients();
    myVisualString->updateCoefficients();
}


double PMA_MiniprojectAudioProcessor::getE(){
    return this-> E;
}

void PMA_MiniprojectAudioProcessor::setE(int newE) {
    this-> E = newE;
}

void PMA_MiniprojectAudioProcessor::setWidth(int newWidth) {
    this-> width = newWidth;
}

double PMA_MiniprojectAudioProcessor::getKappa() {
    return this-> kappa;
}
void PMA_MiniprojectAudioProcessor::setKappa(double newKappa) {
    this-> kappa = newKappa;
}

void PMA_MiniprojectAudioProcessor::setS0(double newS0) {
    this-> s0 = newS0;
}

void PMA_MiniprojectAudioProcessor::setS1(double newS1) {
    this-> s1 = newS1;
}

void PMA_MiniprojectAudioProcessor::setPluckLoc(float pluckLoc) {
    this-> pluckLoc = pluckLoc;
}

void PMA_MiniprojectAudioProcessor::setPluckLocParam(float pluckLoc) {
    *pluckLocParam = pluckLoc;
}

float PMA_MiniprojectAudioProcessor::getPluckLocParam() {
    return *pluckLocParam;
}

void PMA_MiniprojectAudioProcessor::setOutPosition(double newOutPos) {
    this-> outPos = newOutPos;
}

double PMA_MiniprojectAudioProcessor::getN() {
    return this-> N;
}

void PMA_MiniprojectAudioProcessor::setStringLength(int newStringLength) {
    this->stringLength = newStringLength;
}

double PMA_MiniprojectAudioProcessor::getStringLength() {
    return this-> stringLength;
}

void PMA_MiniprojectAudioProcessor::setTension(double newTension) {
    this-> tension = newTension;
}

float PMA_MiniprojectAudioProcessor::limit(float input) {
    if(input > 1.f)
        return 1.f;
    else if (input < -1.f)
        return -1.f;
        
        return input;
}

void PMA_MiniprojectAudioProcessor::timerCallback() {
    outPutParam->operator=(((myString->getOutput(outPos) * 0.5f ) + 0.5f));
    midiHandling.setVelocity(myString->getOutput(outPos)*100.f);
    
//    midiOut.sendMessageNow(midiHandling);
//    std::cout<<*outPutParam<<" \n";
}

void PMA_MiniprojectAudioProcessor::paint(juce::Graphics& g) {
    myVisualString->paint(g);
}

void PMA_MiniprojectAudioProcessor::visualise(Graphics& g, double visualScaling) {
    myVisualString->visualiseState(g, visualScaling);
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PMA_MiniprojectAudioProcessor();
}
