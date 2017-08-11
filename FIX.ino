#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 20, 4);

#include <Wire.h>
#include "DHT.h"
#define DHTPIN 2 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#include <SoftwareSerial.h>
#define DEBUG true
#define SSID "hamdan"     // "SSID-WiFiname"
#define PASS "assalamualaikum" // "password"
#define IP "184.106.153.149"      // thingspeak.com ip
String msg = "GET /update?key=PDO4PJQ22E8VH2ZU"; //change it with your api key like "GET /update?key=Your Api Key"
SoftwareSerial esp8266(11,12);


int     temp;
int     hum;
int     temperature;
String  tempC;
String  humH;
int     error;


const byte interruptPin = 3;
const int buttonUp      = 4;  
const int buttonDown    = 5;  
const int buttonSet3    = 6;  
const int buttonSet2    = 7;  
const int buttonSet1    = 8;     
const int fan           = 9 ;  
const int heater        = 10;
const int startLamp     = 13; 
const int set1Lamp      = A3;
const int set2Lamp      = A2;
const int set3Lamp      = A1;
const int buzzer        = A0;

volatile byte ledState  = LOW;         

char  A           = 0;
int   B           = 0;
int   C           = 0;
char  buzzerStart = 0;
int   set1        = 30;
int   set2        = 35;
int   set3        = 40;

void setup() 
  {
    Serial.begin(115200);
    attachInterrupt(digitalPinToInterrupt(interruptPin),buttonHold, RISING);
    dht.begin();
    lcd.begin();
    lcd.backlight();
    lcd.clear();
    pinMode(buttonDown, INPUT);
    pinMode(buttonUp, INPUT);
    pinMode(buttonSet1, INPUT);
    pinMode(buttonSet2, INPUT);
    pinMode(buttonSet3, INPUT);
    pinMode(interruptPin, INPUT_PULLUP);
  
    pinMode(startLamp, OUTPUT);
    pinMode(fan, OUTPUT);
    pinMode(heater, OUTPUT);
    pinMode(set1Lamp, OUTPUT);
    pinMode(set2Lamp, OUTPUT);
    pinMode(set3Lamp, OUTPUT);
    pinMode(buzzer, OUTPUT);

    digitalWrite(heater, HIGH); 
    digitalWrite(fan, HIGH);
  

    lcd.setCursor(0, 0);
    lcd.print("  THERMOGYRO METER");
    delay(100);
    lcd.setCursor(0, 2);
    lcd.print("Connecting...");
    lcd.setCursor(0, 3);
    lcd.print("Please Wait...");
    Serial.begin(115200); //or use default 115200.
    esp8266.begin(115200);
    Serial.println("AT");
    esp8266.println("AT");

    delay(5000);

    if(esp8266.find("OK"))
      {
        connectWiFi();
      }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  THERMOGYRO METER");
    lcd.setCursor(0, 1);
    lcd.print("by Ovi Indriyani");
    lcd.setCursor(0, 2);
    lcd.print("Teknik Elektro");
    lcd.setCursor(0, 3);
    lcd.print("UNJANI - CIMAHI");
    delay(3000);
    lcd.clear();
    lcd.print("Starting.....");
    delay(1500);
  
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TEMP:  \337C");
    lcd.setCursor(14, 0);
    lcd.print("RH:  % ");
    lcd.setCursor(0, 1);
    lcd.print("SET1:");
    lcd.setCursor(7, 1);
    lcd.print("\337C");
    lcd.setCursor(0, 2);
    lcd.print("SET2:");
    lcd.setCursor(7, 2);
    lcd.print("\337C");
    lcd.setCursor(0, 3);
    lcd.print("SET3:");
    lcd.setCursor(7, 3);
    lcd.print("\337C");
    lcd.setCursor(5, 1);
    lcd.print(set1);
    lcd.setCursor(5, 2);
    lcd.print(set2);
    lcd.setCursor(5, 3);
    lcd.print(set3);  
  }

void loop() 
  { 
    delay(10);
    button();
    indicator();
    control ();
    condition ();
    temperatureChange();
    B = B+10; if (B >= 100) {temperatureRead(); B = 0;}      
  }


void buttonHold ()
  {
    ledState = !ledState;
    digitalWrite(startLamp, ledState);
    if (ledState == HIGH){A=1;}
    if (ledState == LOW){A=0; buzzerStart = 0;}  
  }


