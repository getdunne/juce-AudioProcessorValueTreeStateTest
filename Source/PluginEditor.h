#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "PluginParameters.h"

class PluginProcessor;

class PluginEditor  : public AudioProcessorEditor, private Button::Listener, private Timer
{
public:
    PluginEditor (PluginProcessor&);
    ~PluginEditor() { parameters.detachControls(); }

    void paint (Graphics&) override;
    void resized() override;

    void buttonClicked(Button* b) override;

private:
    void timerCallback() override;

    PluginProcessor& processor;
    PluginParameters& parameters;

    Label waveformLabel;
    ComboBox waveformCombo;
    Label noteNumberLabel;
    Slider noteNumberSlider;

    Label levelLabel;
    Slider levelSlider;
    Label loudLabel;
    ToggleButton loudToggle;

    TextButton undoButton, redoButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
