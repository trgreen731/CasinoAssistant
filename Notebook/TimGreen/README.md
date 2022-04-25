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
![Full Block v5](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/DesignDocument/figures/Full_Block_Diagram_v5.png)

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
![Full Block v6](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/DesignDocument/figures/Full_Block_Diagram_v6.png)

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
* Updated version of the Design Document has been included. Major changes provided by me include the block diagram for the MCU firmware and a better parts list for the communications, display, and power subsystems. The current version of the Design Document is available [here](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/DesignDocument/OddsBoosterDesignDocument.pdf). The majority of the changes were made by my partners after they missed their requirements for the original design document deadline.

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

We are hoping to look into using a C++ library for developing the GUI of the app called SFML. C++ is a coding language we are all familiar with and can be transfered to Android applications in the case of extending this prototype to a full product. The bluetooth portion of the code will require some baseline to allow for classic bluetooth connections. The app will run with BlackJack games primarily as this has a stronger suggestion capability than any poker game which would require a sort of learning method to produce successful suggestions.

# 3/29/2022
The individual progress report was completed today. All the information about my contributions to the project has been expressed. It does not need to be repeated here and instead can be found [here](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/ProgressReport/TimothyGreenProgressReport.pdf).

# 4/3/2022

* The ESP32 MCU chosen has a set of API functions (ESP-IDF) that can be loaded into Visual Studio Code and used to make the development process possible. The download instructions are found here https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/vscode-setup.html.
* The first step in working with the code is to use the devkit to attempt to create a bluetooth connection. From some research, a BLE GATT server seems to be a good option
	* The BLE GATT server has characteristics with corresponding values which can be updated by the server and retreived by the client
	* The following esp produced example code is used to get started with the development of a gatt server https://github.com/espressif/esp-idf/tree/97fb98a91b308d4f4db54d6dd1644117607e9692/examples/bluetooth/bluedroid/ble/gatt_server
* The Bluetooth pairing partner is code run on the laptop from a windows development app created by windows https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BluetoothLE
	* This pairing has the problem where the server on the MCU seems to think the pairing is successful but the windows app shows the bluetooth device but says "could not connect. The MCU output is shown below with error code 0x13 being a timeout. ![MCU_GATT_output](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4409.jpg)
	* The normal Bluetooth searching capabilities on the laptop is not able to see the MCU either when the GAP advertising is running.
* Too attempt to fix this issue and make the coding slightly easier, a developed bluetooth low energy method is investigated. https://github.com/nkolban/esp32-snippets/tree/master/cpp_utils
	* This method did not lead to any good developments. The code would not compile due to depricated includes and references.

# 4/5/2022
* In efforts to find an example that would properly pair, the classic bluetooth options where discovered. The first example is done using the Arduino IDE based off the following tutorial https://randomnerdtutorials.com/esp32-bluetooth-classic-arduino-ide/.
* This successful development led to the discovery of the SPP (serial port pairing) Accepter testing using Classic Bluetooth. The first example used is shown with the following esp-idf example. Acceptor must be used because this allows the MCU to wait for a connection from the laptop or app and accept the connection then communicate serially. https://github.com/espressif/esp-idf/tree/97fb98a91b308d4f4db54d6dd1644117607e9692/examples/bluetooth/bluedroid/classic_bt/bt_spp_acceptor
* The Serial Communication was first attempted using windows socket api and serial communication methods. This was a slow process and did not lead to anything fulfilling.
* The following terminal app establishes a RFCOMM connection and communicates over a serial port. I have discovered now that this is a common way to communicate through bluetooth. The github repo is shown below. This terminal will be used to initialize the bluetooth connection and contain text based output that could be used to communicate information to the user for the app. https://github.com/NSTerminal/terminal. This code uses the MIT license.
* The first successful connection where information could be shared was done with this method. The example is shown in the following image. ![MCU_SPP_Output](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4449.jpg)

# 4/6/2022
I soldered the components onto the board for the buck converter, the boost converter, and the LDO. I did this soldering because my partners did not feel as confident with soldering some of the small components. An intermediate image of the buck converter size is shown with finger for scale. ![buck_soldering](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/Buck_Soldering.jpg)

