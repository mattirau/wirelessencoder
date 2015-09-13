# Wireless encoder
A wireless rotary encoder controller with a capacitive touch button. This was designed to be used to control the LCD interface on a 3D printer.

![Alt text](/Images/Overview.jpg?raw=true "The transmitter unit")

## Hardware
The transmitter unit uses a Teensy 3.0 to read the capacitive touch taps and the rotation of the rotary encoder. The rotation and the tap data is sent over to the receiver with an nrf24l01 2.4 GHz module.

The receiving unit uses a 5V PRO Mini to simulate the button presses and the encoder rotation according to the data received from the transmitter.
