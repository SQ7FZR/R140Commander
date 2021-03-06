/* R140Commander @ SQ9MDD 2016
 * 
 * Obsługa przycisków funkcyjnych:
 * Dzielnik napiecia: +5V - 10K - A3 - 1K - 1K - 1K ... NK
 *                                |     |    |    |
 *                                P1    P2   P3   P4
 *                                |     |    |    |
 *                                GND   GND  GND  GND
 *                                
 * przycisk - wartość na przetworniku:
 * P1   -   0     
 * P2   -   93
 * P3   -   170
 * P4   -   237
 * 
 * CHANGELOG 
 * 2016.11.06 - v.1.40 dodanie obsługi przycisków (na razie tylko pomiar, flagi i obsługa On/Off wentylatora)
 * 2016.11.01 - zmiana pinow enkodera i jednego z pasm
 * 2016.10.31 - poprawki w sterowaniu przekaznikami, reset przekaznikow po czasie, kosmetyka lcd, czyszczenie kodu
 * 2016.10.29 - obsługa lcd i2c, sterowanie przekaznikami, encoder wersja beta dzialajaca
 * 2016.08.21 - dodanie obsługi lcd po I2C    
 * 2016.08.04 - initialcode
 * 
 * ROADMAP
 *  - obsługa automatycznej zmiany pasm po podłączeniu do TRX
 *  - obsługa rozruchu i wyłączenia wzmacniacza
 *  - obsługa sterowania wysokim napięciem
 *  + obsługa sterowania wentylatorem
 *  + obsługa zmiany pasm
 *  + obsuga LCD z I2C
 */ 
//************************************************************************************************//
// zmienne i definicje
#define sv_version  1.40                                // wersja softu
//#define DEBUG                                         // debugowanie skryptu
//#define ENCODER_DO_NOT_USE_INTERRUPTS                 // przelaczenie enkodera w tryb bez przerwań
//#define CIV_COMM                                      // sterowanie po protokole CIV

// biblioteki i inicjalizacje
#include <Wire.h>                                       // biblioteka i2c
#include <LiquidCrystal_PCF8574.h>                      // biblioteka lcd po i2c
#include <Encoder.h>                                    // biblioteka encodera

// konfiguracja
uint8_t m_addr = 0x3F;                                  // adres lcd, wykminione skanerem
const int pulses_for_groove = 2;                        // ilość impulsów na ząbek enkodera zmienić w zależności od posiadanego egzemplarza
const int set_band_interval = 3000;                     // ilość msec do zmiany pasma
const unsigned long resetting_relay_time = 15000;       // czas resetu przekaznika po zestrojeniu wzmacniacza

// definicje wejsc wyjsc
const int band_160m = 2;                                // wyjście D2 pasmo 1 fizyczne wyjscie dla pasm
const int band_80m = 3;                                 // wyjście D3 pasmo 2
const int band_40m = 4;                                 // wyjście D4 pasmo 3                                 
const int band_30m = 7;                                 // wyjście D7 pasmo 4
const int band_20m = 8;                                 // wyjście D8 pasmo 5
const int band_17m = 9;                                 // wyjście D9 pasmo 6
const int band_15m = 10;                                // wyjście D10 pasmo 7
const int band_12m = 11;                                // wyjście D11 pasmo 8
const int band_10m = 12;                                // wyjście D12 pasmo 9
const int amp_wentilator = A0;                          // sterowanie wentylatorem
const int amp_power = A1;                               // sterowanie zasilaniem wzmacniacza
const int amp_hivoltage = A2;                           // sterowanie wysokim napięciem
const int button_in =  A3;                              // wejscie dla klawiszy funkcyjnych
// A4 - LCD
// A5 - LCD

