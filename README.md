# PlatformIO to Arduino IDE Converter

This Python script converts a PlatformIO project structure to an Arduino IDE compatible format. It's designed to help developers transition their projects from PlatformIO to Arduino IDE with ease.

## Features

- Clears the target `ArduinoIDE` folder or creates one
- Copies `.cpp` files from the PlatformIO `src` directory
- Copies `.h` files from the PlatformIO `include` directory
- Renames `main.cpp` to `<project_name>.ino`
- Copies the `data` folder (if it exists)
- Creates a `README.h` file with Arduino IDE settings and library dependencies

## Usage

1. Place this script in the root directory of your PlatformIO project.
2. Run the script:
```
cd /path/to/your/project
python platformIO2arduinoIDE.py
```
3. The script will create an `ArduinoIDE` folder in your project directory, containing the converted project.

## Requirements

- Python 3.x
- A valid PlatformIO project structure

## How it works

1. The script first clears the target `ArduinoIDE` folder.
2. It then copies relevant files from the PlatformIO structure to the `ArduinoIDE` folder.
3. The `main.cpp` file is renamed to match the project name with an `.ino` extension.
4. If a `data` folder exists, it's copied to the `ArduinoIDE` project.
5. A `README.h` file is generated with Arduino IDE settings and library dependencies extracted from `platformio.ini`.

## Note

- ArduinoIDE needs a `project directory` with the same name as the `main.ino` file
- The program will prompt you for this `project` name.
- Ensure you have the necessary permissions to read from the PlatformIO project directory and write to the Arduino IDE directory.

## Version

1.0 (23-07-2024)

## Author

Willem Aandewiel

## MIT License

Copyright (c) 2024 Willem Aandewiel

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
