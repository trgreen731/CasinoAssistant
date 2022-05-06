# ECE 445: Senior Design Laboratory - Lab Notebook
### Name: Jack Arndt (jrarndt2)
### Team 37 - The Odds Booster

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
### [Project Proposal](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/Proposal/OddsBoosterProposal.pdf) due 11:59P.M.

# February 15th, 2022
### Begin Design of Circuits Schematics and Search for Necessary Components
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
### To-Do:
* Finish Design Document due Thursday, Feburary 24th, 2022 at 11:59P.M.
	* Power Subystem
	* MCU Firmware Diagram
	* App Firmware Diagram
	* Schedule
	* Cost Analysis
	* Tolerance Analysis  	
	
# February 23nd, 2022
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

# February 24nd, 2022
### [Design Document](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/DesignDocument/OddsBoosterDesignDocument.pdf) due 11:59P.M. 

## Design Discussion - Power Subystem
* The power subsystem consists of a Lithium-Ion battery (and battery protection circuitry capable of overcharge voltage, overdischarge voltage and overcurrent detection), a linear voltage regulator, a boost converter, a buck converter, and USB port for charging. Essentially, this subsystem is responsible for providing the necessary power to all other hardware subsystems in the overall system in a safe manner. **Reference Block Diagram (v6) above**

