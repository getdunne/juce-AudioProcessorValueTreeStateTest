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
#include "PluginParameters.h"

// Names are symbolic names used to tag AudioParameter objects and XML attributes
// Labels are human-friendly identifiers for use in GUIs
const String PluginParameters::waveform_Id = "waveform";
const String PluginParameters::waveform_Name = TRANS("Waveform");
const String PluginParameters::waveform_Label;
const String PluginParameters::midiNoteNumber_Id = "midiNoteNumber";
const String PluginParameters::midiNoteNumber_Name = TRANS("Midi Note Number");
const String PluginParameters::midiNoteNumber_Label;
const String PluginParameters::level_Id = "level";
const String PluginParameters::level_Name = TRANS("Level");
const String PluginParameters::level_Label = TRANS("/10");
const String PluginParameters::loud_Id = "loud";
const String PluginParameters::loud_Name = TRANS("Loud");
const String PluginParameters::loud_Label;

PluginParameters::PluginParameters(AudioProcessorValueTreeState& vts)
    : valueTreeState(vts)
    , pWaveformAttachment(nullptr)
    , pNoteNumberAttachment(nullptr)
    , pLevelAttachment(nullptr)
    , pLoudAttachment(nullptr)
    , waveformListener(waveform)
    , noteNumberListener(midiNoteNumber)
    , levelListener(level, 0.1f)
    , loudListener(loud)
{
    // Set default values of working values
    // waveform already defaults to "Sine"
    level = 0.5f;
    loud = false;
    midiNoteNumber = 60;
}

void PluginParameters::createAllParameters()
{
    // waveform: choice out of 4 possibilities, values 0..3
    valueTreeState.createAndAddParameter(waveform_Id, waveform_Name, waveform_Label,
        NormalisableRange<float>(0.0f, (float)(SynthWaveform::kChoices - 1), 1.0f),
        (float)waveform.getIndex(),
        SynthWaveform::floatToText,
        SynthWaveform::textToFloat);
    valueTreeState.addParameterListener(waveform_Id, &waveformListener);

    // note number: integer parameter, range 0..127
    valueTreeState.createAndAddParameter(midiNoteNumber_Id, midiNoteNumber_Name, midiNoteNumber_Label,
        NormalisableRange<float>(0.0f, 127.0f, 1.0f),
        (float)midiNoteNumber,
        [](float value) { return MidiMessage::getMidiNoteName((int)value, true, true, 4); },
        [](const String& text) { return text.getFloatValue(); });
    valueTreeState.addParameterListener(midiNoteNumber_Id, &noteNumberListener);

    // level: float parameter, range 0.0-1.0, shown as 0.0-10.0 (scaled x10)
    valueTreeState.createAndAddParameter(level_Id, level_Name, level_Label,
        NormalisableRange<float>(0.0f, 10.0f),
        level,
        [](float value) { return String(value); },
        [](const String& text) { return text.getFloatValue(); } );
    valueTreeState.addParameterListener(level_Id, &levelListener);

    // loud: boolean parameter, range 0.0-1.0, show as "yes" or "no"
    valueTreeState.createAndAddParameter(loud_Id, loud_Name, loud_Label,
        NormalisableRange<float>(0.0f, 1.0f, 1.0f),
        loud ? 1.0f : 0.0f,
        [](float value) { return value < 0.5f ? "no" : "yes"; },
        [](const String& text) { return text == "yes" ? 1.0f : 0.0f; } );
    valueTreeState.addParameterListener(loud_Id, &loudListener);
}

void PluginParameters::detachControls()
{
    if (pWaveformAttachment != nullptr)
    {
        delete pWaveformAttachment;
        pWaveformAttachment = nullptr;
    }
    if (pNoteNumberAttachment != nullptr)
    {
        delete pNoteNumberAttachment;
        pNoteNumberAttachment = nullptr;
    }
    if (pLevelAttachment != nullptr)
    {
        delete pLevelAttachment;
        pLevelAttachment = nullptr;
    }
    if (pLoudAttachment != nullptr)
    {
        delete pLoudAttachment;
        pLoudAttachment = nullptr;
    }
}

void PluginParameters::attachControls(  ComboBox& waveformCombo,
                                        Slider& noteNumberSlider,
                                        Slider& levelSlider,
                                        ToggleButton& loudToggle)
{
    detachControls();   // destroy existing attachments, if any

    pWaveformAttachment = new ComboBoxAttachment(valueTreeState, waveform_Id, waveformCombo);
    pNoteNumberAttachment = new SliderAttachment(valueTreeState, midiNoteNumber_Id, noteNumberSlider);
    pLevelAttachment = new SliderAttachment(valueTreeState, level_Id, levelSlider);
    pLoudAttachment = new ButtonAttachment(valueTreeState, loud_Id, loudToggle);
}

void PluginParameters::putToXml(XmlElement& xml)
{
    // Set XML attributes based on working parameter values
    xml.setAttribute(waveform_Name, waveform.name());
    xml.setAttribute(midiNoteNumber_Name, midiNoteNumber);
    xml.setAttribute(level_Name, level);
    xml.setAttribute(loud_Name, loud);
}

void PluginParameters::getFromXml(XmlElement* pXml)
{
    // Set parameters based on XML attributes
    // Parameter listeners will propagate to working values

    SynthWaveform wf;
    wf.setFromName(pXml->getStringAttribute(waveform_Name));
    valueTreeState.getParameterAsValue(waveform_Id).setValue(wf.getIndex());

    int nn = pXml->getIntAttribute(midiNoteNumber_Name);
    valueTreeState.getParameterAsValue(midiNoteNumber_Id).setValue(nn);

    float lvl = (float)pXml->getDoubleAttribute(level_Name);
    valueTreeState.getParameterAsValue(level_Id).setValue(lvl);

    int isLoud = pXml->getBoolAttribute(loud_Name) ? 1 : 0;
    valueTreeState.getParameterAsValue(loud_Id).setValue(isLoud);
}
