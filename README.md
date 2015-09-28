# Analog Devices ADIS16480 Interactive Demo
### Developed for the ADIS16480, ADF7242, and the Arduino/Teensy platform

This simple demo was created to show engineers how to integrate high performance inertial sensors into new or existing guidance system applications. The code provided in this repository will:
- Interface the ADIS16448 with a low cost microcontroller and acquire data from it at a set rate
- Write data to a FIFO located within the ADF7242 and transmit packets over the 2.4GHz ISM band
- Receive data packets from the ADF7242 and write the data to a serial port
- Display serial data in a demo-friendly application

In order to execute the application, the following packages must be downloaded.
- [Processing 2.2.1](http://download.processing.org/processing-2.2.1-windows32.zip) - Open source, simplified Java development environment
- [Teensyduino Serial Driver](https://www.pjrc.com/teensy/serial_install.exe) - Low cost development platform serial driver

### Contributing to the code

If you would like to help make the software better, you'll need to download a few more packages (this guide assumes you're working in Windows).
- [Arduino 1.0.6 IDE](http://arduino.cc/download.php?f=/arduino-1.0.6-windows.zip) - Open source, simplified microcontroller development environment
- [Teensyduino Libraries](https://www.pjrc.com/teensy/td_124/teensyduino.exe) - Additional libraries to add support for the Teensy development platform to the Arduino environment

### Hardware references

More information on the hardware used can be found below.       

- [ADIS16480](http://www.analog.com/media/en/technical-documentation/data-sheets/ADIS16480.pdf) - 10 DOF Inertial Sensor with Dynamic Orientation Outputs
- [ADF7242](http://www.analog.com/en/products/rf-microwave/integrated-transceivers-transmitters-receivers/low-power-rf-transceivers/adf7242.html) - Low Power IEEE 802.15.4/Proprietary GFSK/FSK Zero-IF 2.4 GHz Transceiver IC
- [Teensy 3.1](https://www.pjrc.com/teensy/teensy31.html) - 32 Bit USB Embedded Platform 
