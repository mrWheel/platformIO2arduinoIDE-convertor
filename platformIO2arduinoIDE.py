############################################################################################################
#
#    convertor from platformIO to the ArduinoIDE
#
#    file name    : platformIO2arduinoIDE.py
#    by           : Willem Aandewiel
#
#    Version      : v1.0 (23-07-2024)
#
############################################################################################################
import os
import shutil

def clear_folder(folder_path):
    if os.path.exists(folder_path):
        for root, dirs, files in os.walk(folder_path):
            for file in files:
                file_path = os.path.join(root, file)
                os.remove(file_path)
            for dir in dirs:
                dir_path = os.path.join(root, dir)
                shutil.rmtree(dir_path)

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

def create_readme(dest_folder, lib_deps_text):
    readme_path = os.path.join(dest_folder, "README.h")
    with open(readme_path, 'w') as readme_file:
        readme_file.write("/*\n")
        readme_file.write("************************************************************************************\n")
        readme_file.write("**  This is the README file for the <name your project>  project.                 **\n")
        readme_file.write("************************************************************************************\n")
        readme_file.write("**\n")
        readme_file.write("**  Arduino-IDE settings for ESP-12E (but actualy for ALL type of ESP8266 boards):\n")
        readme_file.write("**\n")
        readme_file.write("**   - Board: \"Generic ESP8266 Module\" (ALLWAYS!!!!!)\n")
        readme_file.write("**   - Buildin Led: \"2\"\n")
        readme_file.write("**   - Upload Speed: \"115200\"\n")
        readme_file.write("**   - CPU Frequency: \"80 MHz\" (or if you need the speed: 160MHz)\n")
        readme_file.write("**   - Flash size: \"4MB (FS:2M OTA~1019KB)\"\n")
        readme_file.write("**   - Flash mode: \"DIO\" / \"DOUT\"\n")
        readme_file.write("**   - Flash Frequency: \"40MHz\"\n")
        readme_file.write("**   - Reset Method: \"nodemcu\" or something else\n")
        readme_file.write("**   - Debug port: \"Disabled\"\n")
        readme_file.write("**   - Debug Level: \"None\"\n")
        readme_file.write("**   - IwIP Variant: \"v2 Lower Memory\"\n")
        readme_file.write("**   - VTables: \"Flash\"\n")
        readme_file.write("**   - Exceptions: \"Legacy (new can return nullptr)\"\n")
        readme_file.write("**   - Erase Flash: \"Only Sketch\"\n")
        readme_file.write("**   - Espressif FW: \"nonos-sdk 2.2.1+100 (190703)\"\n")
        readme_file.write("**   - SSL Support: \"All SSL ciphers (most compatible)\"\n")
        readme_file.write("**   - Port: \"ESPticker at <-- IP address -->\"\n")
        readme_file.write("**\n")
        readme_file.write("**  Arduino ESP8266 core v2.7.+\n")
        readme_file.write("**\n")
        readme_file.write("**  Please make sure all the correct (versions!) of the libraries are in place\n")
        readme_file.write("**\n")
        readme_file.write("**  Dependencies from platformio.ini:\n")
        readme_file.write("**\n")
        readme_file.write(lib_deps_text)
        readme_file.write("**\n")
        readme_file.write("**\n")
        readme_file.write("************************************************************************************\n")
        readme_file.write("*/\n")
    print(f"Created README.h in {dest_folder}")

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

def copy_data_folder(platformio_base_folder, dest_folder):
    src_data_folder = os.path.join(platformio_base_folder, 'data')
    dest_data_folder = os.path.join(dest_folder, 'data')

    if os.path.exists(src_data_folder):
        shutil.copytree(src_data_folder, dest_data_folder)
        print(f"Copied data folder from {src_data_folder} to {dest_data_folder}")
    else:
        print(f"No data folder found at {src_data_folder}")

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

def main():
    platformio_base_folder = os.getcwd()
    platformio_src_folder = os.path.join(platformio_base_folder, 'src')
    platformio_include_folder = os.path.join(platformio_base_folder, 'include')
    platformio_ini_path = os.path.join(platformio_base_folder, 'platformio.ini')
    arduino_ide_folder = os.path.join(platformio_base_folder, "ArduinoIDE")

    print(f"ArduinoIDE needs a main.ino file in a directory with the same name!")
    print(f"In this python program that name is refered to as 'project name'.")
    project_name = input("Enter the project name: ")
    #project_name = "ESP_ticker"
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
  
