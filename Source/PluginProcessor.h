#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginEditor.h"
#include "PluginParameters.h"
#include "SynthOscillator.h"

class PluginProcessor : public AudioProcessor
{
public:
    PluginProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(AudioSampleBuffer&, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override { return new PluginEditor(*this); }
    bool hasEditor() const override { return true; }

    const String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const String getProgramName(int) override { return {}; }
    void changeProgramName (int, const String&) override {}

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Plugin's AudioProcessorValueTreeState, and its associated UndoManager
    AudioProcessorValueTreeState valueTreeState;
    UndoManager undoManager;

    // Application's view of the AudioProcessorValueTreeState, including working parameter values
    PluginParameters parameters;

    // Synthesis engine
    SynthOscillator oscillator;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
