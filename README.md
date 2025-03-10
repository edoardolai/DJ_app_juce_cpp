# OtoDecks - Audio Mixing Application

OtoDecks is a DJ deck application built with the JUCE framework that allows users to load, play, and mix audio tracks with various controls and visualizations.



<img width="1440" alt="Screenshot 2025-03-10 at 20 15 45" src="https://github.com/user-attachments/assets/7609d067-b893-49af-9a0b-5eb8639b782d" />



## Features

- Dual deck audio playback system
- Waveform visualization
- Audio position scrubbing
- Speed/tempo adjustment
- Track library with search functionality
- Real-time audio processing
- Cross-fading between decks

## Prerequisites

Before building the project, ensure you have:

- CMake (version 3.15 or higher)
- C++ compiler with C++14 support
- Git

## Installation

1. Clone this repository:

   ```bash
   git clone https://github.com/yourusername/OtoDecks.git
   cd OtoDecks
   ```

2. Clone the JUCE framework repository:

   ```bash
   git clone https://github.com/juce-framework/JUCE.git
   ```

3. Run the build script to compile the application:

   ```bash
   chmod +x run.sh
   ./run.sh
   ```

   This script handles the CMake configuration and builds the project.

   If you are using Visual Studio Code, it is recommended to install the CMake Tools extension for a better development experience with this project.

## Usage

After building the project:

1. Launch the OtoDecks application
2. Use the "Load" buttons to add audio tracks to each deck
3. Press play to start audio playback
4. Adjust the volume sliders and speed controls as desired
5. Use the crossfader to mix between the two decks
6. Browse and manage your tracks in the library section

## Project Structure

- `Source/` - Contains all application source files
  - `MainComponent.cpp/h` - Main application UI
  - `DJAudioPlayer.cpp/h` - Audio playback engine
  - `DeckGUI.cpp/h` - Individual deck interface
  - `PlaylistComponent.cpp/h` - Track library management
  - `WaveformDisplay.cpp/h` - Audio visualization
- `JUCE/` - JUCE framework (added during installation)
- `run.sh` - Build script

## Development

To modify the project, edit the C++ files in the Source directory. After making changes, rebuild using the run.sh script.
