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

Cez knižnicu uart riadime GPS modul, ktorý cez tento protokol komunikuje. Ďalej využívame knižnicu TWI, ktorá obsahuje funkcie potrebné na ovládanie I2C komunikácie medzi senzorom na snímanie vlhkosti a teploty a arduínom. Treťou knižnicou je "oled" obsahujúca funkcie pre vypisovanie a komunikáciu s oled displejom. Na ovládanie časovačov využívame knižnicu "timer".

 Štruktúra projektu:

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

Celá logika programu je napísaná v súbore main.c. Ako prvé dôjde k inicializácii oled displeja, uart komunikácie a časovačov. Nekonečná slučka potom obsahuje niekoľko častí. Prvá z nich je výpis informácií na oled displej, kde môžeme vidieť aktuálny stav teploty a vlhkosti snímanej senzorom. K tomuto výpisu dôjde iba v momente, keď sú pripravené nové dáta. Následne sa kontroluje, či sa má obnoviť buffer pre GPS správu. Táto obnova spočíva v jednoduchom naplnení bufferu znakom '\0'. Následne dochádza ku kontrole aktuálneho riadku získaného z gps modulu. Všetky pre nás potrebné dáta sa nachádzajú na riadku s hlavičkou "GPGLL", preto dochádza ku kontrole tejto hlavičky a ak je odlišná, riadok je preskočený. V prípade, že sa jedná o nami chcený riadok, prevedú sa dáta zo stringu do pripravenej štruktúry zodpovedajúcej štruktúre dát na riadku GPS_data. Po prevedení dát sa tieto dáta vytlačia cez uart do pripojenej konzoly.

Program tiež obsahuje dve prerušenia spustené časovačmi. Prvé prerušenie spúšťa časovač TIM0 každú milisekundu a slúži na čítanie dát cez uart z gps modulu. Druhé prerušenie spúšťa časovač TIM1 a slúži na čítanie dát zo senzora snímajúceho teplotu a vlhkosť ovzdušia.

Logiku programu môžeme vidieť na nasledujúcom diagrame:

![de2_diagram](https://github.com/dalibor-osu/avr-gps-logger/assets/77931392/ad0bc3f7-5111-47fc-8da6-ba7eb121a62b)

## Používateľský návod

Po pripojení dosky k počítaču je možné čítať informácie o polohe, teplote a vlhkosti vzduchu v termináli, alebo ich pomocou programu Putty ukladať do textového súboru. Ak sa GPS modul nie je shopný pripojiť k satelitu, zobrazí sa správa GPS invalid a informácie o teplote a vlhkosti vzduchu budú stále vypísané.
 
## Referencie
* https://content.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_%28GPS.G6-HW-09005%29.pdf
 
* https://robototehnika.ru/file/DHT12.pdf
  
* https://www.vishay.com/docs/37902/oled128o064dbpp3n00000.pdf
