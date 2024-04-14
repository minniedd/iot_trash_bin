#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP8266.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// Define WIFI
#define WIFI_SSID "" //SSID
#define WIFI_PASSWORD "" // Password

/* API Key */
#define API_KEY ""

/* RTDB URL */
#define DATABASE_URL "iot-mina-default-rtdb.firebaseio.com" 

/* User INFO */
#define USER_EMAIL ""
#define USER_PASSWORD ""

//define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

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

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h


  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);
}

void loop(){
  measure();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("garbage level:");
  lcd.setCursor(5,1);
  lcd.print(bin_lvl);
  lcd.println("%");

    if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
          sendDataPrevMillis = millis();

         Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/test/int"), bin_lvl) ? "ok" : fbdo.errorReason().c_str());
    }

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