### Rechargable Lithium-Ion Battery
* As the primary source of power, the consensus decision was to use a **rechargeable Lithium-Ion battery**.
* This is typically a popular choice due to its tendency to provide adequate energy and power density in a relatively small package.
* From the datasheets provided for the ICs we will be using for the MCU ([ESP32- WROVER-E](https://github.com/trgreen731/OddsBooster/blob/master/Resources/Datasheets/ESP32_Bluetooth_Module/esp32-wrover-e_esp32-wrover-ie_datasheet_en.pdf)), RFID Reader ([NXP MRFC531](https://github.com/trgreen731/OddsBooster/blob/master/Resources/Datasheets/RFID_Components/MFRC531.pdf)), and the LCD Display ([Orient Display AFY320240A0-3.5N6NTN-R](https://github.com/trgreen731/OddsBooster/blob/master/Resources/Datasheets/LCD_Components/AFY320240A0-3.5N6NTN-R.pdf)), we find that each draw 500 [mA], 150 [mA], and 210 [mA] maximum current, respectively. Therefore, this leads to a total current draw of 960 [mA].
* Therefore, our selected Lithium-Ion battery ([Mikro- 4474](https://github.com/trgreen731/OddsBooster/blob/master/Resources/Datasheets/Power_Components/MLP805660%203000mAh%203.7V.pdf)) has a rated output of 1.5 [A] continuous within this range. 
* The output of the battery will further be connected to a battery protection IC (DW01-P), which has specifically been chosen to match the parameters of this battery cell. The battery protection circuitry serves the purpose to monitor the status of the battery through its output voltage and current draw. Therefore, the battery (and the external hardware subsystems circuitry) is protected from one another in the presence of over/undercharging and overdrawing by being able to cut power connections. **The schematic for the Battery Protection Circuitry is shown below.**

![BatteryProtection-Schematic.png](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/DesignDocument/figures/BatteryProtection-Schematic.png)
* As outlined in the IC's datasheet, the five modes of operation are as follows:
1. *Overcharge Protection*
When the voltage of the battery cell exceeds the overcharge protection voltage (V<sub>OCP</sub>) beyond the overcharge delay time (T<sub>OC</sub>) period, charging is inhibited by turning off of the charge control MOSFET. The overcharge condition is released in two cases:
	* The voltage of the battery cell becomes lower than the overcharge release voltage (V<sub>OCR</sub>) through self- discharge.
	* The voltage of the battery cell falls below the overcharge protection voltage (V<sub>OCP</sub>) and a load is connected.
When the battery voltage is above V<sub>OCP</sub>, the overcharge condition will not release even a load is connected to the pack.
2. *Overdischarge Protection*
When the voltage of the battery cell goes below the overdischarge protection voltage (V<sub>ODP</sub>) beyond the overdischarge delay time (T<sub>OD</sub>) period, discharging is inhibited by turning off the discharge control MOSFET. The default of overdischarge delay time is 10ms. Inhibition of discharging is immediately released when the voltage of the battery cell becomes higher than overdischarge release voltage (V<sub>ODR</sub>) through charging.
3. *Overcurrent Protection*
In normal mode, the DW01-P continuously monitors the discharge current by sensing the voltage of CS pin. If the voltage of CS pin exceeds the overcurrent protection voltage (V<sub>OIP</sub>) beyond the overcurrent delay time (T<sub>OI1</sub>) period, the overcurrent protection circuit operates and discharging is inhibited by turning off the discharge control MOSFET. The overcurrent condition returns to the normal mode when the load is released or the impedance between BATT+ and BATT- is larger than 500kΩ. The DW01-P provides two overcurrent detection levels (0.15V and 1.35V) with two overcurrent delay time (T<sub>OI1</sub> and T<sub>OI2</sub>) corresponding to each overcurrent detection level.
4. *Charge Detection after Overdischarge*
When overdischarge occurs, the discharge control MOSFET turns off and discharging is inhibited. However, charging is still permitted through the parasitic diode of MOSFET. Once the charger is connected to the battery pack, the DW01-P immediately turns on all the timing generation and detection circuitry. Charging progress is sensed if the voltage between CS and GND is below charge detection threshold voltage (V<sub>CH</sub>).
5. *Power-Down after Overdischarge*
When overdischarge occurs, the DW01-P will enter into power-down mode, turning off all the timing generation and detection circuitry to reduce the quiescent current to 0.1 [μA] (VCC=2.0V). At the same time, the CS pin is pull-up to VCC through an internal resistor.

### Linear Voltage Regualtor
* The voltage regulator is what connects the output of Power subsystem to the other three hardware subsystems via a 3.3V DC voltage signal used for CMOS logic powering.
* Our **linear voltage regulator** ([TI LP3965ESX-ADJ/NOPB](https://github.com/trgreen731/OddsBooster/blob/master/Resources/Datasheets/Power_Components/lp3962.pdf)) is a low-dropout voltage regulator used to convert the output voltage from the battery protection IC (input to the voltage regulator; 3.0V – 3.7V) to a 3.3V output.
	* However, if the input voltage is < 3.3V, the output follows it directly, which is an acceptable lower voltage range to all the components requiring this power bus. 
* This type of regulator is adequate for dropping small voltage levels (i.e. 3.7V to 3.3V) and has a much lower noise margin and EMI profile than switching converters.
* This is ideal for operation on a board with high frequency digital operations and RF traces. 
* Note that LDO voltage regulators end to have lower efficiencies than switching converters, so to counteract this, an existing adjustable LDO was implemented.
**The schematic for the LDO Linear Voltage Regulator is shown below.**
![LDO_Schem](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/DesignDocument/figures/LDO_Schem.PNG)
* The most important biasing consideration  are the feedback resistors. Following the schematic design as suggested by the datasheet, R1 = 10kΩ and R2 = R1*((V<sub>OUT</sub> / 1.216) - 1), which is approximately 17.14kΩ. 
	* This is not a value achievable by a single component, therefore two 33kΩ resistors are used in parallel along with a 2kΩ potentiometer to allow for fine tuning of the output voltage to the desired value.

### Boost and Buck Converters
* The **Boost Converter** ([ABLIC S-8337ABIA-T8T1U](https://github.com/trgreen731/OddsBooster/blob/master/Resources/Datasheets/Power_Components/S8337_8338_E.pdf)) is a switching converter that takes the output voltage from the battery protection IC (3.0V – 3.7V) and produces a 9.6V DC voltage output necessary to power the LED backlighting of the LCD display located in the display subsystem. **The Boost Converter schematic is shown below.**
![Power_Boost_Schem](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/DesignDocument/figures/Power_Boost_Schem.PNG)
* This boost converter requires a PWM (Pulse-Width Modulation) signal to control a MOSFET, which enables and disables current flow from the input to output. 
* The boost converter essentially functions by storing energy within the capacitor and inductor bank and transferring this energy in the form of power at a rate defined by defined by the duty cycle of the PWM signal to the MOSFET.
* A small inductor value for this circuit leads to higher stability, but greater current fluctuations. A lower inductor value increases the efficiency with less stability. Therefore, the chosen inductor rating is a midpoint of that suggested in the [datasheet](https://github.com/trgreen731/OddsBooster/blob/master/Resources/Datasheets/Power_Components/S8337_8338_E.pdf).
* The feedback biasing is also implemented to be adjustable. Here, R<sub>FB1</sub> is set to be 82kΩ and R<sub>FB2</sub>  is calculated to be 9.5kΩ implemented using a 8.2kΩ resistor and a 2kΩ potentiometer. 

* The **Buck Converter** ([Microchip MIC2169YMM](https://github.com/trgreen731/OddsBooster/blob/master/Resources/Datasheets/Power_Components/mic2169.pdf)) is a switching converter used to take the 5.0V USB voltage input and shift it down to the 4.2V charging voltage of the battery. **The Buck Converter Schematic is shown below.**
![Power_Buck_Schem](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/DesignDocument/figures/Power_Buck_Schem.PNG)
* A buck converter operates very similarly to a boost converter; however, in reverse. 
	* This subcircuit includes a second MOSFET for switching to avoid unintended floating effects and is also designed to be tuned using the included potentiometer.

### USB Charging Port
* The USB Micro-B Port will solely be used to charge the Lithium-Ion battery when necessary. 
* The 5V power supplied by the USB port will be down shifted to 4.2V through the implementation of the buck converter, which will be fed into the battery protection circuitry to recharge the battery. 

# March 1st, 2022
* Updated original Design Document sumbission
## Design Review
* Met with Professor Song for Design Review
	* Key takeaways: 
		* Given the complexity of the overall system, acheiving full integration and functionality may be difficult.
		* Consider fall-back plans if something doesn't work as intended
			* Potential Pitfalls:
				* Difficulties on obtaining Android device
				* Dealing with many high-frequency signals
				* Distribution of power	 
## PCB Board Review Notes
The following notes were received at the board review
* Make sure to add PCBWay DRC check
* Remove component values from the silkscreen
* Increase the spacing between components and traces
* Make sure that data signals are at least 10 mil and power are at least 20 mil.
	* According to https://www.4pcb.com/trace-width-calculator.html a trace of 20 mil can safely support 1.2A which meets the requirements for the communications and display portion
	* The power portion may need to have larger traces, but that portion is not assigned to me.
* Make sure to add test points throughout the layout
* Upload gerber files to PCBWay for a method of auditing
* **First Draft for Communications and Display Subsystems PCB Layout shown below.**

![comms display layout draft 1](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/3-1-2022-CommsDisplayLayout.PNG)

# March 7th, 2022
* Adjustments to PCB Layout were made to pass to PCBWay DRC. 
* Significant work done by team to create custom footprints of necessary components in EAGLE for PCB board layout
	* This can be found in the EAGLE library called [OddsBooster](https://github.com/trgreen731/OddsBooster/tree/master/ProjectFiles/EAGLE/ECE445_OddsBooster_Custom). 

# March 12th, 2022
* Full PCB design and layout completed. 
* I was responsible for a majority of the layout (i.e. Power subsystem and Card Reader subsystem), while Tim completed the rest.
* We initially ran into DRC issues as we did not run DRC using the provided file by PCBWay; however, these issues were quickly resolved once the errors were identified.
* Further, careful consideration was taken in the sizes of the traces, depending on the specific subsystem. 
	* For example, the Power subsystem was designed to deal with relatively high voltage and current ratings (i.e. ~3V-5V, ~1-2A), which would require thicker traces. 
* A trace size of 20 mil can safely support 1.2A, which meets the requirements for the Communications and Display subsystem specifications.
* I elected to use 10 mils for data signal traces and used 32 mils for power signals (i.e. VDD, etc.) to be well within compliance.
* The finished PCB layout is shown below.

![PCB_Ordered](https://github.com/trgreen731/OddsBooster/blob/master/Notebook/TimGreen/PCB-layout-v2(ordered).PNG)

# March 25th, 2022
* Waiting for PCB and components to arrive.
### App Develepment Considerations
* The two main candidates for mobile app development: Android and iOS. 
* While there exists a large user base for both platforms, each have posed their respective difficulties. 
* We had initially proposed to develop the application using the Android Studio IDE and Android SDK because of Android’s open-source nature and application development process promoting compatibility across most, if not all Android devices. 
	* However, we have been unable to obtain the necessary Android hardware to develop, test, and debug the software. 
* Apple has many strict guidelines and constraints in place regarding their development process for iOS-based applications that prevented us from adopting this approach. 
* After discussing, we ultimately decided that our best approach would be to emulate a phone-based application by writing the software in a computer program using C++. 
	* This originally served as our back-up plan if we encountered issues with the phone application development process, as outlined above. However, we will still be able to successfully interface with the MCU as almost all modern computers also have Bluetooth capabilities, so this approach is still well-within our defined requirements of the application. 
* C++ is our language of choice to write the application due to our familiarity with the programming language and its ability to potentially be integrated into an Android app. 

# March 30th, 2022
### Individual Progress Report (IPR) due 11:59P.M.

# April 6th, 2022
## PCB and Components have arrived
* Team met in ECEB 2070 (Senior Design Lab) to begin soldering components onto the PCB
* Tim and Marco have the most soldering experience in the group and soldered a large portion of the components onto the board
* As expected, the Buck Converter IC package and pads were extremely small. This could lead to potential issues down the line.

# April 9th, 2022
* Power subsystem components soldered onto PCB
* Tests were carried out according to the Requirements and Verifications for the Power subysystem, shown below.

### Requirements and Verifications for the Power Subsystem
* **REQUIREMENT**: When the voltage of the lithium ion battery cell exceeds the overcharge protection voltage of 4.25V (within a tolerance of ±0.05V), the battery protection circuit must be able to disconnect the connected components and inhibit charging by turning off the charge control MOSFET.
	* **VERIFICATION**: _To verify the correction functionality of the overcharge battery protection circuit, we can replace the lithium ion battery cell with a ramp DC voltage source, starting from the nominal 3.7V battery voltage and slowly “ramping up” the voltage by 0.1V increments to the rated 4.25V overcharge protection voltage. During this test, we can probe the gate (voltage measurement), drain and source (current measurement) terminals using an oscilloscope to confirm that the charge control MOSFET enters the cutoff region, as intended. Further, we must also probe the load of this circuit and take a resistance measurement between the two output terminals to confirm there is a very high impedance (i.e. the load is disconnected from the circuitry)._
* **REQUIREMENT**: When the voltage of the lithium ion battery cell falls below the overdischarge protection voltage of 2.40V (within a tolerance of ±0.05V), the battery protection circuit must be able to disconnect the connected components and inhibit discharging by turning off the discharge control MOSFET.
	* **VERIFICATION**: _To verify the correction functionality of the overdischarge battery protection circuit, we can replace the lithium ion battery cell with a ramp DC voltage source, starting from the nominal 3.7V battery voltage and slowly “ramping down” the voltage by 0.1V increments to the rated 2.40V overdischarge protection voltage. During this test, we can probe the gate (voltage measurement), drain and source (current measurement) terminals using an oscilloscope to confirm that the discharge control MOSFET enters the cutoff region, as intended. Further, we must also probe the load of this circuit and take a resistance measurement between the two output terminals to confirm there is a very high impedance (i.e. the load is disconnected from the circuitry)._
* **REQUIREMENT**: The voltage regulator circuit must be able to maintain a voltage within a safe operating range of 3.3V ± 0.1V (safe operating range for average CMOS device). If the input voltage falls below this range, the voltage regulator circuit must then match (follow) the input voltage signal to the LDO regulator (< 3.3V). Otherwise, the LDO regulator must output a constant voltage 3.3V. 
	* **VERIFICATION**:
		* Lithium-Ion battery directly connected to PCB as power source
			* Measurements showed battery voltage to be ~3.8V, as expected 
		* **Output of LDO showed reading of ~3.3V** (after fine-tuning the feedback resistors using the potentiometer)   
* **REQUIREMENT**: The boost converter circuit must step up the lithium ion battery voltage of 3.7V to a DC voltage of 9.6V (± 0.2V) as to provide enough power to the LED backlighting of the LCD display.
	* **VERIFICATION**:
		* Lithium-Ion battery directly connected to PCB as power source
			* Measurements showed battery voltage to be ~3.8V, as expected
		*  **Output of the Boost Converter showed reading of ~9.6V**
* **REQUIREMENT**: The buck converter circuit must step down the USB charging port DC voltage from 5V to 4.2V ± (0.1V). 
	* **VERIFICATION**:
		* 5V input voltage provided from power supply to USB input pins
		* **2mV output from Buck**
		* Potential Bugs:
			* Very small IC, pins could potential be bridged
			* PWM generation delivered to the gate signals of the MOSFET seem faulty
			* Incorrect duty cycle?	 

### Test Results
* LDO works 
* Boost Converter works
* Buck Converter does not work

# April 19th, 2022
## Phone App Update

### Development
* As stated previously on March 25th, we ultimately decided that our best approach would be to emulate a phone-based application by writing the software in a computer program using C++. 
* Blackjack chosen as our primary game implementation
	* In Blackjack, the user is prompted to take a definitive action (i.e. hit, stand, double), which greatly simplifies the overall experience for new users. Games such as Texas Hold’em poker will output the “strength” of the user’s hand in the form of percentage of “winning” a hand, which could cause initial confusion to players unfamiliar with such decisions. 
* The flow-chart algorithm shown below takes inspiration from a “Super-Easy Simplified Blackjack Basic Strategy Chart” that is commonly utilized by many novice Blackjack players, which is also shown below. 

![App_Flow](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/FinalReport/App_Flow.png)

![Super-Easy Simplified Blackjack Basic Strategy Chart](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/FinalReport/UsageDiagram/blackjack-super-easy-basic-strat.png)

* We chose to only implement one, very basic strategy to appeal to a larger pool of players. However, future considerations and improvement of the application suggest extending user customizability by allowing the players to select their own strategy they would like to employ. This would only require minor modifications to the existing code.

## Final Code
```
#include <iostream>

#define HIT 0
#define STAND 1
#define DOUBLE 2 
#define SPLIT 3  

/* NOTE: The strategy implemented in this program is a modified version of the "Super-Easy" Blackjack strategy accessed from http://blackjackcalculation.com/.
The getRecommendedPlayerAction function follows these guidelines:
    * [1] If you have a pair of Aces or 8's, split them. 
    * [2] Double down on 9 or 10 vs. a dealer low card (i.e. 2, 3, 4, 5 , 6) and always double down on 11. 
    * [3] Hit on soft 17 or less and stand on soft 18 or more. 
    * [4] Against a dealer low card (i.e. 2, 3, 4, 5, 6), "never bust" (i.e. stand on 12 or more). 
    * [5] Against a dealer high card (i.e. 7, 8, 9, 10, A), "mimic the dealer" (i.e. hit until you get at least 17). 
    */ 

/* Function: getRecommendedPlayerAction
Input(s): 
    * int *playercards: a pointer to an array of integer values of the player's cards, ranging from 1-10 (A = 1, J = Q = K = 10)
    * int numPlayerCards: number of cards a player has in his/her hand
    * int dealerCard: an integer representing the value of the dealer's up card
Output(s):
    * HIT, STAND, DOUBLE, SPLIT
*/

int getRecommendedPlayerAction(int *playerCards, int numPlayerCards, int dealerCard) { 
    
    int handValue = 0 // value of player's current hand
    int softValue = -1 

    for (int i = 0; i < numPlayerCards; i++) {
        handValue += playerCards[i];
        if (playerCards[i] == 1) { 
            softValue = 0;
        } 
    }

    if (softValue == 0) { 
        softValue = handValue + 10
    }

    // Split
    if ((playerCards[0] == playersCards[1]) && (numPlayerCards == 2)) {
        if ((playersCards[0]) == 1) || (playerCards[0] == 8)) 
        {
            return SPLIT; // [1]
        }
    }

    // Double Down
    if ((numPlayerCards == 2) && ((handValue >= 0) && (handValue <= 21))){
        if ((handValue == 9) && (dealerCard < 7) && (dealerCard != 1)) {
            return DOUBLE; // [2]
        }
        if ((handValue == 10) && (dealerCard < 7) && (dealerCard != 1)) {
            return DOUBLE; // [2]
        }
        if ((handValue == 11) {
            return DOUBLE; // [2]
        }
    }

    // Hit and Stand
    if ((handValue >= 17) || (softValue >= 18)) {
        return STAND;
    }
    else if (handValue <= 11) {
        return HIT;
    }
    else if ((dealerCard < 7) && (dealerCard != 1)) {
        return STAND;
    }
    else {
        return HIT;
    }
}
```
## Testing
![GUI_BlackjackHand](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/FinalReport/UsageDiagram/GUI_BlackjackHand.png)

# April 23rd, 2022
## Debugging before Demonstration on Monday, April 25th, 2022
### LCD Display
* The LCD Display backlights are not working
	* Tim originally concluded that this issue arose due to misinterpretation of the datasheet claiming that 9.6V necessary to power the LED backlights is in fact the forward voltage for a single LED device. This would require 28.8V applied at the LED+ pin of the LCD instead of 9.6V due to three LEDs being placed in series.
	* This did not seem like a feasible voltage only to power a small LCD display, which led us to believe there was another issue.
	* Ultimately, we discovered that the *VDD* and *GND* pins on the LCD connector were switched when laying out the PCB.
		* Our solution was to use insulating tape on these pads and jumper wires to make the correct connections.
		* In this process, our original LCD Display connector was destroyed and we had to resolder on a spare component.
		* Ultimately, we were able to sucessfully power the LCD Display backlights using a 9.6V input, as shown below.

![LCD_UnitTest](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/FinalReport/UsageDiagram/LCD_UnitTest.jpeg)

* When resoldering the LCD Display connector, we accidently shorted V<sub>CC</sub> to GND...
	* Although unintentional, we did confirm the functionality of the battery protection circuitry through this incident by observing the battery's output voltage drop to nearly 0V. 
* 3-way switch jumper cables were replaced.
	* The original connections between the switch and the necessary pins were unreliable and resulted in sporatic power loss. 
* Layout of MFRC uses a separate GND symbol/connection than the ground plane of the PCB.
	* Easily fixed using jumper wires  

# April 24th, 2022
### Final Demonstration TOMORROW, Monday, April 25th

### Notes/Talking Points for Demonstration:
* Removal of battery protection IC from original design
	* Instead, battery protection circuitry already came included with our selected Lithium-Ion battery
* Breifly mention about what ended up working and what did not and why
	* LDO and Boost Converter work
		* Show off tests used to verify requirements for Power subsystem (Successful) 
	* Buck Converter did not work (most likely from duty cycles on gate signals, or overlooked soldering issue)
* RFID tag identification using phone app
* MFRC Command Register interface (returning bf)
	* Mention oscillator not generating proper 13.56MHz clock
	* NCS pin maybe interfering with interface detection
	* Mention lack of documentation/common use of MFRC531
* Show all other code pertaining to MFRC and ESP32
* LCD backlight turning on & clock synchronization
	* Show switchup of pins on layout, and our troubleshooting of it
* Blackjack code working
* Explain jumper wires/insulated tape/pull-down resistor

# April 25th, 2022
### Final Demonstration TODAY 

# April 29th, 2022
### [Final Presentation Slides](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/OddsBoosterPresentationThemed.pptx) are completed. 

# May 2rd, 2022
### Final Presentation TODAY 

# May 3rd-4th, 2022
## May 3rd, 2022
* Responsible for some sections of the Power subsystem Design and Verification and App Subsystem Design and Verification in the Final Paper. 

## May 4th, 2022
### [Final Paper](https://github.com/trgreen731/OddsBooster/blob/master/ProjectFiles/FinalReport/OddsBoosterFinalReport.pdf) due 11:50P.M.








	








