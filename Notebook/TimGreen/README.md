## 2/8/2022
### Proposal Brainstorming: Module Components and Requirements
* RFID Sensor:
	* specify voltage requirements for the sensor
	* specific output signal (different for each of 52 cards/tags)
	* specify frequency of RF sensor
		* 910 MHz (center frequency for digikey tags)
* Processor:
	* handle RFID sensor signal input
	* create a bluetooth packet holding card ID and output to transceiver
	* handle incoming bluetooth packets and respond accordingly
	* specify voltage requirement for power
* Battery:
	* Battery Cell Type and thus output voltage + current
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
	* Utilize bluetooth capabilities of the phone (successfully send and receive packets)
	* Store information about current game
	* Logic/State Machine for game actions
	* Logic for best move decision process
