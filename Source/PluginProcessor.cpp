/* Copyright (c) 2017-2018 Shane D. Dunne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
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