// zmienne pomocnicze
int current_band = 0;                                   // ustawiane pasmo
int current_set_band = 0;                               // aktualnie ustawione pasmo
int last_band = 0;                                      // poprzednie pasmo
int current_state = 0;                                  // tryb pracy 
int last_tmp = 0;                                       // zmienna pomocnicza do liczenia impulsów z enkodera 
int enc_sum = 0;                                        // zmienna pomocnicza do liczenia impulsów z enkodera
int reset_flag = 0;                                     // flaga wymuszania resetu przekaznikow
boolean amp_wentilator_flag = false;                    // flaga wentylatora
boolean amp_power_flag = false;                         // flaga zalaczenia zasilania wzmacniacza
boolean amp_hivoltage_flag = false;                     // flaga zalaczenia wysokiego napięcia
unsigned long time_to_set_band = 0;                     // 
unsigned long time_to_reset_relay = 0;                  //

//inicjalizacja
LiquidCrystal_PCF8574 lcd(m_addr);                      // inicjalizuje lcd 
Encoder myEnc(5,6);                                     // inicjalizuje enkoder

//************************************************************************************************//
// funkcje pomocnicze
void init_lcd(){                                        //
  lcd.setCursor(0, 0);                                  //
  lcd.print("WYBRANE PASMO:");                          //
  lcd.setCursor(16,0);                                  //
  lcd.print("----");                                    //
  lcd.setCursor(0,1);                                   //
  lcd.print("USTAW:");                                  //
  lcd.setCursor(16,1);                                  //
  lcd.print("----");                                    //
  lcd.setCursor(0,3);
  lcd.print("Wen OFF");
}

void ustaw_lcd(){
  lcd.setCursor(16,1);
  switch(current_band){
    case 0:
     lcd.print("----");
    break;
    case 1:
      lcd.print("160m");
    break;
    case 2:
      lcd.print(" 80m");
    break;
    case 3:
      lcd.print(" 40m");
    break;
    case 4:
      lcd.print(" 30m");
    break;
    case 5:
      lcd.print(" 20m");
    break;
    case 6:
      lcd.print(" 17m");
    break;
    case 7:
      lcd.print(" 15m");
    break;
    case 8:
      lcd.print(" 12m");
    break;
    case 9:
      lcd.print(" 10m");
    break;                        
  }  
}

