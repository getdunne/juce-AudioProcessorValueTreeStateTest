#include "SynthWaveform.h"

const StringArray SynthWaveform::names = { "Sine", "Triangle", "Square", "Sawtooth" };

int SynthWaveform::textToIndex(const String& s)
{
    for (int i = 0; i < kNumberOfWaveformTypes; i++) if (s == names[i]) return i;

    // Were we given an invalid waveform name?
    jassertfalse;
    return 0;
}

void SynthWaveform::setFromName(String wfName)
{
    index = (WaveformTypeIndex)textToIndex(wfName);
}

String SynthWaveform::name()
{
    return names[index];
}

void SynthWaveform::setIndex(int i)
{
    if (i < 0) i = 0;
    if (i >= kNumberOfWaveformTypes) i = kNumberOfWaveformTypes - 1;
    index = (WaveformTypeIndex)i;
}

void SynthWaveform::setupComboBox(ComboBox& cb)
{
    for (int i = 0; i < kNumberOfWaveformTypes; i++)
        cb.addItem(names[i], i + 1);
}
