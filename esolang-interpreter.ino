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
int bracketsPassed = 0;
int currentProgram = 0;
int currentDebugCell = 0;
int cells[80] = {0};
int program[80] = {0};
int program1[80] = {4, 1, 1, 4, 1, 1, 4, 1, 1, 4, 1, 1, 1, 1, 4, 1, 1, 4, 1, 1, 4, 1, 1, 4, 4, 1, 1, 4, 1, 1, 4, 1, 1, 4, 1, 1, 1, 1, 4, 1, 1, 1, 1, 4, 4, 1, 1, 1, 1, 4, 4, 1, 1, 1, 1, 
16};
int program2[80] = {4, 5, 3, 10, 4, 12, 4, 4, 6, 16};
int program3[80] = {0};
int program4[80] = {0};
int notes[25] = {131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523};
int debug[10] = {0};
bool nextCommand = false;
bool previousCommand = false;
bool nextCell = false;
bool previousCell = false;
bool programState = true;
bool deviceOn = false;
bool clearCells = true;
bool sound = true;
bool notCurrentProgram = false;
char ascii[45] = {'+', '-', '<', '>', '[', ']', ',', '.', '?', '!', ':', '~', '^', '=', '@', '*', '/', '\"', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};
char programName[] = "AAAAAA";
String programNames[] = {"PROGRAM1", "PROGRAM2", "PROGRAM3", "PROGRAM4"};
String devicePIN = "43345";
String inputPIN = "";
String debugOutput = "";
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
byte deleteCharacter[] = {
	0b00000,
	0b00111,
	0b01101,
	0b10010,
	0b01101,
	0b00111,
	0b00000,
	0b00000
};

LiquidCrystal_I2C lcd(0x27, 20, 4);

void(* resetFunc) (void) = 0;

void setup() {
    lcd.init();
    lcd.createChar(0, heart);
    lcd.createChar(1, checkMark);
    lcd.createChar(2, crossMark);
    lcd.createChar(3, apple);
    lcd.createChar(4, banana);
    lcd.createChar(5, entity);
    lcd.createChar(6, deleteCharacter);
    lcd.setCursor(0, 0);

    for (int i = 0; i < 80; i++) {
        cells[i] = 0;
        program[i] = 0;
        program3[i] = 0;
        program4[i] = 0;
        debug[i] = 0;
    }
}

