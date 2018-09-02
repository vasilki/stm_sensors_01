# stm_sensors
----WATER SENSOR
Pinout:
PA0 (CN7.28) = S
+5V (CN7.18) = +
GND = GND

Water level is printed trough UART in case of change.


---------------


Using LED and UART on STM32 NUCLEO-F401RE.
UART is UART1, PINS PA9=TX,PA10=RX. PA9 on board is 21 PIN of CN10, PA10 is 33 PIN on CN10.
LED is LD2 on board, PA5 GPIO_output.
stm_sensors_01.ioc is a project STM32CubeMX.


Environment:
STM32 NUCLEO-F401RE
Linux Mint 18.2 Sonya
STM32Cube_1.0 version 4.24.0.Repository is STM32Cube_FW_F4_V1.19.0.
System Workbench for STM32 (Version: 1.13.2.201703061529) based on Eclipse IDE version Mars.2.Release 4.5.2