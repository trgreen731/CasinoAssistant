# ECE 445: Senior Design Laboratory Lab Notebook - Jack Arndt (jrarndt2)
# February 8th, 2022
## Initial Meeting with TA
### Notes from meeting
* Add additional hardware components to increase project complexity
 * Consider replacing original proposed DC power supply adapter with a rechargable battery (Lithium-Ion?)
	* This would require additional circuitry to monitor the battery's state and regulate voltage output to other subsystems
* ESD protection?
* Add an LCD display to display additional information about the game (e.g. dealer instructions, battery levels, etc.)
* Keep working on Project Proposal document
* Start to think about lower-level requirements necessary for each subsystem
### Brainstorming
* Card identification?
 * Light sensors
 * QR codes
 * RFID tags
### High-Level Requirements
* The RFID Sensor must be able to sense and distinguish the 52 different RFID tags associated with each card and provide the card ID signal to the microprocessor
* The microprocessor must be able to receive a card ID value and send this value to the iOS App via bluetooth packet.
* The iOS App must be able to receive card ID info via a bluetooth packet and use the currently played cards to determine an optimal move for the player.
### Additional thoughts to consider
* Progamming langauge to construct phone app?
* Texas Hold'em Poker and Blackjack mathematical analysis to start thinking about psuedo-code for the app
* Define what information will be displayed/hidden to the user
# February 10th, 2022
## Project Proposal due 11:59P.M.
Finished [Project Proposal Document](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/Proposal/OddsBoosterProposal.pdf) found in "ProjectFiles"folder  
# February 15th, 2022
## Begin Design of Circuits Schematics and Search for Necessary Components
* Responsible for Power Subsystem
* All parts are found in the [Resources Folder](https://github.com/trgreen731/OddsBooster/tree/master/Resources). 
# February 18th, 2022 
## Meeting with TA
* Weekly TA meeting time switched from Tuesday to Friday moving forward
### Notes from meeting
* Check overall system power requirements (i.e. power consumption of each subsystem)
 * This will help in deciding on what battery to select to match the desired specifications
 * Additional circuitry to regulate output battery voltage and current
* Designed vs. "Off-the-Shelf" components
 * Suggested by TA to design Boost converter versus using an "off-the-shelf" component to increase complexity of the design
 * However, some components such as the RFID Reader IC and Bluetooth IC already exist and are allowed to be purchased separately
* Try to select component packages with hand-solderable pads, otherwise can lead to many bugs later during construction of PCB
### To-Do:
* Finish selecting components for Power subsystem
* Work on Design Document due Thursday, Feburary 24th, 2022 at 11:59P.M.
# February 22nd, 2022
## Updated Block Diagram (v6)
![Full Block v6](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/DesignDocument/figures/Full_Block_Diagram_v6.png)
### Changes reflected in this updated block diagram
* Power subsystem components "black-boxed" from Version 5 of the block diagram 
 * Current and voltage measurement outputs of the battery protection circuitry can directly interface with the MCU, thus eliminating the need for additional current and voltage measurements