void loop() {
    analogInputValue1 = analogRead(14);
    analogInputValue2 = analogRead(15);
    digitalInputValue1 = analogRead(16);
    digitalInputValue2 = analogRead(17);
    
    if ((analogInputValue1 > 767 || analogInputValue1 < 255) && programState) {
        lcd.noBlink();
        
        while (analogInputValue1 > 767 || analogInputValue1 < 255) {
            if (analogInputValue1 > 767) {
                if (currentCommand == 18) {
                    currentCommand = 0;
                } else {
                    currentCommand++;
                }
            } else {
                if (currentCommand == 0) {
                    currentCommand = 18;
                } else {
                    currentCommand--;
                }
            }
            
            lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));

            if (currentCommand == 18) {
                lcd.write(byte(6));
            } else {
                lcd.print(ascii[currentCommand]);
            }
            
            delay(500);
            analogInputValue1 = analogRead(14);
        }
        
        lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));

        if (program[currentProgramCell] == 0) {
            lcd.print(F(" "));
        } else {
            lcd.print(ascii[program[currentProgramCell] - 1]);
        }
        
        lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
        lcd.blink();
    }
    
    if (analogInputValue2 > 767 && programState) {
        nextCell = true;
    }

    if (analogInputValue2 < 255 && programState) {
        previousCell = true;
    }
    
    if (analogInputValue2 < 520 && nextCell && currentProgramCell != 80 && program[currentProgramCell] != 0 && deviceOn) {
        currentProgramCell++;
        lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
        nextCell = false;
    }

    if (analogInputValue2 > 500 && previousCell && currentProgramCell != 0 && deviceOn) {
        currentProgramCell--;
        lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
        previousCell = false;
    }
    
    if (digitalInputValue1 > 1000 && analogInputValue2 < 1000 && digitalInputDelay == 500 && currentProgramCell != 80 && programState && deviceOn) {
        if (currentCommand == 18) {
            program[currentProgramCell] = 0;

            for (int i = 0; i < 79; i++) {
                if (program[i] == 0) {
                    program[i] = program[i + 1];
                    program[i + 1] = 0;
                }
            }

            errorCell = currentProgramCell;
        } else {
            if (program[currentProgramCell] == 0) {
                program[currentProgramCell] = currentCommand + 1;
                lcd.print(ascii[currentCommand]);
                currentProgramCell++;
                lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
            } else {
                for (int i = 0; i < (80 - currentProgramCell); i++) {
                    program[80 - i] = program[79 - i];
                }

                program[currentProgramCell] = currentCommand + 1;
                errorCell = currentProgramCell;
            }
        }

        if (currentCommand == 18 || program[currentProgramCell] != 0) {
            for (int i = 0; i < 80; i++) {
                if (program[i] != 0) {
                    programLength = i;
                }
            }

            lcd.clear();
            
            if (program[0] != 0) {
                for (currentProgramCell = 0; currentProgramCell < (programLength + 1); currentProgramCell++) {
                    lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
                    lcd.print(ascii[program[currentProgramCell] - 1]);
                }
            }

            currentProgramCell = errorCell;
            lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
        }
        
        digitalInputDelay = 0;
    }
    
    if ((digitalInputValue2 == 0 && analogInputValue1 > 50 && analogInputValue2 > 500 && digitalInputDelay == 500 && deviceOn) || errorType != 0) {
        delay(500);
        digitalInputValue2 = analogRead(17);

        if (digitalInputValue2 == 0 && programState) {
            delay(500);
            lcd.clear();
            lcd.noBlink();
            digitalInputValue2 = analogRead(17);

            if (digitalInputValue2 == 0) {
                lcd.noBacklight();
                deviceOn = false;
            } else {
                digitalInputValue1 = 0;
                currentCommand = 0;
                lcd.setCursor(1, 0);
                lcd.print(F("Program Memory"));
                lcd.setCursor(1, 1);
                lcd.print(F("Sound"));
                lcd.setCursor(1, 2);
                lcd.print(F("About"));
                lcd.setCursor(1, 3);
                lcd.print(F("Reset"));
                lcd.setCursor(0, 0);
                lcd.print(F(">"));
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
                        lcd.print(F(" "));

                        if (currentCommand == 0) {
                            currentCommand = 3;
                        } else {
                            currentCommand--;
                        }
                        
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(">"));
                        delay(500);
                        previousCommand = false;
                    }

                    if (analogInputValue1 > 500 && nextCommand) {
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(" "));

                        if (currentCommand == 3) {
                            currentCommand = 0;
                        } else {
                            currentCommand++;
                        }
                        
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(">"));
                        delay(500);
                        nextCommand = false;
                    }
                }
                
                if (currentCommand == 1) {
                    goto sound;
                }

                if (currentCommand == 2) {
                    goto about;
                }
                
                if (currentCommand == 3) {
                    nextCell = true;
                } else {
                    nextCell = false;
                }
                
                if (inputPIN == "") {
                    digitalInputValue1 = 0;
                    currentCommand = 0;
                    currentCell = 0;
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print(F("PIN="));
                    lcd.setCursor(4, 0);
                    lcd.blink();
                    delay(500);

                    while (currentCell < 5) {
                        analogInputValue1 = analogRead(14);
                        digitalInputValue1 = analogRead(16);
                        
                        if (analogInputValue1 > 767 || analogInputValue1 < 255) {
                            lcd.noBlink();
                            
                            while (analogInputValue1 > 767 || analogInputValue1 < 255) {
                                if (analogInputValue1 > 767) {
                                    if (currentCommand == 9) {
                                        currentCommand = 0;
                                    } else {
                                        currentCommand++;
                                    }
                                } else {
                                    if (currentCommand == 0) {
                                        currentCommand == 9;
                                    } else {
                                        currentCommand--;
                                    }
                                }
                                
                                lcd.setCursor(currentCell + 4, 0);
                                lcd.print(currentCommand);
                                delay(500);
                                analogInputValue1 = analogRead(14);
                            }

                            lcd.setCursor(currentCell + 4, 0);
                            lcd.print(F(" "));
                            lcd.setCursor(currentCell + 4, 0);
                            lcd.blink();
                        }
                        
                        if (digitalInputValue1 > 1000 && analogInputValue1 < 520) {
                            inputPIN += String(currentCommand);
                            lcd.print(currentCommand);
                            currentCell++;
                            lcd.setCursor(currentCell + 4, 0);
                            delay(500);
                        }
                    }
                    
                    currentCommand = 0;
                    lcd.noBlink();
                }

                if (inputPIN != devicePIN) {
                    lcd.setCursor(0, 0);
                    lcd.print(F("ACCESS DENIED!"));
                    digitalInputValue1 = 0;
                    inputPIN = "";
                    delay(500);
                    
                    while (digitalInputValue1 < 1000) {
                        digitalInputValue1 = analogRead(16);
                    }

                    goto end;
                }

                if (nextCell) {
                    goto reset;
                }
                
                digitalInputValue1 = 0;
                delay(500);
                lcd.clear();
                
                for (int i = 0; i < 4; i++) {
                    lcd.setCursor(1, i);
                    lcd.print(programNames[i]);
                }

                lcd.setCursor(0, 0);
                lcd.print(F(">"));
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
                        lcd.print(F(" "));

                        if (currentCommand == 0) {
                            currentCommand = 3;
                        } else {
                            currentCommand--;
                        }
                        
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(">"));
                        delay(500);
                        previousCommand = false;
                    }

                    if (analogInputValue1 > 400 && nextCommand) {
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(" "));

                        if (currentCommand == 3) {
                            currentCommand = 0;
                        } else {
                            currentCommand++;
                        }
                        
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(">"));
                        delay(500);
                        nextCommand = false;
                    }
                }

                errorCell = currentCommand;
                digitalInputValue1 = 0;
                currentCommand = 0;
                lcd.clear();
                lcd.setCursor(1, 0);
                lcd.print(F("Save"));
                lcd.setCursor(1, 1);
                lcd.print(F("Load"));
                lcd.setCursor(1, 2);
                lcd.print(F("Rename"));
                lcd.setCursor(1, 3);
                lcd.print(F("Transmit"));
                lcd.setCursor(0, 0);
                lcd.print(F(">"));
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
                        lcd.print(F(" "));

                        if (currentCommand == 0) {
                            currentCommand = 3;
                        } else {
                            currentCommand--;
                        }
                        
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(">"));
                        delay(500);
                        previousCommand = false;
                    }

                    if (analogInputValue1 > 500 && nextCommand) {
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(" "));

                        if (currentCommand == 3) {
                            currentCommand = 0;
                        } else {
                            currentCommand++;
                        }
                        
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(">"));
                        delay(500);
                        nextCommand = false;
                    }
                }

                if (currentCommand == 0) {
                    if (errorCell == 0) {
                        for (int i = 0; i < 80; i++) {
                            program1[i] = program[i];
                        }
                    }

                    if (errorCell == 1) {
                        for (int i = 0; i < 80; i++) {
                            program2[i] = program[i];
                        }
                    }

                    if (errorCell == 2) {
                        for (int i = 0; i < 80; i++) {
                            program3[i] = program[i];
                        }
                    }

                    if (errorCell == 3) {
                        for (int i = 0; i < 80; i++) {
                            program4[i] = program[i];
                        }
                    }

                    for (int i = 0; i < 80; i++) {
                        program[i] = 0;
                    }
                }

                if (currentCommand == 1) {
                    if (errorCell == 0) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program1[i];
                        }
                    }

                    if (errorCell == 1) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program2[i];
                        }
                    }

                    if (errorCell == 2) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program3[i];
                        }
                    }

                    if (errorCell == 3) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program4[i];
                        }
                    }

                    currentProgram = errorCell;
                }

                if (currentCommand == 2) {
                    digitalInputValue1 = 0;
                    currentCommand = 0;
                    currentCell = 0;
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print(F("PROGRAM NAME="));
                    lcd.setCursor(13, 0);
                    lcd.blink();
                    delay(500);

                    while (currentCell < 6) {
                        analogInputValue1 = analogRead(14);
                        digitalInputValue1 = analogRead(16);
                        
                        if (analogInputValue1 > 767 || analogInputValue1 < 255) {
                            lcd.noBlink();
                            
                            while (analogInputValue1 > 767 || analogInputValue1 < 255) {
                                if (analogInputValue1 > 767) {
                                    if (currentCommand == 26) {
                                        currentCommand = 0;
                                    } else {
                                        currentCommand++;
                                    }
                                } else {
                                    if (currentCommand == 0) {
                                        currentCommand = 26;
                                    } else {
                                        currentCommand--;
                                    }
                                }
                                
                                lcd.setCursor(currentCell + 13, 0);
                                lcd.print(ascii[currentCommand + 18]);
                                delay(500);
                                analogInputValue1 = analogRead(14);
                            }

                            lcd.setCursor(currentCell + 13, 0);
                            lcd.print(F(" "));
                            lcd.setCursor(currentCell + 13, 0);
                            lcd.blink();
                        }
                        
                        if (digitalInputValue1 > 1000 && analogInputValue1 < 520) {
                            programName[currentCell] = ascii[currentCommand + 16];
                            lcd.print(ascii[currentCommand + 16]);
                            currentCell++;
                            lcd.setCursor(currentCell + 13, 0);
                            delay(500);
                        }
                    }

                    programNames[errorCell] = programName;
                    currentCommand = 0;
                }

                if (currentCommand == 3) {
                    Serial.begin(9600);

                    if (errorCell == 0) {
                        for (int i = 0; i < 80; i++) {
                            if (program1[i] != 0) {
                                programLength = i;
                            }
                        }

                        for (int i = 0; i < (programLength + 1); i++) {
                            Serial.print(ascii[program1[i] - 1]);
                        }
                    }

                    if (errorCell == 1) {
                        for (int i = 0; i < 80; i++) {
                            if (program2[i] != 0) {
                                programLength = i;
                            }
                        }

                        for (int i = 0; i < (programLength + 1); i++) {
                            Serial.print(ascii[program2[i] - 1]);
                        }
                    }

                    if (errorCell == 2) {
                        for (int i = 0; i < 80; i++) {
                            if (program3[i] != 0) {
                                programLength = i;
                            }
                        }

                        for (int i = 0; i < (programLength + 1); i++) {
                            Serial.print(ascii[program3[i] - 1]);
                        }
                    }

                    if (errorCell == 3) {
                        for (int i = 0; i < 80; i++) {
                            if (program4[i] != 0) {
                                programLength = i;
                            }
                        }

                        for (int i = 0; i < (programLength + 1); i++) {
                            Serial.print(ascii[program4[i] - 1]);
                        }
                    }

                    Serial.end();
                }

                goto end;

                sound:

                digitalInputValue1 = 0;
                currentCommand = 0;
                lcd.clear();
                lcd.setCursor(1, 0);
                lcd.print(F("On"));
                lcd.setCursor(1, 1);
                lcd.print(F("Off"));
                lcd.setCursor(0, 0);
                lcd.print(F(">"));

                if (sound) {
                    lcd.setCursor(4, 0);
                } else {
                    lcd.setCursor(5, 1);
                }

                lcd.print(F("(current)"));
                delay(500);
                
                while (digitalInputValue1 < 1000 || analogInputValue1 > 520) {
                    analogInputValue1 = analogRead(14);
                    digitalInputValue1 = analogRead(16);
                    
                    if (analogInputValue1 > 767 || analogInputValue1 < 255) {
                        nextCommand = true;
                    }
                    
                    if ((analogInputValue1 < 520 || analogInputValue1 > 500) && nextCommand) {
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(" "));
                        currentCommand = (currentCommand * -1) + 1;
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(">"));
                        delay(500);
                        nextCommand = false;
                    }
                }
                
                if (currentCommand == 0) {
                    sound = true;
                } else {
                    sound = false;
                }

                goto end;

                about:

                lcd.setCursor(0, 0);
                lcd.print(F("MIT License (c) 2023"));
                lcd.setCursor(0, 1);
                lcd.print(F("Alexander Gaggl gith"));
                lcd.setCursor(0, 2);
                lcd.print(F("ub.com/Coder-Dude10"));
                lcd.setCursor(0, 3);
                lcd.print(F("Press ENTER to exit."));
                digitalInputValue1 = 0;
                delay(500);
                
                while (digitalInputValue1 < 1000) {
                    digitalInputValue1 = analogRead(16);
                }

                goto end;

                reset:

                digitalInputValue1 = 0;
                currentCommand = 0;
                lcd.clear();
                lcd.setCursor(1, 0);
                lcd.print(F("Confirm"));
                lcd.setCursor(1, 1);
                lcd.print(F("Cancel"));
                lcd.setCursor(0, 0);
                lcd.print(F(">"));
                delay(500);
                
                while (digitalInputValue1 < 1000 || analogInputValue1 > 520) {
                    analogInputValue1 = analogRead(14);
                    digitalInputValue1 = analogRead(16);
                    
                    if (analogInputValue1 > 767 || analogInputValue1 < 255) {
                        nextCommand = true;
                    }
                    
                    if ((analogInputValue1 < 520 || analogInputValue1 > 500) && nextCommand) {
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(" "));
                        currentCommand = (currentCommand * -1) + 1;
                        lcd.setCursor(0, currentCommand);
                        lcd.print(F(">"));
                        delay(500);
                        nextCommand = false;
                    }
                }

                if (currentCommand == 0) {
                    resetFunc();
                }

                end:
                
                errorCell = programLength + 1;
                errorType = 1;
            }
        } else {
            currentProgramCell = 0;
            currentDebugCell = 0;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.blink();

            for (int i = 0; i < 80; i++) {
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
                    lcd.print(ascii[program[currentProgramCell] - 1]);
                }

                lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
                lcd.print(ascii[program[currentProgramCell] - 1]);
                
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

        for (int i = 0; i < 80; i++) {
            if (program[i] != 0) {
                programLength = i;
            }
        }
        
        if (program[0] != 0) {
            for (currentProgramCell = 0; currentProgramCell < programLength; currentProgramCell++) {
                lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
                lcd.print(ascii[program[currentProgramCell] - 1]);
            }

            lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
            lcd.print(ascii[program[currentProgramCell] - 1]);
        }

        lcd.blink();

        if (!(sound)) {
            goto startupSoundEnd;
        }
        
        tone(11, 15, 5.81);
        delay(200);
        tone(11, 14, 5.81);
        delay(25);
        tone(11, 184, 69.77);
        delay(150);
        tone(11, 14, 5.81);
        delay(35);
        tone(11, 2489, 69.77);
        delay(150);
        tone(11, 14, 5.81);
        delay(35);
        tone(11, 2217, 69.77);
        delay(150);
        tone(11, 15, 5.81);
        delay(100);
        tone(11, 13, 5.81);
        delay(100);
        tone(11, 14, 5.81);
        delay(200);
        tone(11, 2217, 151.16);
        delay(100);
        tone(11, 14, 5.81);
        delay(35);
        tone(11, 9, 5.81);
        delay(100);
        tone(11, 15, 5.81);

        startupSoundEnd:

        deviceOn = true;
        inputPIN = "";
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
            if (currentCell == 80) {
                errorType = 2;
            } else {
                currentCell++;
            }
        }

        if (program[currentProgramCell] == 5 && cells[currentCell] == 0) {
            bracketsPassed = 0;
            
            openBracketStart:
            
            while (program[currentProgramCell] != 6) {
                analogInputValue1 = analogRead(14);
                analogInputValue2 = analogRead(15);
                digitalInputValue2 = analogRead(17);
                
                if (digitalInputValue2 == 0 && analogInputValue1 > 50 && analogInputValue2 > 500) {
                    programState = true;
                    break;
                }
                
                if (currentProgramCell == programLength) {
                    errorType = 3;
                    break;
                } else {
                    currentProgramCell++;
                }

                if (program[currentProgramCell] == 5) {
                    bracketsPassed++;
                }
            }

            if (bracketsPassed != 0) {
                bracketsPassed--;
                currentProgramCell++;
                goto openBracketStart;
            }
        }

        if (program[currentProgramCell] == 6 && cells[currentCell] != 0) {
            bracketsPassed = 0;

            closedBracketStart:

            while (program[currentProgramCell] != 5) {
                analogInputValue1 = analogRead(14);
                analogInputValue2 = analogRead(15);
                digitalInputValue2 = analogRead(17);

                if (digitalInputValue2 == 0 && analogInputValue1 > 50 && analogInputValue2 > 500) {
                    programState = true;
                    break;
                }
                
                if (currentProgramCell == 0) {
                    errorType = 4;
                    break;
                } else {
                    currentProgramCell--;
                }

                if (program[currentProgramCell] == 6) {
                    bracketsPassed++;
                }
            }

            if (bracketsPassed != 0) {
                bracketsPassed--;
                currentProgramCell--;
                goto closedBracketStart;
            }
        }

        if (program[currentProgramCell] == 7) {
            if (cells[currentCell] == 0) {
                if (analogInputValue2 < 255) {
                    cells[currentCell] = 1;
                } else {
                    cells[currentCell] = 0;
                }

                if (analogInputValue2 > 767) {
                    cells[currentCell + 1] = 1;
                } else {
                    cells[currentCell + 1] = 0;
                }
                
                if (analogInputValue1 > 767) {
                    cells[currentCell + 2] = 1;
                } else {
                    cells[currentCell + 2] = 0;
                }
                
                if (analogInputValue1 < 255) {
                    cells[currentCell + 3] = 1;
                } else {
                    cells[currentCell + 3] = 0;
                }
                
                if (digitalInputValue1 > 1000 && analogInputValue2 < 1000) {
                    cells[currentCell + 4] = 1;
                } else {
                    cells[currentCell + 4] = 0;
                }
            } else {
                cells[currentCell] = round(analogInputValue2 / 100);
                cells[currentCell + 1] = round(analogInputValue1 / 100);

                if (digitalInputValue1 > 1000 && analogInputValue2 < 1000) {
                    cells[currentCell + 2] = 1;
                } else {
                    cells[currentCell + 2] = 0;
                }
            }
        }

        if (program[currentProgramCell] == 8) {
            if (cells[currentCell + 2] < 20) {
                lcd.setCursor(cells[currentCell + 2], cells[currentCell + 3]);
            } else {
                lcd.setCursor(cells[currentCell + 2] % 20, floor(cells[currentCell + 2] / 20));
            }

            if (cells[currentCell + 1] == 0) {
                lcd.print(cells[currentCell]);
            }
            
            if (cells[currentCell + 1] == 1) {
                if (cells[currentCell] < 0 || cells[currentCell] > 44) {
                    errorType = 5;
                } else {
                    lcd.print(ascii[cells[currentCell]]);
                }
            }
            
            if (cells[currentCell + 1] == 2) {
                if (cells[currentCell] < 0 || cells[currentCell] > 6) {
                    errorType = 5;
                } else {
                    lcd.write(byte(cells[currentCell]));
                }
            }

            if (cells[currentCell + 1] == 3) {
                lcd.clear();
            }
        }

        if (program[currentProgramCell] == 9) {
            cells[currentCell] = random(cells[currentCell] + 1);
        }

        if (program[currentProgramCell] == 10 && sound) {
            if (cells[currentCell] > -14 && cells[currentCell] < 12) {
                tone(11, notes[cells[currentCell] + 12], (cells[currentCell + 1] * 100));
            } else {
                errorType = 6;
            }
        }

        if (program[currentProgramCell] == 11) {
            Serial.begin(9600);
            
            if (cells[currentCell + 1] == 0) {
                Serial.print(cells[currentCell]);
            } else {
                Serial.print(ascii[cells[currentCell]]);
            }
            
            delay(500);
            Serial.end();
        }
        
        if (program[currentProgramCell] == 12) {
            delay(cells[currentCell] * 100);
        }

        if (program[currentProgramCell] == 13) {
            currentCell = cells[currentCell];
        }
        
        if (program[currentProgramCell] == 14) {
            cells[currentCell] = cells[cells[currentCell]];
        }

        if (program[currentProgramCell] == 15) {
            if (cells[currentCell] < 0 || cells[currentCell] > 3) {
                errorType = 7;
            } else {
                inputPIN = "";

                for (int i = 1; i < 6; i++) {
                    inputPIN += String(cells[currentCell + i]);
                }

                if (inputPIN == devicePIN) {
                    if (cells[currentCell] == 0) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program1[i];
                        }
                    }

                    if (cells[currentCell] == 1) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program2[i];
                        }
                    }

                    if (cells[currentCell] == 2) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program3[i];
                        }
                    }

                    if (cells[currentCell] == 3) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program4[i];
                        }
                    }
                    
                    for (int i = 0; i < 80; i++) {
                        if (program[i] != 0) {
                            programLength = i;
                        }
                    }
                    
                    errorCell = currentProgramCell;
                    notCurrentProgram = true;
                    currentProgramCell = -1;
                } else {
                    errorType = 8;
                }
            }
        }

        if (program[currentProgramCell] == 16) {
            clearCells = false;
        }

        if (program[currentProgramCell] == 17) {
            if (currentDebugCell == 10) {
                currentDebugCell = 0;
            }
            
            debug[currentDebugCell] = cells[currentCell];
            currentDebugCell++;
        }

        if (program[currentProgramCell] == 18) {
            debugOutput = String(debug[0]);
            
            for (int i = 1; i < currentDebugCell; i++) {
                debugOutput += ("," + String(debug[i]));
            }
            
            lcd.setCursor(0, 0);
            lcd.print(debugOutput);
        }
        
        if (errorType != 0) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("ERR: " + String(errorType));
            lcd.setCursor(1, 1);
            lcd.print(F("Goto"));
            lcd.setCursor(1, 2);
            lcd.print(F("Quit"));
            lcd.setCursor(0, 1);
            lcd.print(F(">"));
            currentCommand = 0;
            
            while (digitalInputValue1 < 1000) {
                analogInputValue1 = analogRead(14);
                digitalInputValue1 = analogRead(16);
                
                if (analogInputValue1 > 767 || analogInputValue1 < 255) {
                    nextCommand = true;
                }
                
                if ((analogInputValue1 < 520 || analogInputValue1 > 500) && nextCommand) {
                    lcd.setCursor(0, currentCommand + 1);
                    lcd.print(F(" "));
                    currentCommand = (currentCommand * -1) + 1;
                    lcd.setCursor(0, currentCommand + 1);
                    lcd.print(F(">"));
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

        if (currentProgramCell == programLength && notCurrentProgram) {
            if (currentProgram == 0) {
                for (int i = 0; i < 80; i++) {
                    program[i] = program1[i];
                }
            }

            if (currentProgram == 1) {
                for (int i = 0; i < 80; i++) {
                    program[i] = program2[i];
                }
            }

            if (currentProgram == 2) {
                for (int i = 0; i < 80; i++) {
                    program[i] = program3[i];
                }
            }

            if (currentProgram == 3) {
                for (int i = 0; i < 80; i++) {
                    program[i] = program4[i];
                }
            }

            for (int i = 0; i < 80; i++) {
                if (program[i] != 0) {
                    programLength = i;
                }
            }

            currentProgramCell = errorCell;
            notCurrentProgram = false;
        }

        currentProgramCell++;
    }
}
