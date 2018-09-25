# stm_sensors

Using temperature sensor (DS18B20), LED and UART on STM32 NUCLEO-F401RE.
UART is UART1, PINS PA9=TX,PA10=RX. PA9 on board is 21 PIN of CN10, PA10 is 33 PIN on CN10.
LED is LD2 on board, PA5 GPIO_output.
DS18B20 signal-pin is on PB5 GPIO_output/input.

Be carefull. If you have 37 in 1 sensors' kit where DS18B20 is presented on board "-" should be connected to GND,
"S" should be connected to "signal pin", third pin should be connected to "+5V".
Check here: http://wiki.sainsmart.com/index.php/37_in_1_Sensor_Module_Kit



Environment:
STM32 NUCLEO-F401RE
Linux Mint 19 Tara 64bit
STM32Cube_1.0 version 4.27.0.Repository is STM32Cube_FW_F4_V1.21.0.
System Workbench for STM32 (Version: 1.16.0.201807130628) based on Eclipse IDE version Neon.3.Release 4.6.3