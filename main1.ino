#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseArduino.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);


#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""

// wifi
char ssid[] = "SSID"; //SSID
char pass[] = "PASS"; // Password

// garbage bin heights
const int total_height = 21;
const int hold_height = 16;

// updates
int Minute = 0.30;

const int trigger = 14;
const int echo = 12;
long Time;
int x;
int i;
int distanceCM;
int resultCM;
int bin_lvl = 0;
int snsr_to_max = 0;
const int Field_number = 1; 
WiFiClient  client;

FirebaseArduino firebase;


void setup(){
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Garbage Monitoring");
  lcd.setCursor(0,1);
  lcd.print("Amina Bardak");
  Serial.begin(115200);
  pinMode(trigger, OUTPUT);
  pinMode(echo, OUTPUT);
  WiFi.mode(WIFI_STA);


  snsr_to_max = total_height - hold_height;

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  delay(2500);
}

void loop(){
  internet();
  measure();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("garbage level:");
  lcd.setCursor(5,1);
  lcd.print(bin_lvl);
  lcd.println("%");

  Firebase.setInt("/level/GLevels",bin_lvl);

  for(int i=0;i< Minute;i++){
    Serial.println("-------------------------");
    Serial.println("System Standby....");
    Serial.print(i);
    Serial.println(" Minutes elapsed.");
    delay(20000);
    delay(20000);
    delay(20000);
  }
}

void measure(){
  delay(100);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  Time = pulseIn(echo, HIGH);
  distanceCM=Time * 0.034;
  resultCM = distanceCM /2;

  bin_lvl = map(resultCM, snsr_to_max, total_height, 100, 0);
  if (bin_lvl > 100) bin_lvl = 100;
  if (bin_lvl < 0) bin_lvl = 0;
}

void internet()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid,pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
}