void button()
  {
    if (ledState == LOW)
      {
        if (digitalRead(buttonSet1) == HIGH && digitalRead(buttonUp) == HIGH) {set1=set1+1; delay(100); if (set1>=set2) {set1=set2;}}
        if (digitalRead(buttonSet1) == HIGH && digitalRead(buttonDown) == HIGH) {set1=set1-1; delay(100); if (set1<=10) {set1=10;}}

        if (digitalRead(buttonSet2) == HIGH && digitalRead(buttonUp) == HIGH) {set2=set2+1; delay(100); if (set2>=set3) {set2=set3;}}
        if (digitalRead(buttonSet2) == HIGH && digitalRead(buttonDown) == HIGH) {set2=set2-1; delay(100); if (set2<=set1) {set2=set1;}}

        if (digitalRead(buttonSet3) == HIGH && digitalRead(buttonUp) == HIGH) {set3=set3+1; delay(100); if (set3>=50) {set3=50;}}
        if (digitalRead(buttonSet3) == HIGH && digitalRead(buttonDown) == HIGH) {set3=set3-1; delay(100); if (set3<=set2) {set3=set2;}}

        if (digitalRead(buttonSet1) == HIGH && digitalRead(buttonSet2) == HIGH && digitalRead(buttonSet3) == HIGH) {set1=30;set2=35;set3=40;}

        lcd.setCursor(5, 1);
        lcd.print(set1);
        lcd.setCursor(5, 2);
        lcd.print(set2);
        lcd.setCursor(5, 3);
        lcd.print(set3); 
      }
   }


void indicator ()
  {
    if (ledState == HIGH)
      {
        if (set1 < temperature){analogWrite (set1Lamp,255);} else {analogWrite (set1Lamp,0 );} 
        if (set2 < temperature){analogWrite (set2Lamp,255);} else {analogWrite (set2Lamp,0 );} 
        if (set3 < temperature){analogWrite (set3Lamp,255); analogWrite (buzzer,255);}
      } 
    else 
      {
        analogWrite (set1Lamp,0 );
        analogWrite (set2Lamp,0 );
        analogWrite (set3Lamp,0 ); 
        analogWrite (buzzer,0);
      }
   }


void control ()
  {
    if (ledState == HIGH)
      {
        if (set1 < temperature && set2 < temperature && set3 < temperature) 
          {
            digitalWrite(heater, HIGH); 
            digitalWrite(fan, HIGH);
          } 
        else 
          {
            digitalWrite(heater, LOW); 
            digitalWrite(fan, LOW);
          }
       }
   else 
     {
       digitalWrite(heater, HIGH); 
       digitalWrite(fan, HIGH);
     }
  
  }


void condition()
  {
    if (A==1) 
      {
        if (set1 < temperature){lcd.setCursor(10, 1);lcd.print("STATUS:OK ");} else {lcd.setCursor(10, 1); lcd.print("STATUS:RUN");} 
        if (set2 < temperature){lcd.setCursor(10, 2);lcd.print("STATUS:OK ");} else {lcd.setCursor(10, 2); lcd.print("STATUS:RUN");}
        if (set3 < temperature){lcd.setCursor(10, 3);lcd.print("STATUS:OK ");} else {lcd.setCursor(10, 3); lcd.print("STATUS:RUN");}    
      }

    if (A==0) 
      { 
        lcd.setCursor(10, 1);
        lcd.print("STATUS:OFF");
        lcd.setCursor(10, 2);
        lcd.print("STATUS:OFF");
        lcd.setCursor(10, 3);
        lcd.print("STATUS:OFF");}
   }



void temperatureChange()
  {
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();
    char buffer[10];
    tempC = dtostrf(temp, 4, 1, buffer);
    humH = dtostrf(hum, 4, 1, buffer);
  }


void temperatureRead()
  {
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();
    temperature = dht.readTemperature() ;
    if (isnan(hum) || isnan(temp)) 
      {
        Serial.println("Failed to read from DHT sensor!");
        return;
      }
    
    lcd.setCursor(5, 0);
    lcd.print(temp , 0);
    lcd.print("\337C");
    lcd.setCursor(17, 0);
    lcd.print(hum , 0);
    lcd.print("%"); 
  
    if (ledState == HIGH)
      {
        Serial.println("");
        Serial.println("");
        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.print(" *C ");
        Serial.print("Humidity: ");
        Serial.print(hum);
        Serial.println(" %\t");
  
        String cmd = "AT+CIPSTART=\"TCP\",\"";
        cmd += IP;
        cmd  += "\",80";
        Serial.println(cmd);
        esp8266.println(cmd);
        delay(500);
        
        if(esp8266.find("Error"))
          {
            return;
          }
  
        cmd = msg ;
        cmd += "&field1=";    //field 1 for temperature
        cmd += tempC;
        cmd += "&field2=";  //field 2 for humidity
        cmd += humH;
        cmd += "\r\n";
        Serial.print("AT+CIPSEND=");
        esp8266.print("AT+CIPSEND=");
        Serial.println(cmd.length());
        esp8266.println(cmd.length());
  
        if(esp8266.find(">"))
          {
            Serial.print(cmd);
            esp8266.print(cmd);
          }
      }
  } 



boolean connectWiFi()
  {
    Serial.println("AT+CWMODE=1");
    esp8266.println("AT+CWMODE=1");
    delay(2000);
    String cmd="AT+CWJAP=\"";
    cmd+=SSID;
    cmd+="\",\"";
    cmd+=PASS;
    cmd+="\"";
    Serial.println(cmd);
    esp8266.println(cmd);
    delay(5000);
  
    if(esp8266.find("OK"))
      {
        return true;
      }
    else
      {
        return false;
      }
  } 