void set_new_band(){
  lcd.setCursor(5,2);
  lcd.print("    OK    ");
  delay(500);  
  lcd.setCursor(16,0);
  switch(current_band){
    case 0:
      lcd.print("----");
      digitalWrite(band_160m,HIGH);
      digitalWrite(band_80m,HIGH);
      digitalWrite(band_40m,HIGH);
      digitalWrite(band_30m,HIGH);
      digitalWrite(band_20m,HIGH);
      digitalWrite(band_17m,HIGH);
      digitalWrite(band_15m,HIGH);
      digitalWrite(band_12m,HIGH);
      digitalWrite(band_10m,HIGH);
    break;    
    case 1:
      lcd.print("160m");
      digitalWrite(band_160m,LOW);
      digitalWrite(band_80m,HIGH);
      digitalWrite(band_40m,HIGH);
      digitalWrite(band_30m,HIGH);
      digitalWrite(band_20m,HIGH);
      digitalWrite(band_17m,HIGH);
      digitalWrite(band_15m,HIGH);
      digitalWrite(band_12m,HIGH);
      digitalWrite(band_10m,HIGH);
    break;
    case 2:
      lcd.print(" 80m");
      digitalWrite(band_160m,HIGH);
      digitalWrite(band_80m,LOW);
      digitalWrite(band_40m,HIGH);
      digitalWrite(band_30m,HIGH);
      digitalWrite(band_20m,HIGH);
      digitalWrite(band_17m,HIGH);
      digitalWrite(band_15m,HIGH);
      digitalWrite(band_12m,HIGH);
      digitalWrite(band_10m,HIGH);      
    break;
    case 3:
      lcd.print(" 40m");
      digitalWrite(band_160m,HIGH);
      digitalWrite(band_80m,HIGH);
      digitalWrite(band_40m,LOW);
      digitalWrite(band_30m,HIGH);
      digitalWrite(band_20m,HIGH);
      digitalWrite(band_17m,HIGH);
      digitalWrite(band_15m,HIGH);
      digitalWrite(band_12m,HIGH);
      digitalWrite(band_10m,HIGH);        
    break;
    case 4:
      lcd.print(" 30m");
      digitalWrite(band_160m,HIGH);
      digitalWrite(band_80m,HIGH);
      digitalWrite(band_40m,HIGH);
      digitalWrite(band_30m,LOW);
      digitalWrite(band_20m,HIGH);
      digitalWrite(band_17m,HIGH);
      digitalWrite(band_15m,HIGH);
      digitalWrite(band_12m,HIGH);
      digitalWrite(band_10m,HIGH);      
    break;
    case 5:
      lcd.print(" 20m");
      digitalWrite(band_160m,HIGH);
      digitalWrite(band_80m,HIGH);
      digitalWrite(band_40m,HIGH);
      digitalWrite(band_30m,HIGH);
      digitalWrite(band_20m,LOW);
      digitalWrite(band_17m,HIGH);
      digitalWrite(band_15m,HIGH);
      digitalWrite(band_12m,HIGH);
      digitalWrite(band_10m,HIGH);       
    break;
    case 6:
      lcd.print(" 17m");
      digitalWrite(band_160m,HIGH);
      digitalWrite(band_80m,HIGH);
      digitalWrite(band_40m,HIGH);
      digitalWrite(band_30m,HIGH);
      digitalWrite(band_20m,HIGH);
      digitalWrite(band_17m,LOW);
      digitalWrite(band_15m,HIGH);
      digitalWrite(band_12m,HIGH);
      digitalWrite(band_10m,HIGH);      
    break;
    case 7:
      lcd.print(" 15m");
      digitalWrite(band_160m,HIGH);
      digitalWrite(band_80m,HIGH);
      digitalWrite(band_40m,HIGH);
      digitalWrite(band_30m,HIGH);
      digitalWrite(band_20m,HIGH);
      digitalWrite(band_17m,HIGH);
      digitalWrite(band_15m,LOW);
      digitalWrite(band_12m,HIGH);
      digitalWrite(band_10m,HIGH);      
    break;
    case 8:
      lcd.print(" 12m");
      digitalWrite(band_160m,HIGH);
      digitalWrite(band_80m,HIGH);
      digitalWrite(band_40m,HIGH);
      digitalWrite(band_30m,HIGH);
      digitalWrite(band_20m,HIGH);
      digitalWrite(band_17m,HIGH);
      digitalWrite(band_15m,HIGH);
      digitalWrite(band_12m,LOW);
      digitalWrite(band_10m,HIGH);      
    break;
    case 9:
      lcd.print(" 10m");
      digitalWrite(band_160m,HIGH);
      digitalWrite(band_80m,HIGH);
      digitalWrite(band_40m,HIGH);
      digitalWrite(band_30m,HIGH);
      digitalWrite(band_20m,HIGH);
      digitalWrite(band_17m,HIGH);
      digitalWrite(band_15m,HIGH);
      digitalWrite(band_12m,HIGH);
      digitalWrite(band_10m,LOW);      
    break;                        
  }
  lcd.setCursor(5,2);
  lcd.print("STROJENIE ");
}

void encoder_go(){
  int tmp = myEnc.read();
  //int enc = 0;
  if (tmp != last_tmp){
    if (tmp < last_tmp){
      enc_sum--;
    }
    else if (tmp > last_tmp){
      enc_sum++;
    }
    last_tmp = tmp; 
    if(enc_sum >= pulses_for_groove && current_band < 9){
      current_band++;
      enc_sum = 0;                                              //reset zmiennej zliczającej impulsy enkodera
    }
    if(enc_sum <= -(pulses_for_groove) && current_band > 0){
      current_band--;
      enc_sum = 0;                                              //reset zmiennej zliczającej impulsy enkodera
    }  
    if(last_band != current_band){       
      ustaw_lcd(); 
      last_band = current_band; 
      time_to_set_band = millis() + set_band_interval;    
    }    
  }   
}

