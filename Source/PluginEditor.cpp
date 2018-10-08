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
#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor(&p)
    , processor(p)
    , parameters(p.parameters)
    , waveformLabel(PluginParameters::waveform_Id, PluginParameters::waveform_Name)
    , noteNumberLabel(PluginParameters::midiNoteNumber_Id, PluginParameters::midiNoteNumber_Name)
    , levelLabel(PluginParameters::level_Id, PluginParameters::level_Name)
    , loudLabel(PluginParameters::loud_Id, PluginParameters::loud_Name)
    , undoButton(TRANS("Undo"))
    , redoButton(TRANS("Redo"))
{
    auto initLabel = [this](Label& label)
    {
        addAndMakeVisible(label);
        label.setFont(Font(15.00f, Font::plain).withTypefaceStyle("Regular"));
        label.setJustificationType(Justification::centredRight);
        label.setEditable(false, false, false);
        label.setColour(TextEditor::textColourId, Colours::black);
        label.setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    };

    initLabel(waveformLabel);
    initLabel(noteNumberLabel);
    initLabel(levelLabel);
    initLabel(loudLabel);

    auto initCombo = [this](ComboBox& combo)
    {
        addAndMakeVisible(combo);
        combo.setEditableText(false);
        combo.setJustificationType(Justification::centredLeft);
        combo.setTextWhenNothingSelected("");
        combo.setTextWhenNoChoicesAvailable(TRANS("(no choices)"));
    };

    initCombo(waveformCombo);
    SynthWaveform::setupComboBox(waveformCombo);

    auto initSlider = [this](Slider& slider)
    {
        addAndMakeVisible(slider);
        slider.setSliderStyle(Slider::LinearHorizontal);
        slider.setTextBoxStyle(Slider::TextBoxRight, false, 80, 20);
    };

    initSlider(noteNumberSlider); //noteNumberSlider.setRange(0, 127, 1);
    initSlider(levelSlider); //levelSlider.setRange(0, 1, 0);

    auto initToggle = [this](ToggleButton& toggle)
    {
        addAndMakeVisible(toggle);
    };

    initToggle(loudToggle);

    // Note slider attachments will set slider ranges automatically
    parameters.attachControls(waveformCombo, noteNumberSlider, levelSlider, loudToggle);

    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);
    undoButton.addListener(this);
    redoButton.addListener(this);

    // clear the undo manager now, because this is our starting point
    // (Setting up the ValueTree will have added many actions to the history, which 
    // aren't actually supposed to be undoable.)
    processor.undoManager.clearUndoHistory();

    // execute timer callback once immediately, then every 500ms thereafter
    timerCallback();
    startTimer(500);

    setSize (600, 200);
}

void PluginEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    const int labelLeft = 16;
    const int controlLeft = 144;
    const int labelWidth = 120;
    const int cboxWidth = 150;
    const int sliderWidth = 420;
    const int toggleWidth = 24;
    const int buttonWidth = 80;
    const int buttonGap = 20;
    const int controlHeight = 24;
    const int gapHeight = 8;

    int top = 20;
    waveformLabel.setBounds(labelLeft, top, labelWidth, controlHeight);
    waveformCombo.setBounds(controlLeft, top, cboxWidth, controlHeight);
    top += controlHeight + gapHeight;
    noteNumberLabel.setBounds(labelLeft, top, labelWidth, controlHeight);
    noteNumberSlider.setBounds(controlLeft, top, sliderWidth, controlHeight);
    top += controlHeight + gapHeight;
    levelLabel.setBounds(labelLeft, top, labelWidth, controlHeight);
    levelSlider.setBounds(controlLeft, top, sliderWidth, controlHeight);
    top += controlHeight + gapHeight;
    loudLabel.setBounds(labelLeft, top, labelWidth, controlHeight);
    loudToggle.setBounds(controlLeft, top, toggleWidth, controlHeight);
    top += controlHeight + gapHeight;
    undoButton.setBounds(controlLeft, top, buttonWidth, controlHeight);
    redoButton.setBounds(controlLeft + buttonWidth + buttonGap, top, buttonWidth, controlHeight);
}

void PluginEditor::buttonClicked(Button* button)
{
    if (button == &undoButton)
    {
        processor.undoManager.undo();
    }
    else if (button == &redoButton)
    {
        processor.undoManager.redo();
    }
}

void PluginEditor::timerCallback()
{
    // Doing this on a timed basis is copied from the JUCE ValueTreesDemo, but it's not an ideal
    // approach because we don't want to start new transactions in the middle of an update gesture
    // like dragging a slider.
    processor.undoManager.beginNewTransaction();

    undoButton.setEnabled(processor.undoManager.canUndo());
    redoButton.setEnabled(processor.undoManager.canRedo());
}