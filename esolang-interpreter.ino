#include <LiquidCrystal_I2C.h>

int currentCommand = 0;
int analogInputValue1 = 0;
int analogInputValue2 = 0;
int digitalInputValue1 = 0;
int digitalInputValue2 = 0;
int digitalInputDelay = 0;
int currentProgramCell = 0;
int currentCell = 0;
int errorType = 0;
int errorCell = 0;
int programLength = 0;
int cells[100] = {0};
int program[100] = {0};
int program1[100] = {0};
int program2[100] = {0};
int program3[100] = {0};
int program4[100] = {0};
bool nextCommand = false;
bool previousCommand = false;
bool nextCell = false;
bool previousCell = false;
bool programState = true;
bool deviceOn = false;
bool clearCells = true;
char commands[16] = {'+', '-', '<', '>', '[', ']', ',', '.', '?', '!', '~', '^', '#', '=', '@', '*'};
char ascii[27] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};
char *programNames[] = {"PROGRAM1", "PROGRAM2", "PROGRAM3", "PROGRAM4"};
char programName[] = "AAAAAA";
byte heart[] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};
byte checkMark[] = {
  0b00000,
  0b00001,
  0b00011,
  0b10110,
  0b11100,
  0b01000,
  0b00000,
  0b00000
};
byte crossMark[] = {
  0b00000,
  0b10001,
  0b11011,
  0b01110,
  0b01110,
  0b11011,
  0b10001,
  0b00000
};
byte particulate[] = {
  0b01010,
  0b10001,
  0b00100,
  0b01010,
  0b00100,
  0b10001,
  0b01010,
  0b00000
};
byte apple[] = {
  0b00010,
  0b00100,
  0b01110,
  0b10101,
  0b10001,
  0b10001,
  0b01110,
  0b00000
};
byte banana[] = {
  0b00000,
  0b00110,
  0b01110,
  0b10100,
  0b10100,
  0b10010,
  0b01001,
  0b00110
};
byte entity[] = {
  0b01110,
  0b11111,
  0b10101,
  0b11111,
  0b10101,
  0b11011,
  0b01110,
  0b11011
};

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
    lcd.init();
    lcd.createChar(0, heart);
    lcd.createChar(1, checkMark);
    lcd.createChar(2, crossMark);
    lcd.createChar(3, particulate);
    lcd.createChar(4, apple);
    lcd.createChar(5, banana);
    lcd.createChar(6, entity);
    lcd.setCursor(0, 0);
}

