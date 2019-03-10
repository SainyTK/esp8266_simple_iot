#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

const uint8_t fingerprint[20] = {0xBF, 0x16, 0xAE, 0x79, 0xD2, 0xAB, 0x71, 0x44, 0xBE, 0xD8, 0xE7, 0x55, 0xA2, 0xC7, 0x0B, 0x39, 0x68, 0xDB, 0xB5, 0xD2};

const char* ssid = "";
const char* password = "";
const char* auth = "";

const String url = "https://notify-api.line.me/api/notify";
const String TOKEN = "";

const String message = "Test Message";

void setup()
{
  pinMode(D1, INPUT);
  pinMode(D0, INPUT);

  Serial.begin(115200);

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  Blynk.begin(auth, ssid, password);
  
}

void loop()
{
  Blynk.run();
  if(digitalRead(D1) == 1 && digitalRead(D0) == 1)
    sentNotify(message);
}

void sentNotify(String msg) {
  if (WiFi.status() == WL_CONNECTED) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, url)) {  // HTTPS

      https.addHeader("Authorization", "Bearer " + TOKEN);
      https.addHeader("Content-Type", "application/x-www-form-urlencoded");

      Serial.print("[HTTPS] POST\n");
      int httpCode = https.POST("message=" + msg);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  delay(1000);
}
