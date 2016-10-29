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
// zmienne i definicje
#define DEBUG
#define ENCODER_DO_NOT_USE_INTERRUPTS

// biblioteki i inicjalizacje
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <Encoder.h>

uint8_t m_addr = 0x3F;

// initialize the library with the numbers of the interface pins
LiquidCrystal_PCF8574 lcd(m_addr);
Encoder myEnc(2,3);

// konfiguracja
const int pulses_for_groove = 2;                        //ilość impulsów na ząbek enkodera zmienić w zależności od posiadanego egzemplarza
const int set_band_interval = 3000;                     //ilość msec do zmiany pasma

// definicje wejsc wyjsc
const int band_160m = 4;                                //1 fizyczne wyjscie dla pasm
const int band_80m = 5;                                 //2
const int band_40m = 6;                                 //3                                 
const int band_30m = 7;                                 //4
const int band_20m = 8;                                 //5
const int band_17m = 9;                                 //6
const int band_15m = 10;                                //7
const int band_12m = 11;                                //8
const int band_10m = 12;                                //9
const int amp_wentilator = 0;
const int amp_power = 0;
const int amp_hivoltage = 0;

// zmienne pomocnicze
int current_band = 1;                                   // ustawiane pasmo
int current_set_band = 1;                               // aktualnie ustawione pasmo
int last_band = 1;
int current_state = 1;                                  // tryb pracy 
int last_tmp = 0;                                        //zmienna pomocnicza do liczenia impulsów z enkodera 
int enc_sum = 0;                                        //zmienna pomocnicza do liczenia impulsów z enkodera
unsigned long time_to_set_band = 0;                     //
//************************************************************************************************//
// funkcje pomocnicze
void init_lcd(){
  lcd.setCursor(0, 0);
  lcd.print("WYBRANE PASMO:");
  lcd.setCursor(15,0); 
  lcd.print("160m");
  lcd.setCursor(0,1);
  lcd.print("USTAW:");
  lcd.setCursor(15,1);
  lcd.print("160m");
}

void ustaw_lcd(){
  lcd.setCursor(15,1);
  switch(current_band){
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
  lcd.setCursor(15,0);
  switch(current_band){
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
  lcd.setCursor(9,2);
  lcd.print("OK");
  delay(1000);
  //lcd.setCursor(15,1);
  //lcd.print("    ");
  lcd.setCursor(9,2);
  lcd.print("  ");
}

void encoder_go(){
  int tmp = myEnc.read();
  //int enc = 0;
  if (tmp != last_tmp) {
    if (tmp < last_tmp){
      enc_sum--;
    }
    else if (tmp > last_tmp){
      enc_sum++;
    }
    last_tmp = tmp; 
    
    if(enc_sum >= pulses_for_groove && current_band < 9){
      current_band++;
      enc_sum = 0;                          //reset zmiennej zliczającej impulsy enkodera
    }
    if(enc_sum <= -(pulses_for_groove) && current_band > 1){
      current_band--;
      enc_sum = 0;                          //reset zmiennej zliczającej impulsy enkodera
    }  
    if(last_band != current_band){       
       ustaw_lcd(); 
       last_band = current_band; 
       time_to_set_band = millis() + set_band_interval;    
    }    
    delayMicroseconds(10);                   //małe opóźnienie dla prawidłowego działania enkodera
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
      digitalWrite(band_160m,HIGH);
      digitalWrite(band_80m,HIGH);
      digitalWrite(band_40m,HIGH);
      digitalWrite(band_30m,HIGH);
      digitalWrite(band_20m,HIGH);
      digitalWrite(band_17m,HIGH);
      digitalWrite(band_15m,HIGH);
      digitalWrite(band_12m,HIGH);
      digitalWrite(band_10m,HIGH);  
  //Serial.begin(9600);
  Wire.begin();
  lcd.setBacklight(255);
  lcd.begin(20, 4);
  lcd.setCursor(3, 1);
  lcd.print("R140 Commander");
  lcd.setCursor(0,3);
  delay(1000);
  lcd.print("booting...");
  delay(3000);
  lcd.clear();
  init_lcd();
  digitalWrite(band_160m,LOW); //set default band
}


// główny program
void loop(){
  encoder_go();
  if(current_set_band != current_band && millis() >= time_to_set_band){
    set_new_band();
    current_set_band = current_band;
  }
}

