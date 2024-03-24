#include <BleKeyboard.h>

BleKeyboard bleKeyboard("Wifi+BT Module", "Alexander Gaggl", 100);

int pastCoordinate = 0;
int currentCoordinate = 0;
int currentCharValue = 0;
int inputClock = 0;
int shift = 0;
char inChar = 'a';
char dataChar = 'a';
char ascii[55] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' ', '+', '-', '<', '>', '[', ']', ',', '.', '?', '!', ':', '~', '^', '=', '@', '*', '/', '\"'};
bool readReady = false;
bool enter = false;

void setup() {
    Serial.begin(9600);
    bleKeyboard.begin();
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {
        inChar = Serial.read();
        
        if (dataChar == 'a') {
            dataChar = inChar;
        } else {
            if (inChar == '0') {
                if (enter) {
                    Serial.print('e');
                } else {
                    if (shift > 0 && currentCharValue < 18) {
                        Serial.print(ascii[currentCharValue + 37]);
                    } else {
                        Serial.print(ascii[currentCharValue % 37]);
                    }
                }
                
                if (shift == 1) {
                    shift = 0;
                    updateShiftIndicator();
                }
                
                enter = false;
            }
            
            if (inChar == '1') {
                shift = int(dataChar) - 48;
                updateShiftIndicator();
            }
            
            if (inChar == '2') {
                bleKeyboard.print(dataChar);
            }
            
            dataChar = 'a';
        }
    }
    
    currentCoordinate = digitalRead(2) + (digitalRead(3) * 2);
    
    if (currentCoordinate != pastCoordinate && readReady) {
        if (currentCoordinate == (pastCoordinate + 1) || (pastCoordinate - currentCoordinate) == 2) {
            if (currentCharValue == 37) {
                currentCharValue = 0;
            } else {
                currentCharValue++;
            }
        } else {
            if (currentCharValue == 0) {
                currentCharValue = 37;
            } else {
                currentCharValue--;
            }
        }

        pastCoordinate = currentCoordinate;
    }

    if (digitalRead(4) == 1) {
        readReady = true;
        digitalWrite(6, HIGH);
    }

    if (digitalRead(5) == 1 && readReady) {
        inputClock = 0;
        
        while (digitalRead(5) == 1 && inputClock < 100) {
            delay(10);
            inputClock++;
        }

        if (inputClock == 100) {
            currentCharValue = 0;
            readReady = false;
            digitalWrite(6, LOW);
        } else {
            if (currentCharValue == 37) {
                if (shift == 2) {
                    shift = 0;
                } else {
                    shift++;
                }

                updateShiftIndicator();
            } else {
                enter = true;
            }
        }
    }
}

void updateShiftIndicator() {
    for (int i = 0; i < 2; i++) {
        if (shift == i) {
            digitalWrite(i + 6, HIGH);
        } else {
            digitalWrite(i + 6, LOW);
        }
    }
}