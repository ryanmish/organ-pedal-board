# Fix Python Version Issue - Windows

If you're getting errors installing `python-rtmidi`, it's because Python 3.14 is too new. This guide will help you switch to Python 3.12.

---

## Step 1: Uninstall Python 3.14

### Option A: Using Windows Settings (Easiest)

1. Press `Windows Key`
2. Type: `Add or remove programs`
3. Press Enter
4. In the search box, type: `python`
5. Find **Python 3.14** in the list
6. Click it, then click **Uninstall**
7. Follow the prompts

### Option B: Using Command Prompt

Open Command Prompt as Administrator:
1. Press `Windows Key`
2. Type: `cmd`
3. Right-click **Command Prompt**
4. Click **Run as administrator**

Then run:
```
wmic product where "name like 'Python 3.14%'" call uninstall
```

---

## Step 2: Download Python 3.12

1. Go to this EXACT link:
   ```
   https://www.python.org/ftp/python/3.12.8/python-3.12.8-amd64.exe
   ```

2. Wait for the download to finish (it's about 25 MB)

---

## Step 3: Install Python 3.12

1. Find the downloaded file: `python-3.12.8-amd64.exe`
2. Double-click it to run the installer
3. **CRITICAL:** Check BOTH boxes:
   - ✅ **Add python.exe to PATH**
   - ✅ **Install launcher for all users**
4. Click **Install Now**
5. Wait for it to finish (2-3 minutes)
6. Click **Close**

---

## Step 4: Verify Python Installation

1. **Close any open Command Prompt windows** (this is important!)
2. Open a NEW Command Prompt:
   - Press `Windows Key + R`
   - Type: `cmd`
   - Press Enter

3. Check Python version:
   ```
   python --version
   ```

   You should see: `Python 3.12.8`

4. Check pip:
   ```
   python -m pip --version
   ```

   You should see something like: `pip 25.x.x from ...`

---

## Step 5: Install Dependencies (Clean Start)

Now that you have Python 3.12, install everything:

```
python -m pip install pyserial python-rtmidi pyinstaller
```

This should work now! You should see:
```
Successfully installed pyserial-3.5 python-rtmidi-1.5.8 pyinstaller-6.16.0 ...
```

---

## Step 6: Download the Project (If You Haven't Already)

If you already cloned the repo, skip this. Otherwise:

```
git clone https://github.com/ryanmish/organ-pedal-board.git
cd organ-pedal-board
```

---

## Step 7: Build the Windows App

```
python -m PyInstaller --onefile --windowed --name "Pedalboard MIDI Bridge" python/midi_bridge_gui.py
```

Wait 1-2 minutes. You'll see lots of output - that's normal!

---

## Step 8: Find Your App

```
explorer dist
```

Double-click **Pedalboard MIDI Bridge.exe** to test it!

---

## Troubleshooting

### "python is not recognized"

You didn't check "Add python.exe to PATH" during installation. Fix:
1. Uninstall Python 3.12
2. Reinstall and **make sure to check the PATH box**

### Still getting python-rtmidi errors?

Restart your computer and try Step 5 again.

### Need to go back to the full instructions?

See: `BUILD-WINDOWS-APP.md`

---

## Quick Reference - All Commands in Order

```
# After installing Python 3.12 with PATH checked:

python --version
python -m pip install pyserial python-rtmidi pyinstaller
git clone https://github.com/ryanmish/organ-pedal-board.git
cd organ-pedal-board
python -m PyInstaller --onefile --windowed --name "Pedalboard MIDI Bridge" python/midi_bridge_gui.py
explorer dist
```

Done! 🎉
