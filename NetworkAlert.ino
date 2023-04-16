#include <SoftwareSerial.h>
#include <BlynkSimpleStream.h>
#define BLYNK_TEMPLATE_ID "TMPL6Up2GjwQ-"
#define BLYNK_TEMPLATE_NAME "Final"
#define BLYNK_AUTH_TOKEN "7Ju9bCD-D6toJjWiRUu5WY0SRvWKvp5n"
#define RX_PIN 10
#define TX_PIN 11

SoftwareSerial sim7020e(RX_PIN, TX_PIN);
char buffer[200];
unsigned long lastCheckTime = 0;
unsigned long checkInterval = 120000;

char auth[] = "7Ju9bCD-D6toJjWiRUu5WY0SRvWKvp5n";

void setup()
{
  sim7020e.begin(9600);
  Serial.begin(9600);
  Blynk.begin(Serial, auth);
  Serial.println("Start...");
}

#define LED_PIN V3
int lowSignalCount = 0; 

void loop()
{
  // Kiểm tra AT+CENG mỗi 2 phút
  if (millis() - lastCheckTime >= checkInterval)
  {
    sim7020e.println("AT+CENG?");
    delay(1000);

    int pos1 = 0;
    int pos2 = 0;
    bool lowSignal = false; 

    while (sim7020e.available())
    {
      char c = sim7020e.read();
      buffer[pos1++] = c;
      if (c == '\n')
      {
        if (strstr(buffer, "+CENG") != NULL)
        {
          pos2 = pos1 - 5; 
          String ceng(buffer);
          String rsrp = ceng.substring(20, 23);
          String rsrq = ceng.substring(28, 31); 

          Serial.println("RSRP: " + rsrp + " dBm, RSRQ: " + rsrq + " dB");
          Blynk.virtualWrite(V1, rsrp.toInt()); 
          Blynk.virtualWrite(V2, rsrq.toInt());

          // Check if RSRP value is less than -90 dBm
          if (rsrp.toInt() < -90) {
            lowSignal = true; 
            lowSignalCount++; 
          } else {
            lowSignal = false; 
            lowSignalCount = 0; 
          }

          
          if (lowSignalCount >= 3) {
            digitalWrite(LED_PIN, HIGH);
          } else {
            digitalWrite(LED_PIN, LOW);
          }
        }
        pos1 = 0;
        memset(buffer, 0, sizeof(buffer));
      }
    }
    lastCheckTime = millis();
  }

  Blynk.run();
}
