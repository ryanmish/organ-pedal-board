# How to Build the Windows App from Scratch

This guide will walk you through building the `Pedalboard MIDI Bridge.exe` for Windows, even if you've never used GitHub or Python before.

---

## Prerequisites (One-Time Setup)

### Step 1: Install Python

1. Go to: https://www.python.org/downloads/
2. Click **"Download Python 3.12.x"** (or latest version)
3. Run the installer
4. **IMPORTANT:** Check the box that says **"Add Python to PATH"**
5. Click **"Install Now"**
6. When done, click **"Close"**

### Step 2: Install Git (to download the code)

1. Go to: https://git-scm.com/download/win
2. Download the installer
3. Run it and click **"Next"** through all the options (defaults are fine)
4. Click **"Install"**
5. When done, click **"Finish"**

---

## Building the Windows App

### Step 1: Open Command Prompt

1. Press `Windows Key + R`
2. Type: `cmd`
3. Press Enter

### Step 2: Download the Project

Copy and paste this command into Command Prompt and press Enter:

```
git clone https://github.com/YOUR-USERNAME/organ-pedal-board.git
```

> **Note:** Replace `YOUR-USERNAME` with the actual GitHub username

Then navigate into the folder:

```
cd organ-pedal-board
```

### Step 3: Install Python Dependencies

Copy and paste this command and press Enter:

```
pip install pyserial python-rtmidi pyinstaller
```

Wait for it to finish (takes about 1-2 minutes).

### Step 4: Build the App

Copy and paste this command and press Enter:

```
pyinstaller --onefile --windowed --name "Pedalboard MIDI Bridge" python/midi_bridge_gui.py
```

This will take 1-2 minutes. You'll see lots of text scrolling by - that's normal!

### Step 5: Find Your App

When it's done, your app will be in:

```
organ-pedal-board\dist\Pedalboard MIDI Bridge.exe
```

To open the folder in File Explorer:

```
explorer dist
```

**Done!** You can now double-click `Pedalboard MIDI Bridge.exe` to run it!

---

## Quick Build Script (After First-Time Setup)

If you've already done the prerequisites, you can use the automated build script:

### Option A: Using the Batch File

1. Open File Explorer
2. Navigate to the `organ-pedal-board` folder
3. Double-click `build_windows.bat`
4. Wait for it to finish
5. Your app will be in the `dist` folder

### Option B: Using Command Prompt

```
cd organ-pedal-board
build_windows.bat
```

---

## Troubleshooting

### "Python is not recognized as an internal or external command"

You didn't check "Add Python to PATH" during installation. Fix:
1. Uninstall Python (Control Panel → Programs → Uninstall)
2. Reinstall Python and **check the "Add Python to PATH" box**

### "git is not recognized as an internal or external command"

Git didn't install correctly. Fix:
1. Restart your computer
2. Try the git command again
3. If still broken, reinstall Git from https://git-scm.com/download/win

### "pip install" is taking forever

This is normal the first time! It's downloading the libraries. Wait 2-3 minutes.

### The .exe file is huge (50+ MB)

This is normal! PyInstaller bundles Python and all libraries into one file.

---

## Sharing the App with Others

Once you've built `Pedalboard MIDI Bridge.exe`, you can:

1. Copy it to a USB drive
2. Upload it to Google Drive/Dropbox
3. Share it with anyone - **they don't need Python or any dependencies!**

The `.exe` file is completely standalone.

---

## Need Help?

If you get stuck, open an issue on GitHub:
https://github.com/YOUR-USERNAME/organ-pedal-board/issues

Include:
- What step you're on
- The exact error message you see
- A screenshot (if possible)
