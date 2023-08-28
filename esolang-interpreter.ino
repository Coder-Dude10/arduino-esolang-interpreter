#include <SevSeg.h>

int currentCommand = 1;
int analogInputValue1 = 0;
int analogInputValue2 = 0;
int digitalInputValue1 = 0;
int digitalInputValue2 = 0;
int digitalInputValue3 = 0;
int digitalInputDelay = 0;
int currentProgramCell = 0;
int currentCell = 0;
int programLength = 0;
int program[100] = {0};
int cells[100] = {0};
bool nextCommand = false;
bool previousCommand = false;
bool nextCell = false;
bool previousCell = false;
bool programState = true;
bool error = false;
bool deviceOn = false;
char outputError[] = "err1";
String outputString = "";

SevSeg sevseg;

void setup() {
  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 12, 13};
  bool resistorsOnSegments = 0;
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);
}

void loop() {
    analogInputValue1 = analogRead(14);
    analogInputValue2 = analogRead(16);
    digitalInputValue1 = analogRead(17);
    digitalInputValue2 = analogRead(18);
    digitalInputValue3 = analogRead(19);

    if (digitalInputValue3 == 1023 && !(deviceOn)) {
        tone(11, 15, 5.81395348837);
        delay(200);
        tone(11, 14, 5.81395348837);
        delay(25);
        tone(11, 184, 69.7674418605);
        delay(150);
        tone(11, 14, 5.81395348837);
        delay(35);
        tone(11, 2489, 69.7674418605);
        delay(150);
        tone(11, 14, 5.81395348837);
        delay(35);
        tone(11, 2217, 69.7674418605);
        delay(150);
        tone(11, 15, 5.81395348837);
        delay(100);
        tone(11, 13, 5.81395348837);
        delay(100);
        tone(11, 14, 5.81395348837);
        delay(200);
        tone(11, 2217, 151.162790698);
        delay(100);
        tone(11, 14, 5.81395348837);
        delay(35);
        tone(11, 9, 5.81395348837);
        delay(100);
        tone(11, 15, 5.81395348837);

        deviceOn = true;
    }

    if (digitalInputValue3 != 1023 && deviceOn) {
        deviceOn = false;
    }
    
    if (analogInputValue1 > 767 && programState) {
        nextCommand = true;
    }

    if (analogInputValue1 < 255 && programState) {
        previousCommand = true;
    }

    if (analogInputValue1 < 520 && nextCommand && deviceOn) {
        if (currentCommand == 14) {
            currentCommand = 1;
        } else {
            currentCommand++;
        }

        nextCommand = false;
    }

    if (analogInputValue1 > 500 && previousCommand && deviceOn) {
        if (currentCommand == 1) {
            currentCommand = 14;
        } else {
            currentCommand--;
        }

        previousCommand = false;
    }

    if (analogInputValue2 > 767 && programState) {
        nextCell = true;
    }

    if (analogInputValue2 < 255 && programState) {
        previousCell = true;
    }
    
    if (analogInputValue2 < 520 && nextCell && currentProgramCell != 100 && program[currentProgramCell] != 0 && deviceOn) {
        currentProgramCell++;
        nextCell = false;
    }

    if (analogInputValue2 > 500 && previousCell && currentProgramCell != 0 && deviceOn) {
        currentProgramCell--;
        previousCell = false;
    }
    
    if (digitalInputValue1 == 1023 && digitalInputDelay == 1000 && currentProgramCell != 101 && programState && deviceOn) {
        if (program[currentProgramCell] == 0) {
            program[currentProgramCell] = currentCommand;
            currentProgramCell++;
        } else {
            program[currentProgramCell] = currentCommand;
        }
        
        digitalInputDelay = 0;
    }

    if (digitalInputValue2 == 0 && analogInputValue1 > 50 && digitalInputDelay == 1000 && deviceOn) {
        programState = !(programState);
        
        if (!(programState)) {
            error = false;
            currentCell = 0;
            currentProgramCell = 0;
            
            for (int i = 0; i < 100; i++) {
                if (program[i] != 0) {
                    programLength = i;
                    cells[i] = 0;
                }
            }
        }

        digitalInputDelay = 0;
    }

    if (digitalInputDelay != 1000) {
      digitalInputDelay++;
    }

    if (!(programState) && (currentProgramCell - 1) != programLength && deviceOn) {
        if (program[currentProgramCell] == 1) {
            cells[currentCell]++;
        }

        if (program[currentProgramCell] == 2) {
            cells[currentCell]--;
        }
        
        if (program[currentProgramCell] == 3) {
            if (currentCell == 0) {
                error = true;
                outputError[3] = '1';
                currentProgramCell = programLength;
            } else {
                currentCell--;
            }
        }

        if (program[currentProgramCell] == 4) {
            if (currentCell == 100) {
                error = true;
                outputError[3] = '2';
                currentProgramCell = programLength;
            } else {
                currentCell++;
            }
        }

        if (program[currentProgramCell] == 5 && cells[currentCell] == 0) {
            while (program[currentProgramCell] != 6) {
                if (currentProgramCell == programLength) {
                    error = true;
                    outputError[3] = '3';
                    currentProgramCell = programLength;
                    program[currentProgramCell] = 6;
                } else {
                    currentProgramCell++;
                }
            }
        }

        if (program[currentProgramCell] == 6 && cells[currentCell] != 0) {
            while (program[currentProgramCell] != 5) {
                if (currentProgramCell == 0) {
                    error = true;
                    outputError[3] = '4';
                    currentProgramCell = programLength;
                    program[currentProgramCell] = 5;
                } else {
                    currentProgramCell--;
                }
            }
        }

        if (program[currentProgramCell] == 7) {
            cells[currentCell] = round(analogInputValue1 / 100);
            cells[currentCell + 1] = round(analogInputValue2 / 100);

            if (digitalInputValue1 == 1023) {
                cells[currentCell + 2] = 1;
            } else {
                cells[currentCell + 2] = 0;
            }
        }

        if (program[currentProgramCell] == 8) {
            outputString = String(cells[currentCell]);
        }

        if (program[currentProgramCell] == 9) {
            cells[currentCell] = random(2);
        }

        if (program[currentProgramCell] == 10) {
            tone(11, (cells[currentCell] * 100), (cells[currentCell + 1] * 100));
        }

        if (program[currentProgramCell] == 11) {
            delay(cells[currentCell] * 100);
        }

        if (program[currentProgramCell] == 12) {
            currentProgramCell = cells[currentCell];
        }

        if (program[currentProgramCell] == 13) {
            currentCell = cells[currentCell];
        }

        if (program[currentProgramCell] == 14) {
            cells[currentCell] = cells[cells[currentCell]];
        }

        currentProgramCell++;
    }

    if (programState) {
        outputString = String(currentCommand) + String(program[currentProgramCell]) + String(currentProgramCell);
    }

    if (deviceOn) {
        if (error) {
            sevseg.setChars(outputError);
        } else {
            sevseg.setNumber(outputString.toInt(), 0);
        }
    } else {
        sevseg.setChars("");
    }

    sevseg.refreshDisplay();
}
