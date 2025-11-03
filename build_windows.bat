@echo off
REM Build script for Windows executable
REM Run this on a Windows machine to create the Windows app

echo Building Pedalboard MIDI Bridge for Windows...

REM Install requirements
python -m pip install pyserial python-rtmidi pyinstaller

REM Build the executable
python -m PyInstaller --onefile --windowed --name "Pedalboard MIDI Bridge" python/midi_bridge_gui.py

echo.
echo Build complete! The app is in the dist folder.
echo Double-click "Pedalboard MIDI Bridge.exe" to launch.
pause
