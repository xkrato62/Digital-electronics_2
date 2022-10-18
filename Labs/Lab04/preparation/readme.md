1. Use schematic of the LCD keypad shield and find out the connection of LCD display. What data and control signals are used? What is the meaning of these signals?


| **LCD signal(s)** | **AVR pin(s)**|**Description** |
| :-: | :-: | :-: |
| RS | PB0 | Register selection signal. Selection between Instruction register (RS=0) and Data register (RS=1)| 
| R/W | ground | | 
| E | PB1 | |
| D [3:0] | / |  | 
| D [7:4] | PD(7-4) |  |
| K | (Q1) - PB2 |  |
