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
