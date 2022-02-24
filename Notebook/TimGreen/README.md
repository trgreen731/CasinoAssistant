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
[Full Block v5](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/Design%20Document/figures/Full_Block_Diagram_v5.png)

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
[Full Block v6](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/Design%20Document/figures/Full_Block_Diagram_v6.png)

# 2/23/2022
## Current Requirement Analysis

