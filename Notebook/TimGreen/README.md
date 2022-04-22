# 2/8/2022
## Proposal Brainstorming
### High Level Requirements
* The RFID Sensor must be able to sense and distinguish the 52 different RFID tags associated with each card and provide the card ID signal to the microprocessor
* The microprocessor must be able to receive a card ID value and send this value to the iOS App via bluetooth packet.
* The iOS App must be able to receive card ID info via a bluetooth packet and use the currently played cards to determine an optimal move for the player.
	* Include the probability of this choice leading to a winning hand?
### Module Components and Requirements
* RFID Tags:
 	* passive RFID tags with enough bits to hold serial number to distinguish between 52 cards
* RFID Sensor:
	* specify voltage requirements for power
	* must be able to generate RF signal in specified tag frequency range
		* 860-960 MHz (frequency for digikey tags)
	* must be able to receive RF signal holding serial number of RFID tag
	* must be able to generate specific output signal (different for each of 52 cards/tags)
		* serial or parallel (probably serial)
* Processor:
	* specify voltage requirement for power
	* must be able to receive RFID sensor signal input (card serial number)
	* must be able to create a bluetooth packet holding card ID and output packet to bluetooth transceiver
	* must be able to receive and read bluetooth packets inputs from the bluetooth transceiver
* Battery:
	* Cell Type, Output Voltage, Output Current
	* Is the battery chargeable?
	* Battery Management System
		* Know when the battery is low and signal this (LED or send to app)
	* Voltage Regulator
* Bluetooth Transceiver:
	* RF transmitter and receiver (2.4 GHz)
	* take in packet and transmit
	* receive packet and send to processor
	* input voltage requirements
* iOS App:
	* Must be able to send packets to the bluetooth device
		* Pairing requests
		* Request for card
	* Must be able to receive packets holding card ID
	* Must be able to take user input
		* Reset or end of hand
		* Game type and number of players (add players between hands)
		* "Backspace" for rescanning a card
	* Must be able to follow the course of the game and display next actions for each step of the game
	* Must be able to determine and display the best move for each player
### Risk to Successful Completion
* Implementing logic within the app to output the proper odds and moves
* Keeping track of what is on the table and reset after each hand
## TA Meeting Notes
* More complex in hardware
	* Add chargeable battery with battery management system, voltage regulator, and ESD protection
	* Add central display for dealer instructions and maybe current cards
* Keep adding to proposal throughout the project
* For Next Week:
	* Find specific components
	* specific requirements for all the modules and components

# 2/10/2022
## Proposal Work
Proposal work all written in proposal file in project files. The [Finished Proposal](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/Proposal/OddsBoosterProposal.pdf) will be included within this notebook.

