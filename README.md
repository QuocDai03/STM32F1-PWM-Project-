- My source code is used with a model provided by my professor in my uni.

- IDE and compliler used: Keil C

- The schematic of this model showing in this link: 
https://drive.google.com/file/d/10fvUAA3HmId-gqAXSxXMBl2QNpnc4bxN/view?usp=sharing 

Here is what my source code do:
+ Creating PWM with frequency 10kHz
+ Percent of PWM is sent from PC.
+ This PWM used to adjust the level of brightness of a LED with the percent of PWM. The percent is changed from 0% to 100% means the LED is changed from being Off to be the highest level of brightness. 
+ The percent of PWM is displayed on module LED 7-segment 5 digits.

Details about hardware:
+ The module LED 7-segment 5 digits is used with IC74HC594.
++ Pin PB3 is wired to SCK(clock) of IC74HC594
++ Pin PE0 is wired to RCK(Lath) of IC74HC594
++ Pin PB5 is wired to SER(Data) of IC74HC594
++ Pin PE2 is wired to RCL of IC74HC594

+ Pin PB10 has function of adjusting the level of brightness of LED.

+ The USART1 is used: TX: pin PA9; RX: pin PA10.
++ The software used to send data from PC to microcontroller is "Advance Serial Port Terminal 5.5 by Eltima Software".