# 4/9/2022
The following tests were run today now that the power system soldering was completed. The orginal test notes are [here](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/4-12-2022-TestResults.txt).
* 5V input provided from the power source to the USB input pins
	* 2mV signal measured at the output of the buck
	* PWM generation delivered to the gate signals of the MOSFETs seems faulty
	* The duty cycle might be incorrect for some reason
	* The buck is also tiny so there might be a problem with bridging pins on the device
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4444.jpg)
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4445.jpg)
* The battery is connected to the battery terminal. The battery and input pins are connected simulating switching the device on. The LDO output voltage (Vcc) is recorded at various spots on the board.
	* The battery voltage at the switch is 3.8V as expected
	* The LDO output is recorded and adjusted using the potentiometer (varies between 3.28V to 3.36V)
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4436.jpg)
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4440.jpg)
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4438.jpg)
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4439.jpg)
* The battery is connected to the battery terminal. The battery and input pins are connected simulating switching the device on. The Boost converter output voltage (Vdd) is recorded at the main output (only connected at one place).
	* The buck output is recorded and adjusted using the potentiometer (varies between 9.5V to 9.68V).
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4441.jpg)
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4442.jpg)

# 4/12/2022
* The MCU was soldered onto the board along with the FPC connector. Partners completed all card system RLC components.
* The bluetooth code running on the devkit was tested on the soldered board for the first time. The following operation notes are recorded:
	* The file can be successfully loaded to flash
	* When providing Vcc with the UART pins, BT Init fails because the USB to serial board for programming does not have a strong enough LDO to provide the needed RF communication init current.
	* Disconnecting Vcc from UART (keep common ground) and using the on-board LDO leads to successful operation
	* Load through UART by set boot switch to 0, press flash on VSCode ESP-IDF commands, press reset button on board
	* Load from flash memory by set boot switch to 1 then press reset button
	* Bluetooth Pairing is successful with existing code
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4447.jpg)
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4450.jpg)

# 4/14/2022
* Soldering the MFRC chip onto the board
* The GPIO functionality and interrupt capabilities of the MFRC are important. This functionality is tested by simulating an interrupt on the MCU IRQ pin and a pseudo card value is read at that time. Touching the IRQ pin to Vcc successfully triggers the interrupt operation
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4451.jpg)
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4454.jpg)

# 4/15/2022
* Clock outputs on the MCU are not easy to program. The MCU has RGB parallel display support through the esp examples. https://github.com/espressif/esp-idf/tree/master/examples/peripherals/lcd/rgb_panel.
	* 52 MHz clock corresponds to a period of 19ns for the data signals assuming no setup or tear down time. The LCD requires a setup time of 12ns and a hold time of 12ns. The combination of this is longer than the perfect period of the data meaning it won't be properyly transmitted to the LCD.
	* Setting the clock output using the SPI LCD control example shows that this clock must be a division of the 80MHz master internal clock. The devision factor of this clock cannot be a decimal as stated in the datasheet suggesting 52MHz clock cannot be generated for external use anyways.
	* Easy fix to the problem is use the 6.5MHz clock output as expected with the 3 bits of data pins at this rate. The 8 input color pins can be tied together to get a lower color depth but still acheive functionlity.

# 4/19/2022
* Unfortunately the RGB code is only accessible for ESP32-S3 chips which is not compatible with our MCU. The same problems occur when trying to fix this as the old bluetooth work, the includes and references do not trace and the support is not provided. I have tried for very long to shift the support but I cannot get it to work.
* To fix this, I have found alternate examples of people using a parallel setup of a I2S interface (normally used for audio) to control a display. The baseline code for this is found here. https://github.com/har-in-air/ESP32-LCD-I2S
* This code can be compiled on the devkit but needs to be configured to our purposes.

# 4/21/2022
* For the first time, the UART loading of the MCU is not working. The flash method checks the download of the code using an md5 value of the sent file and the file on the flash memory. These are not matching. The file on the flash has the same md5 file each time indicating write permissions not granted.
* GPIO 12 needs a pull down resistor during startup or the flash will be configured for a higher input power voltage within the module and will fail. This is fixed by attaching a 560 Ohm resistor between the pin and ground.
* The loading of the bluetooth files work again.

# 4/22/2022
The following functions are tested and the videos are kept with partners.
* Confirmed the operation of the Bluetooth communication (video)
* Confirmed operation of the LDO (video)
* Confirmed operation of the Boost (video)

Work towards functioning LCD
* Utilize the parallel i2s interface with tied color pins to change to monochrome (this simplifies the parallel i2s interface as well)
	* the red and green output pins of the MCU aren't able to be configured to output pins (input pins only) which is strange for a pin labeled GPIO
* couldn't handle the high clock frequency at the MCU (divide from 80MHz and no decimal so no 52MHz, no synchronization method with a secondary clock, only one clock off the 80MHz master others must be off lower frequency master)
* Additionally the required setup and hold time on the LCD longer than the period if were to use shift registers and 52MHz clock

