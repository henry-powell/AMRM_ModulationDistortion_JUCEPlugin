/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ModulationDistortionPulserAudioProcessor::ModulationDistortionPulserAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
                    #else
    : AudioProcessor()
#endif
, apvts (*this, nullptr, "PARAMS", createParameterLayout())

{
    DBG ("APVTS param count = " << apvts.state.getNumChildren());
    DBG ("APVTS state XML: " << apvts.state.toXmlString());
}

ModulationDistortionPulserAudioProcessor::~ModulationDistortionPulserAudioProcessor()
{
}

//==============================================================================
const juce::String ModulationDistortionPulserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ModulationDistortionPulserAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ModulationDistortionPulserAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ModulationDistortionPulserAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ModulationDistortionPulserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ModulationDistortionPulserAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ModulationDistortionPulserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ModulationDistortionPulserAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ModulationDistortionPulserAudioProcessor::getProgramName (int index)
{
    return {};
}

void ModulationDistortionPulserAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ModulationDistortionPulserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);

        mSampleRate = sampleRate;

    // reset per-channel phase (stereo)
    mModPhase[0] = juce::MathConstants<double>::halfPi;
    mModPhase[1] = juce::MathConstants<double>::halfPi;

    mPulsePhase[0] = juce::MathConstants<double>::halfPi;
    mPulsePhase[1] = juce::MathConstants<double>::halfPi;
}

void ModulationDistortionPulserAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ModulationDistortionPulserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ModulationDistortionPulserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                             juce::MidiBuffer& midiMessages)
{
    
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // read parameters once per block
    const float modFreq   = apvts.getRawParameterValue("modFreq")->load();
    const int   modType   = (int) apvts.getRawParameterValue("modType")->load();   // 0=AM, 1=RM
    const float driveDb   = apvts.getRawParameterValue("driveDb")->load();
    const int   distType  = (int) apvts.getRawParameterValue("distType")->load(); // 0=Soft, 1=Hard
    const float pulseFreq = apvts.getRawParameterValue("pulseFreq")->load();

    const double twoPi = juce::MathConstants<double>::twoPi;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* x = buffer.getWritePointer (channel);
        
        double modPhase = mModPhase[channel];
        double pulPhase = mPulsePhase[channel];
        
        // precompute phase increments
        const double modInc = (modFreq  > 0.0f) ? (twoPi * (double)modFreq  / mSampleRate) : 0.0;
        const double pulInc = (pulseFreq > 0.0f) ? (twoPi * (double)pulseFreq / mSampleRate) : 0.0;
        
        for (int n = 0; n < buffer.getNumSamples(); ++n)
            
        {
            float samp = x[n];

    // 1) RM/AM (first)
         
            const float modSin = (float) std::sin (modPhase);

            if (modType == 0) // AM
            {
                const float am = 0.5f * (modSin + 1.0f); // 0..1, at 0 Hz -> 1.0
                samp *= am;
            }
            else // RM
            {
                samp *= modSin; // at 0 Hz -> 1.0
            }

            // advance mod phase only if freq > 0
            if (modInc > 0.0)
            {
                modPhase += modInc;
                if (modPhase >= twoPi) modPhase -= twoPi;
            }

       
    // 2) Distortion (second) — bypass when driveDb == 0
      
            if (driveDb > 0.0f)
            {
                const float driveLin = juce::Decibels::decibelsToGain (driveDb);
                float d = samp * driveLin;

                if (distType == 0) // Soft
                    samp = std::tanh (d);
                else               // Hard
                    samp = juce::jlimit (-1.0f, 1.0f, d);
            }

     
    // 3) Amplitude pulsing (last)
          
            const float pulSin = (float) std::sin (pulPhase);
            const float pulAmp = 0.5f * (pulSin + 1.0f); // 0..1, at 0 Hz -> 1.0
            samp *= pulAmp;

            if (pulInc > 0.0)
            {
                pulPhase += pulInc;
                if (pulPhase >= twoPi) pulPhase -= twoPi;
            }

            x[n] = samp;
        }

        mModPhase[channel]   = modPhase;
        mPulsePhase[channel] = pulPhase;
    }
}

//==============================================================================
bool ModulationDistortionPulserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ModulationDistortionPulserAudioProcessor::createEditor()
{
    return new ModulationDistortionPulserAudioProcessorEditor (*this);
}

//==============================================================================
void ModulationDistortionPulserAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ModulationDistortionPulserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ModulationDistortionPulserAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout
ModulationDistortionPulserAudioProcessor::createParameterLayout()

{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    

    // RM/AM modulator frequency: 0 - 1000 Hz
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "modFreq", 1 },
        "Mod Freq",
        juce::NormalisableRange<float>(0.0f, 1000.0f, 0.01f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [] (float value, int) { return juce::String(value, 1) + " Hz"; },
        [] (const juce::String& text) { return text.getFloatValue(); }
    ));

    // Modulation type: 0 = AM, 1 = RM
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "modType", "Mod Type",
        juce::StringArray { "AM", "RM" },
        0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "driveDb", 1 },
        "Drive (dB)",
        juce::NormalisableRange<float>(0.0f, 50.0f, 0.01f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [] (float value, int) { return juce::String(value, 1) + " dB"; },
        [] (const juce::String& text) { return text.getFloatValue(); }
    ));

    // Distortion type: 0 = Soft, 1 = Hard
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "distType", "Dist Type",
        juce::StringArray { "Soft", "Hard" },
        0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "pulseFreq", 1 },
        "Pulse Freq",
        juce::NormalisableRange<float>(0.0f, 20.0f, 0.001f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [] (float value, int) { return juce::String(value, 1) + " Hz"; },
        [] (const juce::String& text) { return text.getFloatValue(); }
    ));

    return { params.begin(), params.end() };

}

