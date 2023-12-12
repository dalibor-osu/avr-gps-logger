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

Block diagram:
![bloková schéma](https://github.com/dalibor-osu/avr-gps-logger/assets/124789239/aecf42dd-40f4-42b7-9335-89453f5ede8b)

Circuit diagram:
![shcema](https://github.com/dalibor-osu/avr-gps-logger/assets/124789239/10efadbf-f61a-4e46-8765-3eac135d7ddb)

Reálne zapojenie:
* Prvá varianta:
![405266042_366938725840177_5990859187008306452_n](https://github.com/Bubo8521/digital_electronics-2/assets/124887713/6e1b9259-d891-40e4-b476-ed13594b3523)

![370153749_786611976812593_4385338365502907530_n](https://github.com/dalibor-osu/avr-gps-logger/assets/124789239/f20758bb-d2ec-438a-bbb1-ed6dc5454a18)

* Druhá varianta:
![1](https://github.com/dalibor-osu/avr-gps-logger/assets/124789239/172508a6-c2c8-4e4d-a066-e0fd3001a7e8)

![2](https://github.com/dalibor-osu/avr-gps-logger/assets/124789239/b3f3c8ab-14ad-4d99-8db4-f4bfcdb9ae16)

![3](https://github.com/dalibor-osu/avr-gps-logger/assets/124789239/8035de33-d2a5-407f-83c7-5f127132864c)

## Popis Softwaru

Cez uart knižnicu riadime GPS modul a cez I2C(TWI) riadíme OLED displaj a senzor vlhkosti.

Popis gpio, oled, main!!!

 Štruktúra projektu vyzerá nasledovne:

   ```c
   AVR-GPS-LOGGER      // PlatfomIO project
   ├── include         // Included file(s)
   │   └── timer.h
   ├── lib             // Libraries
   │   └── gpio
   │   │    ├── gpio.c
   │   │    └── gpio.h
   │   └── oled
   │   │    ├── oled.c
   │   │    ├── oled.h
   │   │    └── font.h
   │   └── twi
   │   │    ├── twi.c 
   │   │    └── twi.h
   │   └── uart        // Peter Fleury's UART library
   │       ├── uart.c
   │       └── uart.h
   ├── src             // Source file(s)      
   │   └── main.c
   ├── test            // No need this
   └── platformio.ini  // Project Configuration File
   ```

## Používateľský návod

Po připojení desky k počítači je možné číst informace o poloze, teplotě a vlhkosti vzduchu v terminálu, nebo je pomocí programu Putty ukádat do textového souboru. Pokud se modul GPS není shopný připojit k satelitu zobrazí se zpráva GPS invalid a infomace o teplotě a vlhkosti vzduchu budou stále vypsnány. 
## Referencie
* https://content.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_%28GPS.G6-HW-09005%29.pdf
 
* https://robototehnika.ru/file/DHT12.pdf
  
* https://www.vishay.com/docs/37902/oled128o064dbpp3n00000.pdf
