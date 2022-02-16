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
Proposal work all written in proposal file in project files. The finished proposal will be included within this notebook.

# 2/15/2022
## Circuit Design and Component Search
All found parts will be in the resources folder along with key points of the part