Adjusting this code to work with the LCD we have:
* Reconfigure DMA to store single pixel value in each byte instead of 4 pixel values
* Adjust sizing and add space for the front and back porches
* Change the clear screen to set data to 0 on porches, control the h and v syncs, and control the de
* Able to get screen flash between black and "white" but the white is dim (maybe wrong backlight voltage)

The following videos showcase the flashing screen along with the clock, vsync, and hysnc.
![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4490.mov)
![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4492.mov)
![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4493.MOV)
![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4494.MOV)

# 4/23/2022
* The LCD backlight does not work due to a misinterpretation of the data sheet on my part. The 9.6V forward voltage referred to a single LED not to the voltage needed at the LED+ pin of the LCD as I thought. The true voltage needed at this pin is 28.8V because three of these LEDs are placed in series.
* This is actually not accurate, I accidentally swapped the Vdd and GND planes on the connector that caused the backlight to not function (the voltage was right)
	* Fixed this using insulating tape on the pads and jumper wires
	* Had to swap the connector with extra and broke original in process
	* Now the data is not transmitted properly. Probing the spots shows it has the right signals but doesn't seem to receive them.
	* ![](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/TestEvidence/IMG-4490.mov)

Attempting to combine the bluetooth and LCD functionality into a single program that can be loaded onto the MCU. The RAM needed for the allocation of the program exceeds the available size. The board has a PSRAM chip not used for program files but available for dynamic allocation. Perhaps finding a way to use this and dynamically allocate more large data structures will fix this. Methods used to get around this problem:
* Dynamically allocate the bluetooth stack at runtime instead of statically allocating it
* Dynamically allocate the frame buffer at runtime instead of statically allocating it (no automatic way of freeing this which is worrying)
* These steps fixed exceed size problem but caused stack overflow on the CPU 1 with the LCD task running on it (where the frame buffer malloc occurred but malloc should go to heap not stack)
	* We can place the .bss in the external PSRAM through the sdkconfig ESP32-specifc SPIRAM settings and not dynamically allocate the frame buffer if notice strange behavior
	* Can also try to allocate WiFi and LWIP in SPIRAM since neither used but might be taking up internal memory

Today's problems:
* VDD and GND swapped on LCD
* shorting vcc when resoldered
* unreliable switch connections means random loss of power
* connected uart pins while powering leads vdd to drop to 3.9V
* backlight with reworking of the vdd pins works but now the data signals are getting lost along the way (don't have another connector to redo it again)
* the synch signals from the mcu still work but don't seem to be controlling the lcd properly as they once did
* running the bluetooth and lcd code at same time leads to overflow of dram from the frame buffer allocation so need to dynamically allocate it at runtime (still problems because its data and psram isn't dma accessible) (will just run them modularly and can do mfrc and bluetooth together then lcd on its own if needed)
* mfrc layout has separate ground signal from the ground plane due to different symbols in schematic and separate building of the layout (fix with simple jumper wire)

Today's outcomes:
* LDO works (most times)
* Boost works (most times)
* Buck doesn't work (duty cycle regulation is my theory or small soldering problems)
* Bluetooth from MCU works (sending is reliable but not up to speed requirements)
* LCD clock output works (only 6.5MHz not 52 MHz due to MCU and LCD limitations)
* Writing to the buffer while operating works (show the no backlight test)
* Output data for LCD can be changed fast enough (fill the buffer with vertical stripes maybe)
* Interface with MFRC works (doesn't get out of startup phase)
* ID stored on RFID tags works
* Reading with the MFRC (in progress)
* App communication through bluetooth works
* App calculating best move (in progress)
* App displaying this move to the user in text (in progress)

To Do:
* Vertical Stripe LCD coding (maybe)
* MFRC interface (start with jump wire to the grounds)
* App Calculation (just get the output in text format)
* Polish up notebook a little bit
* Consolidate the testing evidence
* Print out the block diagram, HL requirements, and RV points
* Review technical information and plan out tests to do

# 4/24/2022
* Vertical Stripe Coding shows the frequency of the data transfer but is not shown on the LCD
* MFRC interface is continually saying CMD reg is 0xbf
	* This tells us that the chip reads the address properly and sets the data pins for the read
	* The analog power pin might need 5V (didn't fix it)
	* The oscillator circuit might be not functional (replaced with waveform didn't fix it)
	* The value indicates the chip is in startup and cannot properly detect the interface of the device
	* Final hypothesis for lack of functionality is the improper oscillation setup since it needs stable operation before startup occurs
* App functionality finished with player decision GUI for blackjack built into terminal app as discussed before

# 4/25/2022
Demo Today:
