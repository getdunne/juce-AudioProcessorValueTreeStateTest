# juce-AudioProcessorValueTreeStateTest
This is a very simple [JUCE](https://www.juce.com)-based audio plugin illustrating a *second approach* to handling parameter automation. (See [juce-AudioParameterTest](https://github.com/getdunne/juce-AudioParameterTest) for the first one.) It consists of a single oscillator driven by four parameters, accessible either via its own custom GUI, or via the host's (e.g. DAW) generic GUI and automation interface. For simplicity, it ignores MIDI and outputs sound continuously; it is thus a *generator* plugin rather than a *synthesizer*. (For examples of true JUCE synthesizers, see my [VanillaJuce](https://github.com/getdunne/VanillaJuce) and [SARAH](https://github.com/getdunne/SARAH) projects.)

As simple as this code may be, it is not a toy example. I have attempted to produce code which can be used as a template for realistic plugin projects with many more parameters. An important aspect of this is that all of the parameter-related code is encapsulated in a single **PluginParameters** class.

## Background
[JUCE](https://www.juce.com) version 5 introduces a new set of classes for handling communication of parameter-value changes between a plugin (VST, AU, etc.) and its host program (e.g. [DAW](https://en.wikipedia.org/wiki/Digital_audio_workstation)). These are built around **AudioProcessorValueTreeState**, whose definition includes a number of related classes for small objects such as Listeners, and an entirely new class, **AudioProcessorValueTreeState::Parameter** which essentially replaces the older class **AudioParameter** and its subclasses.

**AudioProcessorValueTreeState** and related classes offer two advantages over the older approach:
1. Less code is required, as new "attachment" objects take care of bi-directional communication between the Parameter objects and GUI controls.
2. Parameter objects are organized using a **ValueTree** structure, which can work with a **juce::UndoManager** object to provide full undo/redo support, with very little additional code.

I had difficulty finding code examples showing how these new classes ought to be used, so I decided to create my own. My goal was to verify what is called *automation* in audio-recording parlance, which requires true bi-directional communication of parameter values, i.e.,
- Plugin to host: user manipulates GUI controls, changes are recorded by a host DAW.
- Host to plugin: DAW recreates user's manipulations automatically using playback.

I have fully tested the Audio Unit (v2) build under Logic Pro X on the Mac, and the VST (v2) build under Reaper v5.52/x64 on Windows 10, including automation with all four parameters. I do not have the means to test other plugin types.

With JUCE v5.1, undo functionality was spotty, and redo didn't work at all. I was told these were "known bugs" at the time, but they appear to be fixed in JUCE 5.3.2.

## Detailed description

I intend to write a more detailed description of this code soon, which will be available at http://getdunne.net/wiki/doku.php?id=juce_and_parameter_automation.

Note that the **SynthWaveform** and **SynthOscillator** classes are taken directly from my [VanillaJuce](https://github.com/getdunne/VanillaJuce) synthesizer-plugin code; they are not new to this project.

I encourage you to compare this code with my earlier [juce-AudioParameterTest](https://github.com/getdunne/juce-AudioParameterTest) project, which I used as the starting point for this one. By design, the two plugins are almost identical.

This plugin has exactly four parameters, illustrating four distinct data types:
1. **Waveform** is a *choice* parameter, with options *sine, triangle, square,* and *sawtooth*.
2. **MIDI note number** is an integer parameter, in the range [0..127].
3. **Level** is a float parameter, in the range [0, 1.0]
4. **Loud** is a Boolean parameter. When true, the *level* setting is effectively doubled.

The GUI also includes "Undo" and "Redo" buttons, which trigger the corresponding actions in a **juce::UndoManager** object. At the time of writing, though, the Undo button doesn't work as cleanly as I would prefer, and **the Redo button doesn't work at all**. I would be very grateful for any feedback. You can reach me on the [JUCE Forum](https://forum.juce.com/) as user **getdunne**.

## Code licensing terms
This code is licensed under the terms of the MIT License (see the file *LICENSE* in this repo). To compile it, you will need a copy of the [JUCE framework](https://juce.com), and the resulting *combined work* will be subject to JUCE's own licensing terms, and under certain circumstances may become subject to the [GNU General Public License, version 3 (GPL3)](https://www.gnu.org/licenses/gpl-3.0.en.html).

I am grateful to Julian Storer of Roli, Inc. for clarifying, via the [JUCE Forum](https://forum.juce.com/t/open-source-without-gpl/29721), that this code will continue to be freely usable under the terms of the MIT license, because
1. The MIT license is "GPL3 compatible" according to the Free Software Foundation.
2. Clause 5(c) of the GPL3 specifically states that it does not invalidate less restrictive usage permissions explicitly granted by a copyright holder.
3. The JUCE license does not affect copyright status of MIT-licensed code in combined works.

In light of these considerations, and for maximum clarity, I have added my copyright notice and the full text of the MIT license to every one of the source files in this repo.

Shane Dunne, October 2018
