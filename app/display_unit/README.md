# Requirements 
-  [Raspberry pi 3B] 
-  [Nokia 5110 display]
-  [USB to TTL]
-  [2024-07-04-raspios-bookworm-armhf-lite.img.xz Operating system]

Nokia 5110 Pins   |   Raspberry Pi GPIO Pins
--------------------------------------------
RST               ->  GPIO 17 (Pin 11)
CE/CS             ->  SPI CE0 (Pin 24)
DC                ->  GPIO 22 (Pin 15)
DIN (MOSI)        ->  SPI MOSI (Pin 19)
CLK (SCK)         ->  SPI SCLK (Pin 23)
VCC               ->  3.3V
BL (Backlight)    ->  Optional (3.3V)
GND               ->  GND

# Commands 
```
$ sudo dpkg -i libgpiod1_1.0.1-1~bpo9+1_armhf.deb libgpiod-dev_1.0.1-1~bpo9+1_armhf.deb
$ gcc nokia5110.c -o nokia5110 -lgpiod
$ ./nokia5110
```
