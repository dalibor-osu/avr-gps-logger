# GPS-based environmental sensor data logger

### Členovia týmu

* Dalibor Dřevojánek
* Maxim Jehlička
* Frederic Michael Buberník
* Maroš Fandel 

## Teoretický popis 

Komplexný systém zaznamenávania údajov pomocou mikrokontroléra AVR. Systém integruje funkciu GPS na sledovanie polohy a senzor prostredia I2C na zachytávanie údajov súvisiacich s podmienkami prostredia. Cieľom projektu je zaznamenávať a zobrazovať údaje zo senzorov a poskytovať možnosť exportovania zozbieraných informácií na analýzu.

## Popis Hardwaru

### Komponenty
* Arduino Uno board
* OLED display - I2C OLED display 128x64
* GPS module - GPS module NEO-6M GYNEO6MV2
* DHT12 - I2C humidity and temperature sensor

![405266042_366938725840177_5990859187008306452_n](https://github.com/Bubo8521/digital_electronics-2/assets/124887713/6e1b9259-d891-40e4-b476-ed13594b3523)

## Popis Softwaru

Cez uart knižnicu riadime GPS modul a cez I2C(TWI) riadíme OLED displaj a senzor vlhkosti.

 Štruktúra projektu vyzerá nasledovne:

   ```c
   AVR-GPS-LOGGER      // PlatfomIO project
   ├── include         // Included file(s)
   │   └── timer.h
   ├── lib             // Libraries
   │   └── uart        // Peter Fleury's UART library
   │       ├── uart.c
   │       └── uart.h
   ├── src             // Source file(s)      
   │   └── main.c
   ├── test            // No need this
   └── platformio.ini  // Project Configuration File
   ```

## Používateľský návod

Write an instruction manual for your application, including photos and a link to a short app video.

## Referencie
* https://content.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_%28GPS.G6-HW-09005%29.pdf
 
* https://robototehnika.ru/file/DHT12.pdf
  
* https://www.vishay.com/docs/37902/oled128o064dbpp3n00000.pdf
