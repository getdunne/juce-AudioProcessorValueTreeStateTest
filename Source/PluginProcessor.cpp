#include "PluginProcessor.h"

// Factory function
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}


// PluginProcessor class
PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties().withInput  ("Input",  AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", AudioChannelSet::stereo(), true) )
    , valueTreeState(*this, &undoManager)
    , parameters(valueTreeState)
{
    // call state.createAndAddParameter() for all params...
    parameters.createAllParameters();

    // initialize the ValueTree object within our AudioProcessorValueTreeState
    valueTreeState.state = ValueTree(Identifier(JucePlugin_Name));
}

void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    ignoreUnused(samplesPerBlock);

    oscillator.setWaveform(parameters.waveform);
    oscillator.setFrequency(MidiMessage::getMidiNoteInHertz(parameters.midiNoteNumber) / sampleRate);
}

void PluginProcessor::releaseResources()
{
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void PluginProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ignoreUnused(midiMessages);

    oscillator.setWaveform(parameters.waveform);
    oscillator.setFrequency(MidiMessage::getMidiNoteInHertz(parameters.midiNoteNumber) / getSampleRate());

    float level = parameters.level;
    if (parameters.loud) level *= 2.0f;
    
    if (buffer.getNumChannels() == 1)
    {
        float* pLeft = buffer.getWritePointer(0);
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            *pLeft++ = oscillator.getSample() * level;
        }
    }
    else
    {
        float* pLeft = buffer.getWritePointer(0);
        float* pRight = buffer.getWritePointer(1);
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            *pLeft++ = *pRight++ = oscillator.getSample() * level;
        }
    }
}

void PluginProcessor::getStateInformation (MemoryBlock& destData)
{
    ScopedPointer<XmlElement> pXml(valueTreeState.state.createXml());
    copyXmlToBinary(*pXml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> pXml = getXmlFromBinary(data, sizeInBytes);
    if (pXml != nullptr)
        if (pXml->hasTagName(valueTreeState.state.getType()))
            valueTreeState.state = ValueTree::fromXml(*pXml);
}
