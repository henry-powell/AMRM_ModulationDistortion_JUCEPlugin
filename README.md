# ModulationDistortionPulser — JUCE AM/RM Modulation Distortion Plugin

## Plugin Interface

![AMRM Modulation Distortion Plugin](assets/plugin_ui.png)

![Language](https://img.shields.io/badge/Language-C%2B%2B-blue)
![Framework](https://img.shields.io/badge/Framework-JUCE-orange)
![Project Type](https://img.shields.io/badge/Project-Type%3A%20Audio%20Plugin-success)
![Domain](https://img.shields.io/badge/Domain-Audio%20DSP-critical)
![Focus](https://img.shields.io/badge/Focus-Modulation%20%26%20Distortion-informational)
![Grad Program](https://img.shields.io/badge/Grad%20Program-Audio%20Technology%20(M.A.)-purple)
![Institution](https://img.shields.io/badge/American%20University-Washington%2C%20DC-darkblue)

ModulationDistortionPulser is a JUCE-based audio plugin combining amplitude modulation (AM), ring modulation (RM), and nonlinear distortion stages. The project explores dynamic harmonic shaping through modulation-driven gain and clipping structures.

---

## Features

- **AM and RM modulation modes**
  - Selectable modulation type via parameter choice
- **Soft and Hard distortion types**
  - Nonlinear waveshaping options
- **Modulation frequency control**
  - 0–1000 Hz range
- **Drive control**
  - 0–50 dB gain staging
- **Pulse frequency modulation**
  - 0–20 Hz amplitude pulsing
- **Parameter-based architecture**
  - JUCE AudioProcessorValueTreeState (APVTS)

---

## File Contents

| File | Description |
|------|-------------|
| `PluginProcessor.cpp` | Core DSP processing logic |
| `PluginProcessor.h` | Processor class definition |
| `PluginEditor.cpp` | GUI layout and parameter attachments |
| `PluginEditor.h` | Editor class definition |
| `ModulationDistortionPulser.jucer` | JUCE project configuration |
| `README.md` | Project documentation |

---

## Technical Overview

The plugin architecture is built around:

- **JUCE AudioProcessor**
- **AudioProcessorValueTreeState (APVTS)**
- Frame-based modulation processing
- Nonlinear distortion using soft/hard clipping strategies
- Parameter-controlled modulation routing

AM and RM behaviors are implemented via modulation of signal amplitude using internal oscillation logic. Distortion is applied post-modulation, allowing dynamic harmonic enhancement based on modulation depth and drive levels.

This design enables experimental sound design textures, tremolo-like effects, and aggressive ring-modulated distortion tones.

---

## How to Build

1. Install JUCE
2. Open `ModulationDistortionPulser.jucer` in Projucer
3. Generate Xcode project files
4. Open in Xcode
5. Build AU/VST3 target

---

## Requirements

- JUCE Framework
- Xcode (macOS)
- C++17 or later

---

## Future Enhancements

- Oversampling for improved distortion quality
- Additional LFO waveform options
- Visual modulation feedback meter
- Preset management system
- Cross-platform builds (Windows)

---

© Henry Powell — Audio DSP Development
