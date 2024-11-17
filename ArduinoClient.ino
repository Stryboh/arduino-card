#include <ArduinoUniqueID.h>

char uniqueIDBuffer[UniqueIDsize * 2 + 1]; 
bool keepSending = true;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600); 

    while (!Serial);
    for (size_t i = 0; i < UniqueIDsize; i++) {
        sprintf(&uniqueIDBuffer[i * 2], "%02X", UniqueID[i]);
    }
}

void loop() {
    if (keepSending) {
        Serial.println(uniqueIDBuffer);
        delay(1000);
    }

    if(!keepSending){
      digitalWrite(LED_BUILTIN, HIGH);
    }

    if (Serial.available() > 0) {
        String incomingMessage = Serial.readStringUntil('\n');
        incomingMessage.trim();
        if (incomingMessage == "STOP") {
            keepSending = false;
            Serial.println("STOPPING");
        }
    }
}
