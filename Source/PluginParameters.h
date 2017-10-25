#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthWaveform.h"

typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

struct PluginParameters
{
    // Id's are symbolic names, Names are human-friendly names for GUI
    // Labels are supplementary, typically used for units of measure
    static const String waveform_Id, waveform_Name, waveform_Label;
    static const String midiNoteNumber_Id, midiNoteNumber_Name, midiNoteNumber_Label;
    static const String level_Id, level_Name, level_Label;
    static const String loud_Id, loud_Name, loud_Label;

    PluginParameters(AudioProcessorValueTreeState& vts);
    void createAllParameters();
    void detachControls();
    void attachControls(ComboBox& waveformCombo,
                        Slider& noteNumberSlider,
                        Slider& levelSlider,
                        ToggleButton& loudToggle);

    // Actual working parameter values
    SynthWaveform waveform;
    int midiNoteNumber;
    float level;
    bool loud;
    
    // get/put XML
    void putToXml(XmlElement& xml);
    void getFromXml(XmlElement* xml);

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Attachment objects link GUI controls to parameters
    ComboBoxAttachment* pWaveformAttachment;
    SliderAttachment* pNoteNumberAttachment;
    SliderAttachment* pLevelAttachment;
    ButtonAttachment* pLoudAttachment;

    // Specialized versions of AudioProcessorValueTreeState::Listener adapted for our parameter types
    struct WaveformListener : public AudioProcessorValueTreeState::Listener
    {
        SynthWaveform& waveform;

        WaveformListener(SynthWaveform& wf) : AudioProcessorValueTreeState::Listener(), waveform(wf) {}
        void parameterChanged(const String&, float newValue) override
        {
            waveform.setIndex((int)(newValue + 0.5f));
        }
    };

    struct IntegerListener : public AudioProcessorValueTreeState::Listener
    {
        int& workingValue;

        IntegerListener(int& wv) : AudioProcessorValueTreeState::Listener(), workingValue(wv) {}
        void parameterChanged(const String&, float newValue) override
        {
            workingValue = (int)newValue;
        }
    };

    struct FloatListener : public AudioProcessorValueTreeState::Listener
    {
        float& workingValue;
        float scaleFactor;      // multiply parameter values by this to get working value

        FloatListener(float& wv, float sf=1.0f) : AudioProcessorValueTreeState::Listener(), workingValue(wv), scaleFactor(sf) {}
        void parameterChanged(const String&, float newValue) override
        {
            workingValue = scaleFactor * newValue;
        }
    };

    struct BoolListener : public AudioProcessorValueTreeState::Listener
    {
        bool& workingValue;

        BoolListener(bool& wv) : AudioProcessorValueTreeState::Listener(), workingValue(wv) {}
        void parameterChanged(const String&, float newValue) override
        {
            workingValue = newValue >= 0.5f;
        }
    };

    WaveformListener waveformListener;
    IntegerListener noteNumberListener;
    FloatListener levelListener;
    BoolListener loudListener;
};
