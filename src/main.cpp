#include <Arduino.h>
#include <SSD1306.h>

SSD1306             display(0x3c, 4, 5, GEOMETRY_128_32);
const int           pulseA = 12;
const int           pulseB = 13;
const int           pushSW = 2;
volatile int        lastEncoded = 0;
volatile long       encoderValue = 0;
char data[10];

IRAM_ATTR void handleRotary() {
    // Never put any long instruction
    int MSB = digitalRead(pulseA); //MSB = most significant bit
    int LSB = digitalRead(pulseB); //LSB = least significant bit

    int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
    int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
    lastEncoded = encoded; //store this value for next time
    if (encoderValue > 255) {
        encoderValue = 255;
    } else if (encoderValue < 0 ) {
        encoderValue = 0;
    }
}

IRAM_ATTR void buttonClicked() {
    Serial.println("pushed");
}

void setup() {
    Serial.begin(115200);
    pinMode(pushSW, INPUT_PULLUP);
    pinMode(pulseA, INPUT_PULLUP);
    pinMode(pulseB, INPUT_PULLUP);
    attachInterrupt(pushSW, buttonClicked, FALLING);
    attachInterrupt(pulseA, handleRotary, CHANGE);
    attachInterrupt(pulseB, handleRotary, CHANGE);

    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
    
    display.display();
    display.clear();
}

void loop() {
    delay(500);
    // sprintf(data, "%.1f", encoderValue);
    ultoa(encoderValue, data, 10);

    display.drawString(10, 10, data);
    
    display.display();
    display.clear();
    
    delay(1000);
    Serial.println(encoderValue);
}
