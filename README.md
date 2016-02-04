Insect Tracking Project

This git repository contains all of the source code for running code to track
insect with computer vision as well as a number of tools that are necessary 
for the project to compile.


Download CMake
Generate new makefile with cmake and correct path
Make program

Download OpenCV 2.4.11 for linux and extract
Follow instructions on opencv website (http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html)

Download flycapture (https://www.ptgrey.com/KB/10548)
Follow instructions
Change buffer size under USBFS at https://www.ptgrey.com/KB/10685


UART Example 
Download https://github.com/mavlink/c_uart_interface_example
make; ./mavlink_control -d /dev/ttyACM0

Init MAVLINK submodule
while inside fyp git repo:
git submodule update --init --recursive

To remote control to NUC
Putty and XMing
Launch XMing and XMing Launch
Use 10.42.0.1 for and max sure to enable xforwarding under ssh. Use localhost:0

Download QGroundControl
sudo add-apt-repository ppa:beineri/opt-qt542-trusty
sudo add-apt-repository ppa:qgroundcontrol/ppa
sudo apt-get update
sudo apt-get install qgroundcontrol

Get simulator
https://pixhawk.org/dev/hil/jmavsim to install

java -Djava.ext.dirs= -cp lib/*:out/production/jmavsim.jar me.drton.jmavsim.Simulator -serial /dev/ttyACM0 57600 -qgc to run