# ARC Device

ARC Device is an IoT project built with mbed-os for home automation.

#### To build the project:

1. Install [Mbed CLI](https://os.mbed.com/docs/mbed-os/v6.0/build-tools/install-and-set-up.html) and dependencies
2. Download the required librairies by running `mbed sync`
3. Copy the `.mbed.template` file and add it to the root of the project calling it `.mbed`. This file is picked up by the mbed compiler to configure a few build settings. The minimum settings needed are already added to the file but other settings can be configured as well, please check with the mbed-cli documentation for more details.
4. Start the build process by running `mbed compile`

#### To develop using VS Code:

1. Install the C/C++ Intellisense extension
2. Configure the `.vscode/c_cpp_properties.json` file according to your target device and development environment. The config file included in this project is for a `NUCLEO_F401RE` target and `Linux` as the development environment.