# 2/14/2022
## Machine Shop Meeting Notes
* Project doesn't have too much mechanical complexity besides case so should be easy
* Machine shop will purchase case and worry about constructing the design so just focus on getting parts list and plan asap
* Look into getting a separate charging port that can be wired to PCB (don't mount it on PCB)

# 2/15/2022
## Circuit Design and Component Search
All found parts will be in the resources folder along with key points of the part

# 2/19/2022
## Bluetooth Module
* New bluetooth module has been chosen (ESP32-WROVER-E) for the following reasons
	* Bluetooth RF hardware (2.4GHz) is high frequency and requires precise traces when transmitting signals between components on the PCB. Combined module containing the source of data (MCU) and the bluetooth RF components (antenna) allows for abstraction of those high frequency traces and avoids what would have been the tolerance condition. This also makes the overall device much cheaper requiring fewer purchased components and using the included components more efficiently
	* ESP modules also have extensive documentation and development tools improving our potential ability to create a working program for the microcontroller to communicate to another device.
* The ESP32-WROVER-E Operation + Hardware Notes
	* Boot Mode specified by GPIO0
		* SPI boot (load program from internal flash) - GPIO0(IO0) = 1, GPIO2(IO2) = X
		* Download boot (download new program through uart) - GPIO0(IO0) = 0, GPIO2(IO2) = 0
	* EN pin serves as reset so button (RC filter after this for basic debouncing) (active high so should be GND when pressed and have pull up resistor to Vcc) will control power on and programming (see boot mode above)
	* USB to UART interface device to allow for use with ESP IDE and development tools to program the microcontroller. This means UART connection pins will be sent to pins accessible by jumper wires.
	* VSPI connections can be used for communication with the RFID module.
	* Available JTAG connections for debugging. The JTAG usage operations will have to be investigated further
	* Registers can be adjusted as part of the boot/programming sequence or as first instructions within the code. This allows for control over the GPIO operations, the defined interfaces (SPI, UART, etc), and clock outputs
	* Module includes clock that can be output at up to 160MHz allowing for fast operations useful for display control

# 2/20/2022
## LCD Display Adjustment
* Current LCD requires 24bit parallel RGB input (MCU does not have that many GPIOs)
* The output clocks available from the MCU are much faster than the specified clock requirements of the display.
* Shift registers allow for usage of high clock capabilites with the lower number of GPIOs
* Will need to determine how to operate the MCU GPIOs to provide data with this speed (similar to the FPGA outputs)
* Other low pin number options was MIPI DSI but this protocol is not publicly available and has a large learning curve
## New Block Diagram (v5)
This block diagram is updated with the new communications and display module ideas
![Full Block v5](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/Design%20Document/figures/Full_Block_Diagram_v5.png)

# 2/22/2022
## Design Document Check Presentation Notes
* Include reasoning for why RFID cards over QR code or other within the intro of the design as competing item.
* Include a table of contents
* Perform power consumption analysis

# Still Needed Items for Thursday
* Power Subsystem - Jack
* Card Reader Subsystem - Marco
* MCU Firmware Diagram
* App Firmware Diagram
* Schedule
* Cost Analysis
* Tolerance Analysis (RF equations for trace sizing)

## New Block Diagram (v6)
This block diagram is adjusted to consider that the power system is going to be designed from scratch so our current and voltage measurements can communicate directly from the battery protection block to the MCU (no additional current measurement needed)
![Full Block v6](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/Design%20Document/figures/Full_Block_Diagram_v6.png)

# 2/23/2022
## Current Requirement Analysis
* ESP32-WROVER-E = 500mA min required (600mA safe)
* Shift Reg = 3x0.52mA
* RFID reader = 150mA
* LCD = 40mA at 9.6V (210mA at 3.3V input with 70% efficient boost converter)
Total: 961.5mA at full operations

Suggested Battery: MIKROE-4474 (MLP805660)
* 3Ah capacity
* 0.5C continuous discharge (1.5A)
* 1C peak discharge (3A)
* 3.7V output

# 2/24/2022
## Design Document
* The design for the boost converter, buck converter, and voltage regulator are done by me. See the design document in the project files. See the data sheets in the resources for the reasoning behind component values. Additionally, see the notes [here](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/2-24-2022-DesignDocNotes.pdf) for some of the block sketches, design ideas, and equations used to design portions of the project and determine component values for circuit designs.

# 3/1/2022
## Design Review
* Updated version of the Design Document has been included. Major changes provided by me include the block diagram for the MCU firmware and a better parts list for the communications, display, and power subsystems. The current version of the Design Document is available [here](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/Design%20Document/OddsBoosterDesignDocument.pdf). The majority of the changes were made by my partners after they missed their requirements for the original design document deadline.

## PCB Review
The following notes were received at the board review
* Make sure to add PCBWay DRC check
* Remove component values from the silkscreen
* Increase the spacing between components and traces
* Make sure that data signals are at least 10 mil and power are at least 20 mil.
	* According to https://www.4pcb.com/trace-width-calculator.html a trace of 20 mil can safely support 1.2A which meets the requirements for the communications and display portion
	* The power portion may need to have larger traces, but that portion is not assigned to me.
* Make sure to add test points throughout the layout
* Upload gerber files to PCBWay for a method of auditing
The communications and display layout first draft I completed can be seen here.

![comms display layout draft 1](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/3-1-2022-CommsDisplayLayout.PNG)

# 3/7/2022
The comms and display layout was adjusted to meet the new trace width requirements and the PCBWay DRC. The new layout can be seen here. Significant work was done on the creation of footprints for the other portions of the design (again, not my part) which can be seen in the Eagle library called OddsBooster. I will try to take a step back and not do my partners assigned work and be a little more firm with that now.

![comms display layout draft 2](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/3-7-2022-CommsDisplayLayout.PNG)

# 3/12/2022
The full design of the PCB was completed. I was responsible for many of the footprints within the power section while my partners were responsible for much of the wiring and layout shown. Splitting up the specific pieces we each completed is not a good use of time. The finished PCB that is ordered is shown below.

![PCB_Ordered](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/PCB-layout-v2(ordered).PNG)

I also finalized the parts list to be ordered for the project. This full list is ordered through digikey and shown in the Design Document [here](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/Design%20Document/OddsBoosterDesignDocument.pdf). 

# 3/25/2022
While waiting for the parts and PCB to arrive, more considerations have been made to the development of the App. Each member of the team has an Apple mobile device but does not have an apple computer. This means we cannot develop applications for iOS. We have the software to develop Android apps but do not have any devices to run these Android apps. Reaching out to William Null, there are no available Android devices for us to use. Looking through our requirements and verification table, nothing specifies the app must run on a mobile device. With this in mind, we will move towards an application to run on a computer for showing the functionality of our project. In a real setting, it would be better to have a mobile app, but lack of supplies stopped us here.

We are hoping to look into using a C++ library for developing the GUI of the app called SFML. C++ is a coding language we are all familiar with and can be transfered to Android applications in the case of extending this prototype to a full product. The bluetooth portion of the code will require some baseline to allow for classic bluetooth connections.

# 3/29/2022
The individual progress report was completed today. All the information about my contributions to the project has been expressed. It does not need to be repeated here and instead can be found [here](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/ProgressReport/TimothyGreenProgressReport.pdf).

# 4/3/2022
ESP-IDF Visual Studio Code Getting Started
BLE GATT Server Example Tests
Kolban github repo failure due to not updated and so many unresolved items (github repo)
Windows Development App Bluetooth Visual Studio Program
Pairing Problems

# 4/5/2022
Classic Bluetooth Fallback
First with Arduino
SPP Acceptor Example Test
Attempt to Program Own Bluetooth App
Terminal App (github repo)

# 4/6/2022
Started soldering components (buck converter, boost converter, LDO)

# 4/9/2022
Testing the LDO, buck, and boost

# 4/12/2022
Successful communication between the app and the MCU on the devkit

# 4/14/2022
Soldering the MCU, MFRC, shift registers on the board
First test with the soldered board and the bluetooth code

# 4/15/2022
Cannot determine method to get both the 52MHz clock and the 6.5 MHz clock
Fallback on the 6.5MHz clock with tied input pins for 8 color options but full functionality

# 4/19/2022
Problems with the RGB support for the ESP32 chip being used so have to configure the i2s output of the board to work with the LCD (github repo)
This has been shown to work with the devkit but needs to be configured to meet our needs

# 4/21/2022
Problems with the loading of new programs onto the MCU
GPIO 12 needs a pull down resistor or the flash will be configured for a higher input power voltage within the module and will fail. (hasn't been a problem until now)


