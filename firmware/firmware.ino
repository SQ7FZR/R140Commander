/* R140Commander @ SQ9MDD 2016
 * 
 * 
 * CHANGELOG 
 * 2016.08.21 - dodanie obsługi lcd po I2C: "sketch > include libraries > manage > TWILigudCrystal
 *         
 * 2016.08.04 - initialcode
 * 
 */
 
//************************************************************************************************//
// biblioteki i inicjalizacje
#include <TWILiquidCrystal.h>

uint8_t m_addr = 50;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(m_addr);

// zmienne i definicje
#define DEBUG

// definicje wejsc wyjsc
const int band_160m = 0;                                // fizyczne wyjscie dla pasm
const int band_80m = 0;
const int band_40m = 0;
const int band_30m = 0;
const int band_20m = 0;
const int band_17m = 0;
const int band_15m = 0;
const int band_12m = 0;
const int band_10m = 0;
const int amp_wentilator = 0;
const int amp_power = 0;
const int amp_hivoltage = 0;

// zmienne pomocnicze
int current_band = 1;                                   // aktualne pasmo
int current_state = 1;                                  // tryb pracy 

//************************************************************************************************//
// funkcje pomocnicze
void refresh_lcd(){
  
}

void change_band(){
  
}

//************************************************************************************************//
// odpalamy przy starcie
void setup(){
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  Wire.begin();
  lcd.begin(20, 4);
  lcd.println("R140 Commander");
}

// główny program
void loop(){
  
}

