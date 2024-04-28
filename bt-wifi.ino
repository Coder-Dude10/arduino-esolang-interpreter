#include <BleKeyboard.h>

BleKeyboard bleKeyboard("Wifi+BT Module", "Alexander Gaggl", 100);

int pastCoordinate = 0;
int currentCoordinate = 0;
int currentCharValue = 0;
int inputClock = 0;
int shift = 0;
int commands[18] = {43, 45, 60, 62, 91, 93, 44, 46, 63, 33, 58, 126, 94, 61, 64, 42, 47, 34};
bool readReady = false;
bool enter = false;
char inChar = 'a';
char dataChar = 'a';
String charNames[7] = {"[heart]", "[checkMark]", "[crossMark]", "[apple]", "[banana]", "[entity]", "[deleteChar]"};

void setup() {
    Serial.begin(9600);
    bleKeyboard.begin();
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
}

void updateShiftLayer() {
    if (shift == 3) {
        digitalWrite(5, HIGH);
        digitalWrite(6, HIGH);
        digitalWrite(7, LOW);
    } else {
        for (int i = 0; i < 3; i++) {
            if (shift == i) {
                digitalWrite(i + 5, HIGH);
            } else {
                digitalWrite(i + 5, LOW);
            }
        }
    }
}

int ascii(int asciiValue) {
    if (asciiValue < 10) {
        return (asciiValue + 48);
    } else {
        if (asciiValue < 37) {
            if (asciiValue == 36) {
                return 32;
            } else {
                return (asciiValue + 55);
            }
        } else {
            return commands[asciiValue - 37];
        }
    }
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
                    Serial.print(char(ascii(currentCharValue + (shift * 14))));
                }
                
                enter = false;
            }
            
            if (inChar == '1') {
                shift = int(dataChar) - 48;
                updateShiftLayer();
            }
            
            if (inChar == '2') {
                if (int(dataChar) < 128) {
                    bleKeyboard.print(dataChar);
                } else {
                    bleKeyboard.print(charNames[int(dataChar) - 128]);
                }
            }
            
            dataChar = 'a';
        }
    }
    
    currentCoordinate = digitalRead(2) + (digitalRead(3) * 2);
    
    if (currentCoordinate != pastCoordinate && readReady) {
        if (currentCoordinate == (pastCoordinate + 1) || (pastCoordinate - currentCoordinate) == 2) {
            if (currentCharValue == 14) {
                currentCharValue = 0;
            } else {
                currentCharValue++;
            }
        } else {
            if (currentCharValue == 0) {
                currentCharValue = 14;
            } else {
                currentCharValue--;
            }
        }

        pastCoordinate = currentCoordinate;
    }
    
    if (digitalRead(4) == 1) {
        inputClock = 0;
        
        while (digitalRead(4) == 1 && inputClock < 100) {
            delay(10);
            inputClock++;
        }

        if (inputClock == 100) {
            if (readReady) {
                digitalWrite(6, LOW);
            } else {
                digitalWrite(6, HIGH);
            }
            
            shift = 0;
            enter = false;
            currentCharValue = 0;
            currentCoordinate = 0;
            readReady = !(readReady);
        } else {
            if (currentCharValue == 14) {
                if (shift == 3) {
                    shift = 0;
                } else {
                    shift++;
                }

                updateShiftLayer();
            } else {
                enter = true;
            }
        }
    }
}
