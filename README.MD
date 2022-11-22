# xConnect Hardware repository

![scv](https://img.shields.io/badge/Required%20xConnect%20version-V1.0-brightgreen)
![discord](https://img.shields.io/discord/1034171027548278784?color=blue&label=xConnect%20discord&logo=discord&logoColor=white)
![license](https://img.shields.io/badge/license-CC%20BY--NC--SA%204.0-orange)

## Welcome

Welcome to the xConnect hardware github page, here you will find all of the official cluster implementations. This repository is a work in progress, and will be updated as new clusters are released.
All of the contributions to this repository are licensed under the [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/) license.
Contributors are always listed on the top of the main .ino file of each cluster.

## Required hardware

-   Arduino UNO or MEGA 2560
-   A CAN BUS shield, we recommend the [CAN-BUS Shield V2.0](https://wiki.seeedstudio.com/CAN-BUS_Shield_V2.0/)
-   A 12V >2.5A power supply
-   Jumper wires
-   Cluster that you want to connect

## Installation instructions

Firstly you will need to install the CAN BUS library, you can find it [here](https://github.com/coryjfowler/MCP_CAN_lib). After downloading it, you will need to install it in your Arduino IDE. You can find instructions on how to do that [here](https://www.arduino.cc/en/Guide/Libraries#toc4).

After you install the CAN BUS library, you can go onto installing the cluster implementation code onto the arduino. To do that you will need to download one of the [repository branches](https://github.com/InfoX1337/xConnect-hardware/branches). You can do this by clicking on the green "Code" button, and then clicking on "Download ZIP". After that, you will need to extract the ZIP file, and then open the .ino file in the Arduino IDE.

After you open the .ino file, you will need to select the correct board and port. You can do that by going to Tools -> Board -> Arduino UNO or MEGA 2560, and then Tools -> Port -> COMX (where X is the port number). After that, you can upload the code to the arduino.

## Testing

After you upload the code to the arduino, you can test it by connecting the arduino to the cluster. You can do that by connecting the CAN BUS shield to the arduino, and then connecting the cluster to the CAN BUS shield, like seen on the wiring diagram. After that, you can open the serial monitor in the Arduino IDE, and you should see the cluster data being printed to the serial monitor. If you don't see any data, you can try to change the baud rate in the serial monitor to 115200.

To check the Speed, Coolant temp, Fuel level and RPM functions you will need to send this: `4000:100:80:100:N:European:` into the serial monitor. After that, you should see the cluster rpm move to 4000, the speed should display 100, the coolant temp should display 80, the fuel level should display 100, and the gear should display N. If you don't see any changes, you can try to change the baud rate in the serial monitor to 115200.

## Connecting the arduino to xConnect

This section is not finished yet, and will be updated in the future once we release the xConnect desktop application.