void reset_relay(){
  digitalWrite(band_160m,HIGH);
  digitalWrite(band_80m,HIGH);
  digitalWrite(band_40m,HIGH);
  digitalWrite(band_30m,HIGH);
  digitalWrite(band_20m,HIGH);
  digitalWrite(band_17m,HIGH);
  digitalWrite(band_15m,HIGH);
  digitalWrite(band_12m,HIGH);
  digitalWrite(band_10m,HIGH);    
  lcd.setCursor(5,2);
  lcd.print("  GOTOWY  ");
}

void set_zero(){
  digitalWrite(band_160m,HIGH);
  digitalWrite(band_80m,HIGH);
  digitalWrite(band_40m,HIGH);
  digitalWrite(band_30m,HIGH);
  digitalWrite(band_20m,HIGH);
  digitalWrite(band_17m,HIGH);
  digitalWrite(band_15m,HIGH);
  digitalWrite(band_12m,HIGH);
  digitalWrite(band_10m,HIGH);    
  lcd.setCursor(5,2);
  lcd.print("          ");  
  lcd.setCursor(16,0);
  lcd.print("----");
}

void read_buttons(){
  int buttons_val = analogRead(button_in);                              // czytamy co tam na przyciskach
  if(buttons_val < 10){                                                 // pierwszy klawisz całkiem na mase wentylator
    amp_wentilator_flag = !amp_wentilator_flag;                         // zmieniam flage stanu
    set_func();                                                         // wyzwalam zmianę 
    delay(250);                                                         // opozninie musi byc po nacisnieciu klawisza 
  }

}

void set_func(){
  if(amp_wentilator_flag){
    digitalWrite(amp_wentilator,LOW);
    lcd.setCursor(0,3);
    lcd.print("Wen ON ");
  }else{
    digitalWrite(amp_wentilator,HIGH);
    lcd.setCursor(0,3);
    lcd.print("Wen OFF");    
  }
}

//************************************************************************************************//
// odpalamy przy starcie
void setup(){
  pinMode(band_160m,OUTPUT);
  pinMode(band_80m,OUTPUT);
  pinMode(band_40m,OUTPUT);
  pinMode(band_30m,OUTPUT);
  pinMode(band_20m,OUTPUT);
  pinMode(band_17m,OUTPUT);
  pinMode(band_15m,OUTPUT);
  pinMode(band_12m,OUTPUT);
  pinMode(band_10m,OUTPUT);
  pinMode(button_in,INPUT);
  pinMode(amp_wentilator,OUTPUT);
  
  digitalWrite(band_160m,HIGH);
  digitalWrite(band_80m,HIGH);
  digitalWrite(band_40m,HIGH);
  digitalWrite(band_30m,HIGH);
  digitalWrite(band_20m,HIGH);
  digitalWrite(band_17m,HIGH);
  digitalWrite(band_15m,HIGH);
  digitalWrite(band_12m,HIGH);
  digitalWrite(band_10m,HIGH);
  digitalWrite(amp_wentilator,HIGH);
    
  Wire.begin();
  lcd.setBacklight(255);
  lcd.begin(20, 4);
  lcd.setCursor(3, 1);
  lcd.print("R140 Commander");
  lcd.setCursor(14,3);
  lcd.print("v.");
  lcd.print(sv_version);
  lcd.setCursor(0,3);
  delay(1000);
  lcd.print("booting...");
  delay(3000);
  lcd.clear();
  init_lcd();
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("debug mode");
  #endif
}

// główny program
void loop(){
  read_buttons();
  encoder_go();
  if(current_set_band != current_band && millis() >= time_to_set_band){
    if(current_band != 0){
      set_new_band();
      current_set_band = current_band;
      time_to_reset_relay = millis() + resetting_relay_time;
      reset_flag = 1;     
    }else{
      set_zero();
      current_set_band = current_band;  
      reset_flag = 0;   
    }
  }
  if(millis() >= time_to_reset_relay && reset_flag == 1){
    current_set_band = current_band;
    reset_relay();  
    reset_flag = 0;
  }
}
