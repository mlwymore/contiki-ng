Instructions to Compile and Flash contiki OS

1. Compile Sensor Tag:
    `make TARGET=cc26x0-cc13x0 BOARD=sensortag/cc2650 receiver-broadcast`

After compilation a binary file will be generated in build directory

2. Flash sensortag:
    Choose LAUNCHXL-CC2650 launchpad in Choose Your Device section of UniFlash then click on start to start a new session.
    Now select a bin which you just generated and click on Load image to flash it to the device.

3. Connection with UART:
    `make TARGET=cc26x0-cc13x0 BOARD=sensortag/cc2650 PORT=/dev/ttyACM0 login`