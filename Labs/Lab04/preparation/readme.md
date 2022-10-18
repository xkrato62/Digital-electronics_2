1. Use schematic of the LCD keypad shield and find out the connection of LCD display. What data and control signals are used? What is the meaning of these signals?


| **LCD signal(s)** | **AVR pin(s)**|**Description** |
| :-: | :-: | :-: |
| RS | PB0 | Register selection signal. Selection between Instruction register (RS=0) and Data register (RS=1)| 
| R/W | ground | -128,..,127| 
| E | PB1 | 0,1,...,65 535|
| D [3:0] | / | -32 768,..,32 767 | 
| D [7:4] | PD(7-4) | 3.4e+38,...,3.4e+38 |
| K | (Q1) - PB2 | / |
