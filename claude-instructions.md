# Instructions for Recreating platformIO2arduinoIDE.py

This document provides detailed instructions for recreating the `platformIO2arduinoIDE.py` file, which is a Python script designed to convert a PlatformIO project structure to an Arduino IDE compatible structure.

## File Structure

Create a new Python file named `platformIO2arduinoIDE.py` and follow these steps to recreate its contents:

1. Start with a header comment block explaining the purpose of the script, including the file name, author, and version information.

2. Import required modules:
   ```python
   import os
   import shutil
   ```

3. Define the following functions in order:

   a. `clear_folder(folder_path)`
   b. `copy_files(src_folder, dest_folder, extensions, project_name)`
   c. `create_readme(dest_folder, lib_deps_text)`
   d. `rename_files(dest_folder, project_name)`
   e. `copy_data_folder(platformio_base_folder, dest_folder)`
   f. `read_lib_deps(platformio_ini_path)`
   g. `main()`

4. Implement each function as described below.

## Function Implementations

### clear_folder(folder_path)

This function clears all contents of the specified folder.

```python
def clear_folder(folder_path):
    if os.path.exists(folder_path):
        for root, dirs, files in os.walk(folder_path):
            for file in files:
                file_path = os.path.join(root, file)
                os.remove(file_path)
            for dir in dirs:
                dir_path = os.path.join(root, dir)
                shutil.rmtree(dir_path)
```

### copy_files(src_folder, dest_folder, extensions, project_name)

This function copies files with specified extensions from the source folder to the destination folder.

```python
def copy_files(src_folder, dest_folder, extensions, project_name):
    if not os.path.exists(dest_folder):
        os.makedirs(dest_folder)

    for root, dirs, files in os.walk(src_folder):
        for file in files:
            if file.endswith(extensions):
                src_file_path = os.path.join(root, file)
                dest_file_path = os.path.join(dest_folder, file)
                if file == "main.cpp" and extensions == ('.cpp',):
                    dest_file_path = os.path.join(dest_folder, project_name + ".cpp")
                shutil.copy2(src_file_path, dest_file_path)
                print(f"Copied {file} to {dest_file_path}")
```

### create_readme(dest_folder, lib_deps_text)

This function creates a README.h file in the destination folder with Arduino IDE settings and library dependencies.

```python
def create_readme(dest_folder, lib_deps_text):
    readme_path = os.path.join(dest_folder, "README.h")
    with open(readme_path, 'w') as readme_file:
        readme_file.write("/*\n")
        # Write the content of the README file here
        # Include all the Arduino IDE settings and other information
        readme_file.write(lib_deps_text)
        readme_file.write("**\n")
        readme_file.write("**\n")
        readme_file.write("************************************************************************************\n")
        readme_file.write("*/\n")
    print(f"Created README.h in {dest_folder}")
```

### rename_files(dest_folder, project_name)

This function renames the main.cpp file to project_name.ino.

```python
def rename_files(dest_folder, project_name):
    main_cpp_path = os.path.join(dest_folder, "main.cpp")
    project_cpp_path = os.path.join(dest_folder, project_name + ".cpp")
    project_ino_path = os.path.join(dest_folder, project_name + ".ino")

    if os.path.exists(main_cpp_path):
        os.rename(main_cpp_path, project_cpp_path)
        print(f"Renamed main.cpp to {project_cpp_path}")

    if os.path.exists(project_cpp_path):
        os.rename(project_cpp_path, project_ino_path)
        print(f"Renamed {project_cpp_path} to {project_ino_path}")
```

### copy_data_folder(platformio_base_folder, dest_folder)

This function copies the data folder from the PlatformIO project to the Arduino IDE project.

```python
def copy_data_folder(platformio_base_folder, dest_folder):
    src_data_folder = os.path.join(platformio_base_folder, 'data')
    dest_data_folder = os.path.join(dest_folder, 'data')

    if os.path.exists(src_data_folder):
        shutil.copytree(src_data_folder, dest_data_folder)
        print(f"Copied data folder from {src_data_folder} to {dest_data_folder}")
    else:
        print(f"No data folder found at {src_data_folder}")
```

### read_lib_deps(platformio_ini_path)

This function reads the library dependencies from the platformio.ini file.

```python
def read_lib_deps(platformio_ini_path):
    lib_deps_text = ""
    lib_deps_start = False
    with open(platformio_ini_path, 'r') as file:
        for line in file:
            if lib_deps_start:
                if line.startswith(" ") or line.startswith("\t"):  # Check if the line is indented
                    lib_deps_text += "**  "+line
                else:
                    break  # Stop reading if a non-indented line is encountered
            elif "lib_deps" in line:
                key, value = line.split("=", 1)
                if key.strip() == "lib_deps":
                    lib_deps_start = True
                    lib_deps_text += "**  "+line  # Include the entire line with correct indentation
    return lib_deps_text.strip()
```

### main()

This is the main function that orchestrates the conversion process.

```python
def main():
    platformio_base_folder = os.getcwd()
    platformio_src_folder = os.path.join(platformio_base_folder, 'src')
    platformio_include_folder = os.path.join(platformio_base_folder, 'include')
    platformio_ini_path = os.path.join(platformio_base_folder, 'platformio.ini')
    arduino_ide_folder = os.path.join(platformio_base_folder, "ArduinoIDE")

    print(f"ArduinoIDE needs a main.ino file in a directory with the same name!")
    print(f"In this python program that name is refered to as 'project name'.")
    project_name = input("Enter the project name: ")
    dest_folder = os.path.join(arduino_ide_folder, project_name)

    # Clear the ArduinoIDE folder
    clear_folder(arduino_ide_folder)

    # Copy .cpp files from src
    copy_files(platformio_src_folder, dest_folder, ('.cpp',), project_name)

    # Copy .h files from include
    copy_files(platformio_include_folder, dest_folder, ('.h',), project_name)

    # Rename files as specified
    rename_files(dest_folder, project_name)

    # Copy data folder
    copy_data_folder(platformio_base_folder, dest_folder)

    # Read lib_deps from platformio.ini
    lib_deps_text = read_lib_deps(platformio_ini_path)

    # Create README.h
    create_readme(dest_folder, lib_deps_text)

if __name__ == "__main__":
    main()
```

## Conclusion

To recreate the `platformIO2arduinoIDE.py` file, copy all the provided function implementations into a new Python file in the order they are presented. Make sure to include the necessary import statements at the beginning of the file and the `if __name__ == "__main__":` block at the end to call the `main()` function when the script is run.

This script will convert a PlatformIO project structure to an Arduino IDE compatible structure, copying necessary files, renaming them as needed, and creating a README.h file with important project information.