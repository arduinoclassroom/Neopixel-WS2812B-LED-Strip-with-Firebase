#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define PIN 13

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define API_KEY "YOUR_FIREBASE_PROJECT_API_KEY"

#define DATABASE_URL "YOUR_FIREBASE_PROJECT_DATABASE_URL" //Enter your database url without "https://".

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
String stringValue = " ";

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
                                               NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
                                               NEO_GRB + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(204, 0, 204), matrix.Color(204, 204, 0), matrix.Color(0, 255, 255),
  matrix.Color(255, 10, 127), matrix.Color(127, 0, 255), matrix.Color(0, 255, 0),
  matrix.Color(255, 99, 255)
};

void setup() {

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(120);
  matrix.setTextColor(colors[0]);

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.println("Connected!");

  config.api_key = API_KEY;

  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }


  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

int x = matrix.width();
int pass = 0;

void loop() {

  matrix.fillScreen(0);
  matrix.setCursor(x, 0);

  if (Firebase.ready() ) {

    if (Firebase.RTDB.getString(&fbdo, "/L1")) {
      if (fbdo.dataType() == "string") {
        stringValue = fbdo.stringData();
        Serial.println(stringValue);
        matrix.print(stringValue);  

        if (--x < -36) {
          x = matrix.width();

          if (++pass >= 8) pass = 0;
          matrix.setTextColor(colors[pass]);
        }
        matrix.show();
        delay(50);
      }

    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}

/*
Subscribe Arduino classroom Youtube channel : www.youtube.com/Arduinoclassroom