void loop() {
    analogInputValue1 = analogRead(14);
    analogInputValue2 = analogRead(15);
    digitalInputValue1 = analogRead(16);
    digitalInputValue2 = analogRead(17);
    
    if (analogInputValue1 > 767 && programState) {
        nextCommand = true;
    }

    if (analogInputValue1 < 255 && programState) {
        previousCommand = true;
    }

    if (analogInputValue1 < 520 && nextCommand && deviceOn) {
        if (currentCommand == 15) {
            currentCommand = 0;
        } else {
            currentCommand++;
        }

        nextCommand = false;
    }

    if (analogInputValue1 > 500 && previousCommand && deviceOn) {
        if (currentCommand == 0) {
            currentCommand = 15;
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
        lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
        nextCell = false;
    }

    if (analogInputValue2 > 500 && previousCell && currentProgramCell != 0 && deviceOn) {
        currentProgramCell--;
        lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
        previousCell = false;
    }
    
    if (digitalInputValue1 > 1000 && analogInputValue2 < 1000 && digitalInputDelay == 500 && currentProgramCell != 101 && programState && deviceOn) {
        if (program[currentProgramCell] == 0) {
            program[currentProgramCell] = currentCommand + 1;
            lcd.print(commands[currentCommand]);
            currentProgramCell++;
            lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
        } else {
            program[currentProgramCell] = currentCommand + 1;
            lcd.print(commands[currentCommand]);
        }
        
        digitalInputDelay = 0;
    }
    
    if ((digitalInputValue2 == 0 && analogInputValue1 > 50 && analogInputValue2 > 500 && digitalInputDelay == 500 && deviceOn) || errorType != 0) {
        lcd.clear();
        delay(500);
        digitalInputValue2 = analogRead(17);

        if (digitalInputValue2 == 0) {
            delay(500);
            lcd.noBlink();
            digitalInputValue2 = analogRead(17);

            if (digitalInputValue2 == 0) {
                lcd.noBacklight();
                deviceOn = false;
            } else {
                for (int i = 0; i < 4; i++) {
                    lcd.setCursor(1, i);
                    lcd.print(programNames[i]);
                }

                lcd.setCursor(0, 0);
                lcd.print(">");
                currentCommand = 0;
                
                while (digitalInputValue1 < 1000 || analogInputValue1 > 520) {
                    analogInputValue1 = analogRead(14);
                    digitalInputValue1 = analogRead(16);
                    
                    if (analogInputValue1 > 767) {
                        previousCommand = true;
                    }

                    if (analogInputValue1 < 255) {
                        nextCommand = true;
                    }
                    
                    if (analogInputValue1 < 520 && previousCommand) {
                        lcd.setCursor(0, currentCommand);
                        lcd.print(" ");

                        if (currentCommand == 0) {
                            currentCommand = 3;
                        } else {
                            currentCommand--;
                        }
                        
                        lcd.setCursor(0, currentCommand);
                        lcd.print(">");
                        delay(500);
                        previousCommand = false;
                    }

                    if (analogInputValue1 > 400 && nextCommand) {
                        lcd.setCursor(0, currentCommand);
                        lcd.print(" ");

                        if (currentCommand == 3) {
                            currentCommand = 0;
                        } else {
                            currentCommand++;
                        }
                        
                        lcd.setCursor(0, currentCommand);
                        lcd.print(">");
                        delay(500);
                        nextCommand = false;
                    }
                }

                errorCell = currentCommand;
                digitalInputValue1 = 0;
                currentCommand = 0;
                lcd.clear();
                lcd.setCursor(1, 0);
                lcd.print("Save");
                lcd.setCursor(1, 1);
                lcd.print("Load");
                lcd.setCursor(1, 2);
                lcd.print("Rename");
                lcd.setCursor(1, 3);
                lcd.print("Clear");
                lcd.setCursor(0, 0);
                lcd.print(">");
                delay(500);

                while (digitalInputValue1 < 1000 || analogInputValue1 > 520) {
                    analogInputValue1 = analogRead(14);
                    digitalInputValue1 = analogRead(16);
                    
                    if (analogInputValue1 > 767) {
                        previousCommand = true;
                    }

                    if (analogInputValue1 < 255) {
                        nextCommand = true;
                    }
                    
                    if (analogInputValue1 < 520 && previousCommand) {
                        lcd.setCursor(0, currentCommand);
                        lcd.print(" ");

                        if (currentCommand == 0) {
                            currentCommand = 3;
                        } else {
                            currentCommand--;
                        }
                        
                        lcd.setCursor(0, currentCommand);
                        lcd.print(">");
                        delay(500);
                        previousCommand = false;
                    }

                    if (analogInputValue1 > 500 && nextCommand) {
                        lcd.setCursor(0, currentCommand);
                        lcd.print(" ");

                        if (currentCommand == 3) {
                            currentCommand = 0;
                        } else {
                            currentCommand++;
                        }
                        
                        lcd.setCursor(0, currentCommand);
                        lcd.print(">");
                        delay(500);
                        nextCommand = false;
                    }
                }

                if (currentCommand == 0) {
                    if (errorCell == 0) {
                        for (int i = 0; i < 100; i++) {
                            program1[i] = program[i];
                        }
                    }

                    if (errorCell == 1) {
                        for (int i = 0; i < 100; i++) {
                            program2[i] = program[i];
                        }
                    }

                    if (errorCell == 2) {
                        for (int i = 0; i < 100; i++) {
                            program3[i] = program[i];
                        }
                    }

                    if (errorCell == 3) {
                        for (int i = 0; i < 100; i++) {
                            program4[i] = program[i];
                        }
                    }

                    for (int i = 0; i < 100; i++) {
                        program[i] = 0;
                    }
                }

                if (currentCommand == 1) {
                    if (errorCell == 0) {
                        for (int i = 0; i < 100; i++) {
                            program[i] = program1[i];
                        }
                    }

                    if (errorCell == 1) {
                        for (int i = 0; i < 100; i++) {
                            program[i] = program2[i];
                        }
                    }

                    if (errorCell == 2) {
                        for (int i = 0; i < 100; i++) {
                            program[i] = program3[i];
                        }
                    }

                    if (errorCell == 3) {
                        for (int i = 0; i < 100; i++) {
                            program[i] = program4[i];
                        }
                    }
                }

                if (currentCommand == 2) {
                    digitalInputValue1 = 0;
                    currentCommand = 0;
                    currentCell = 0;
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("PROGRAM NAME=");
                    lcd.setCursor(13, 0);
                    lcd.blink();
                    delay(500);

                    while (currentCell < 5) {
                        analogInputValue1 = analogRead(14);
                        digitalInputValue1 = analogRead(16);
                        
                        if (analogInputValue1 > 767) {
                          nextCommand = true;
                        }
                        
                        if (analogInputValue1 < 255) {
                          previousCommand = true;
                        }
                        
                        if (analogInputValue1 < 520 && nextCommand) {
                            if (currentCommand == 26) {
                                currentCommand = 0;
                            } else {
                                currentCommand++;
                            }
                            
                            delay(500);
                            nextCommand = false;
                        }
                        
                        if (analogInputValue1 > 500 && previousCommand) {
                            if (currentCommand == 0) {
                                currentCommand = 26;
                            } else {
                                currentCommand--;
                            }
                            
                            delay(500);
                            previousCommand = false;
                        }

                        if (digitalInputValue1 > 1000 && analogInputValue1 < 520) {
                            programName[currentCell] = ascii[currentCommand];
                            lcd.print(ascii[currentCommand]);

                            if (currentCell < 5) {
                                currentCell++;
                                lcd.setCursor(currentCell + 13, 0);
                            }

                            delay(500);
                        }
                    }

                    programNames[errorCell] = programName;
                    currentCommand = 0;
                }

                if (currentCommand == 3) {
                    digitalInputValue1 = 0;
                    currentCommand = 0;
                    lcd.clear();
                    lcd.setCursor(1, 0);
                    lcd.print("Confirm");
                    lcd.setCursor(1, 1);
                    lcd.print("Cancel");
                    lcd.setCursor(0, 0);
                    lcd.print(">");
                    delay(500);

                    while (digitalInputValue1 < 1000 || analogInputValue1 > 520) {
                        analogInputValue1 = analogRead(14);
                        digitalInputValue1 = analogRead(16);
                        
                        if (analogInputValue1 > 767 || analogInputValue1 < 255) {
                            nextCommand = true;
                        }
                        
                        if ((analogInputValue1 < 520 || analogInputValue1 > 500) && nextCommand) {
                            lcd.setCursor(0, currentCommand);
                            lcd.print(" ");
                            currentCommand = (currentCommand * -1) + 1;
                            lcd.setCursor(0, currentCommand);
                            lcd.print(">");
                            delay(500);
                            nextCommand = false;
                        }
                    }

                    if (currentCommand == 0) {
                        if (errorCell = 0) {
                            for (int i = 0; i < 100; i++) {
                                program1[i] = 0;
                            }
                        }

                        if (errorCell == 1) {
                            for (int i = 0; i < 100; i++) {
                                program2[i] = 0;
                            }
                        }

                        if (errorCell == 2) {
                            for (int i = 0; i < 100; i++) {
                                program3[i] = 0;
                            }
                        }

                        if (errorCell == 3) {
                            for (int i = 0; i < 100; i++){
                                program4[i] = 0;
                            }
                        }
                    }
                }

                errorCell = programLength + 1;
                errorType = 1;
            }
        } else {
            currentProgramCell = 0;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.blink();

            for (int i = 0; i < 100; i++) {
                if (program[i] != 0) {
                    programLength = i;

                    if (clearCells && !(programState)) {
                        cells[i] = 0;
                    }
                }
            }

            clearCells = true;
            
            if (errorType == 0) {
              programState = !(programState);
            }
            
            if (programState && program[0] != 0) {
                for (currentProgramCell = 0; currentProgramCell < programLength; currentProgramCell++) {
                    lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
                    lcd.print(commands[program[currentProgramCell] - 1]);
                }

                lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
                lcd.print(commands[program[currentProgramCell] - 1]);
                
                if (errorType != 0) {
                    currentProgramCell = errorCell;
                    lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
                }
            }

            errorType = 0;
            
            if (!(programState)) {
                lcd.noBlink();
                currentCell = 0;
                currentProgramCell = 0;
            }

            delay(500);
        }
        
        digitalInputDelay = 0;
    }

    if (digitalInputValue2 != 0 && !(deviceOn)) {
        lcd.backlight();

        for (int i = 0; i < 100; i++) {
            if (program[i] != 0) {
                programLength = i;
            }
        }
        
        if (program[0] != 0) {
            for (currentProgramCell = 0; currentProgramCell < programLength; currentProgramCell++) {
                lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
                lcd.print(commands[program[currentProgramCell] - 1]);
            }

            lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
            lcd.print(commands[program[currentProgramCell] - 1]);
        }

        lcd.blink();
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

    if (digitalInputDelay != 500) {
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
                errorType = 1;
            } else {
                currentCell--;
            }
        }

        if (program[currentProgramCell] == 4) {
            if (currentCell == 100) {
                errorType = 2;
            } else {
                currentCell++;
            }
        }

        if (program[currentProgramCell] == 5 && cells[currentCell] == 0) {
            while (program[currentProgramCell] != 6) {
                if (currentProgramCell == programLength) {
                    errorType = 3;
                    break;
                } else {
                    currentProgramCell++;
                }
            }
        }

        if (program[currentProgramCell] == 6 && cells[currentCell] != 0) {
            while (program[currentProgramCell] != 5) {
                if (currentProgramCell == 0) {
                    errorType = 4;
                    break;
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
            lcd.setCursor(cells[currentCell + 2], cells[currentCell + 3]);

            if (cells[currentCell + 1] == 0) {
                lcd.print(cells[currentCell]);
            } else {
                if (cells[currentCell] < 0 || cells[currentCell] > 49) {
                    errorType = 5;
                } else {
                    if (cells[currentCell] < 27) {
                        lcd.print(ascii[cells[currentCell]]);
                    }

                    if (cells[currentCell] > 26 && cells[currentCell] < 43) {
                        lcd.print(commands[cells[currentCell] - 27]);
                    }

                    if (cells[currentCell] > 42) {
                        lcd.write(byte(cells[currentCell] - 43));
                    }
                }
            }
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

        if (program[currentProgramCell] == 15) {
            if (cells[currentCell] < 0 || cells[currentCell] > 3) {
                errorType = 6;
            } else {
                if (cells[currentCell] == 0) {
                    for (int i = 0; i < 100; i++) {
                        program[i] = program1[i];
                    }
                }
                
                if (cells[currentCell] == 1) {
                    for (int i = 0; i < 100; i++) {
                        program[i] = program2[i];
                    }
                }
                
                if (cells[currentCell] == 2) {
                    for (int i = 0; i < 100; i++) {
                        program[i] = program3[i];
                    }
                }
                
                if (cells[currentCell] == 3) {
                    for (int i = 0; i < 100; i++) {
                        program[i] = program4[i];
                    }
                }
                
                for (int i = 0; i < 100; i++) {
                    if (program[i] != 0) {
                        programLength = i;
                    }
                }
                
                currentProgramCell = -1;
            }
        }

        if (program[currentProgramCell] == 16) {
            clearCells = false;
        }
        
        if (errorType != 0) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("ERR: " + String(errorType));
            lcd.setCursor(1, 1);
            lcd.print("Goto");
            lcd.setCursor(1, 2);
            lcd.print("Quit");
            lcd.setCursor(0, 1);
            lcd.print(">");
            currentCommand = 0;
            
            while (digitalInputValue1 < 1000) {
                analogInputValue1 = analogRead(14);
                digitalInputValue1 = analogRead(16);
                
                if (analogInputValue1 > 767 || analogInputValue1 < 255) {
                    nextCommand = true;
                }
                
                if ((analogInputValue1 < 520 || analogInputValue1 > 500) && nextCommand) {
                    lcd.setCursor(0, currentCommand + 1);
                    lcd.print(" ");
                    currentCommand = (currentCommand * -1) + 1;
                    lcd.setCursor(0, currentCommand + 1);
                    lcd.print(">");
                    delay(500);
                    nextCommand = false;
                }
            }
            
            if (currentCommand == 0) {
                errorCell = currentProgramCell;
            } else {
                errorCell = programLength + 1;
            }

            programState = true;
        }

        currentProgramCell++;
    }
}
