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
int bufferCell = 0;
int cells[80];
int program[80];
int program0[80];
int program1[80];
int program2[80];
int program3[80];
int notes[25] = {131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523};
int powBase2[9] = {256, 128, 64, 32, 16, 8, 4, 2, 1};
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
bool programConcatenation = false;
char inputs[5] = {'l', 'r', 'u', 'd', 'e'};
char commands[22] = {'+', '-', '<', '>', '[', ']', ',', '.', '?', '!', ':', '~', '^', '=', '#', '|', '@', '*', ';', '_', '/', '\"'};
char programName[] = "AAAAAA";
char charBuffer = 'A';
String programNames[] = {"PROGRAM0", "PROGRAM1", "PROGRAM2", "PROGRAM3"};
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
byte deleteChar[] = {
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

char ascii(int value) {
    if (value < 22) {
        return commands[value];
    } else {
        if (value == 48) {
            return ' ';
        } else {
            return char(value + 43);
        }
    }
}

int readCell(int index) {
    if (index < 80) {
        return cells[index];
    }

    if (index > 79 && index < 160) {
        return program0[index - 80];
    }

    if (index > 159 && index < 240) {
        return program1[index - 160];
    }

    if (index > 239 && index < 320) {
        return program2[index - 240];
    }

    if (index > 319) {
        return program3[index - 320];
    }
}

void writeCell(int index, int value, int multiplier) {
    if (index < 80) {
        cells[index] = ((cells[index] * multiplier) + value);
    }

    if (index > 79 && index < 160) {
        program0[index - 80] = ((program0[index - 80] * multiplier) + value);
    }

    if (index > 159 && index < 240) {
        program1[index - 160] = ((program1[index - 160] * multiplier) + value);
    }

    if (index > 239 && index < 320) {
        program2[index - 240] = ((program2[index - 240] * multiplier) + value);
    }

    if (index > 319) {
        program3[index - 320] = ((program3[index - 320] * multiplier) + value);
    }
}

void(* resetFunc) (void) = 0;

void setup() {
    lcd.init();
    lcd.createChar(0, heart);
    lcd.createChar(1, checkMark);
    lcd.createChar(2, crossMark);
    lcd.createChar(3, apple);
    lcd.createChar(4, banana);
    lcd.createChar(5, entity);
    lcd.createChar(6, deleteChar);
    lcd.setCursor(0, 0);
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
                if (currentCommand == 22) {
                    currentCommand = 0;
                } else {
                    currentCommand++;
                }
            } else {
                if (currentCommand == 0) {
                    currentCommand = 22;
                } else {
                    currentCommand--;
                }
            }

            lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));

            if (currentCommand == 22) {
                lcd.write(byte(6));
            } else {
                lcd.print(commands[currentCommand]);
            }

            delay(500);
            analogInputValue1 = analogRead(14);
        }

        lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));

        if (program[currentProgramCell] == 0) {
            lcd.print(F(" "));
        } else {
            lcd.print(ascii(program[currentProgramCell] - 1));
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
        if (currentCommand == 22) {
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
                lcd.print(ascii(currentCommand));
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

        if (currentCommand == 22 || program[currentProgramCell] != 0) {
            for (int i = 0; i < 80; i++) {
                if (program[i] != 0) {
                    programLength = i;
                }
            }

            lcd.clear();

            if (program[0] != 0) {
                for (currentProgramCell = 0; currentProgramCell < (programLength + 1); currentProgramCell++) {
                    lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
                    lcd.print(ascii(program[currentProgramCell] - 1));
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
                lcd.print(F("Receive/Transmit"));
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
                            program0[i] = program[i];
                        }
                    }

                    if (errorCell == 1) {
                        for (int i = 0; i < 80; i++) {
                            program1[i] = program[i];
                        }
                    }

                    if (errorCell == 2) {
                        for (int i = 0; i < 80; i++) {
                            program2[i] = program[i];
                        }
                    }

                    if (errorCell == 3) {
                        for (int i = 0; i < 80; i++) {
                            program3[i] = program[i];
                        }
                    }

                    for (int i = 0; i < 80; i++) {
                        program[i] = 0;
                    }
                }

                if (currentCommand == 1) {
                    if (errorCell == 0) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program0[i];
                        }
                    }

                    if (errorCell == 1) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program1[i];
                        }
                    }

                    if (errorCell == 2) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program2[i];
                        }
                    }

                    if (errorCell == 3) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program3[i];
                        }
                    }

                    for (int i = 0; i < 80; i++) {
                        if (program[i] != 0) {
                            programLength = i;
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
                                lcd.print(ascii(currentCommand + 18));
                                delay(500);
                                analogInputValue1 = analogRead(14);
                            }

                            lcd.setCursor(currentCell + 13, 0);
                            lcd.print(F(" "));
                            lcd.setCursor(currentCell + 13, 0);
                            lcd.blink();
                        }

                        if (digitalInputValue1 > 1000 && analogInputValue1 < 520) {
                            programName[currentCell] = ascii(currentCommand + 18);
                            lcd.print(ascii(currentCommand + 18));
                            currentCell++;
                            lcd.setCursor(currentCell + 13, 0);
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
                    lcd.print(F("Receive"));
                    lcd.setCursor(1, 1);
                    lcd.print(F("Transmit"));
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
                    
                    Serial.begin(9600);
                    
                    if (currentCommand == 0) {
                        goto receive;
                    } else {
                        goto transmit;
                    }
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

                digitalInputValue1 = 0;
                lcd.setCursor(0, 0);
                lcd.print(F("MIT License (c) 2023"));
                lcd.setCursor(0, 1);
                lcd.print(F("Alexander Gaggl gith"));
                lcd.setCursor(0, 2);
                lcd.print(F("ub.com/Coder-Dude10"));
                lcd.setCursor(0, 3);
                lcd.print(F("Press ENTER to exit."));
                delay(1000);
                
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

                goto end;

                receive:

                if (errorCell == 0) {
                    charBuffer = '+';
                    currentDebugCell = 0;

                    if (errorCell == 0) {
                        memset(program0, 0, sizeof(program0));
                    }

                    if (errorCell == 1) {
                        memset(program1, 0, sizeof(program1));
                    }

                    if (errorCell == 2) {
                        memset(program2, 0, sizeof(program2));
                    }

                    if (errorCell == 3) {
                        memset(program3, 0, sizeof(program3));
                    }
                    
                    while (charBuffer != 'a') {
                        Serial.print(currentDebugCell);
                        Serial.print('4');
                        while (Serial.available() == 0);
                        charBuffer = Serial.read();
                        
                        for (int i = 0; i < 22; i++) {
                            if (commands[i] == charBuffer) {
                                if (errorCell == 0) {
                                    program0[currentDebugCell] = (i + 1);
                                }

                                if (errorCell == 1) {
                                    program1[currentDebugCell] = (i + 1);
                                }

                                if (errorCell == 2) {
                                    program2[currentDebugCell] = (i + 1);
                                }

                                if (errorCell == 3) {
                                    program3[currentDebugCell] = (i + 1);
                                }
                            }
                        }

                        currentDebugCell++;
                    }
                }

                Serial.end();

                goto end;

                transmit:

                if (errorCell == 0) {
                    for (int i = 0; i < 80; i++) {
                        if (program0[i] != 0) {
                            programLength = i;
                        }
                    }
                    
                    for (int i = 0; i < (programLength + 1); i++) {
                        Serial.print(ascii(program0[i] - 1));
                        Serial.print('1');
                    }
                }
                
                if (errorCell == 1) {
                    for (int i = 0; i < 80; i++) {
                        if (program1[i] != 0) {
                            programLength = i;
                        }
                    }
                    
                    for (int i = 0; i < (programLength + 1); i++) {
                        Serial.print(ascii(program1[i] - 1));
                        Serial.print('1');
                    }
                }
                
                if (errorCell == 2) {
                    for (int i = 0; i < 80; i++) {
                        if (program2[i] != 0) {
                            programLength = i;
                        }
                    }
                    
                    for (int i = 0; i < (programLength + 1); i++) {
                        Serial.print(ascii(program2[i] - 1));
                        Serial.print('1');
                    }
                }
                
                if (errorCell == 3) {
                    for (int i = 0; i < 80; i++) {
                        if (program3[i] != 0) {
                            programLength = i;
                        }
                    }
                    
                    for (int i = 0; i < (programLength + 1); i++) {
                        Serial.print(ascii(program3[i] - 1));
                        Serial.print('1');
                    }
                }
                
                Serial.end();

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
                }
            }

            if (clearCells && !(programState)) {
                memset(cells, 0, sizeof(cells));
            }

            clearCells = true;

            if (errorType == 0) {
            programState = !(programState);
            }

            if (programState && program[0] != 0) {
                for (currentProgramCell = 0; currentProgramCell < programLength; currentProgramCell++) {
                    lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
                    lcd.print(ascii(program[currentProgramCell] - 1));
                }

                lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
                lcd.print(ascii(program[currentProgramCell] - 1));

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
                lcd.print(ascii(program[currentProgramCell] - 1));
            }

            lcd.setCursor(currentProgramCell % 20, floor(currentProgramCell / 20));
            lcd.print(ascii(program[currentProgramCell] - 1));
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
            writeCell(currentCell, 1, 1);
        }

        if (program[currentProgramCell] == 2) {
            writeCell(currentCell, -1, 1);
        }

        if (program[currentProgramCell] == 3) {
            if (currentCell > 0) {
                currentCell--;
            } else {
                errorType = 1;
            }
        }

        if (program[currentProgramCell] == 4) {
            if (currentCell < 80 || (currentCell < 400 && programConcatenation)) {
                currentCell++;
            } else {
                errorType = 2;
            }
        }

        if (program[currentProgramCell] == 5 && readCell(currentCell) == 0) {
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

        if (program[currentProgramCell] == 6 && readCell(currentCell) != 0) {
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
            if (readCell(currentCell) == 0) {
                if (analogInputValue2 < 255) {
                    writeCell(currentCell, 1, 0);
                } else {
                    writeCell(currentCell, 0, 0);
                }

                if (analogInputValue2 > 767) {
                    writeCell((currentCell + 1), 1, 0);
                } else {
                    writeCell((currentCell + 1), 0, 0);
                }

                if (analogInputValue1 > 767) {
                    writeCell((currentCell + 2), 1, 0);
                } else {
                    writeCell((currentCell + 2), 0, 0);
                }

                if (analogInputValue1 < 255) {
                    writeCell((currentCell + 3), 1, 0);
                } else {
                    writeCell((currentCell + 3), 0, 0);
                }

                if (digitalInputValue1 > 1000 && analogInputValue2 < 1000) {
                    writeCell((currentCell + 4), 1, 0);
                } else {
                    writeCell((currentCell + 4), 0, 0);
                }
            } else {
                writeCell(currentCell, round(analogInputValue2 / 100), 0);
                writeCell((currentCell + 1), round(analogInputValue1 / 100), 0);

                if (digitalInputValue1 > 1000 && analogInputValue2 < 1000) {
                    writeCell((currentCell + 2), 1, 0);
                } else {
                    writeCell((currentCell + 2), 0, 0);
                }
            }
        }

        if (program[currentProgramCell] == 8) {
            if (readCell(currentCell + 2) < 20) {
                lcd.setCursor(readCell(currentCell + 2), readCell(currentCell + 3));
            } else {
                lcd.setCursor(readCell(currentCell + 2) % 20, floor(readCell(currentCell + 2) / 20));
            }

            if (readCell(currentCell + 1) == 0) {
                lcd.print(readCell(currentCell));
            }

            if (readCell(currentCell + 1) == 1) {
                if (readCell(currentCell) < 0 || readCell(currentCell) > 44) {
                    errorType = 5;
                } else {
                    lcd.print(ascii(readCell(currentCell)));
                }
            }

            if (readCell(currentCell + 1) == 2) {
                if (readCell(currentCell) < 0 || readCell(currentCell) > 5) {
                    errorType = 5;
                } else {
                    lcd.write(byte(readCell(currentCell)));
                }
            }

            if (readCell(currentCell + 1) == 3) {
                lcd.clear();
            }
        }

        if (program[currentProgramCell] == 9) {
            writeCell(currentCell, random(readCell(currentCell) + 1), 0);
        }

        if (program[currentProgramCell] == 10 && sound) {
            if (readCell(currentCell) > -14 && readCell(currentCell) < 12) {
                tone(11, notes[readCell(currentCell) + 12], (readCell(currentCell + 1) * 100));
            } else {
                errorType = 6;
            }
        }

        if (program[currentProgramCell] == 11) {
            Serial.begin(9600);

            if (readCell(currentCell + 1) == 0) {
                Serial.print(readCell(currentCell));
            }

            if (readCell(currentCell + 1) == 1) {
                Serial.print(ascii(readCell(currentCell)));
            }

            if (readCell(currentCell + 1) == 2) {
                Serial.print(char(readCell(currentCell) + 128));
            }

            Serial.print(readCell(currentCell + 2));

            if (readCell(currentCell + 3) == 1) {
                charBuffer = 'a';

                while (charBuffer == 'a') {
                    if (Serial.available() > 0) {
                        charBuffer = Serial.read();

                        if (int(charBuffer) > 47 && int(charBuffer) < 58) {
                            writeCell(currentCell, (int(charBuffer) - 48), 0);
                            writeCell((currentCell + 1), 0, 0);
                        } else {
                            if (int(charBuffer) > 96 && int(charBuffer) < 123) {
                                for (int i = 0; i < 5; i++) {
                                    if (inputs[i] == charBuffer) {
                                        writeCell((currentCell + i), 1, 0);
                                    } else {
                                        writeCell((currentCell + i), 0, 0);
                                    }
                                }
                            } else {
                                for (int i = 0; i < 45; i++) {
                                    if (ascii(i) == charBuffer) {
                                        writeCell(currentCell, i, 0);
                                        writeCell((currentCell + 1), 1, 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Serial.end();
        }

        if (program[currentProgramCell] == 12) {
            delay(readCell(currentCell) * 100);
        }

        if (program[currentProgramCell] == 13) {
            if (readCell(currentCell) > -1) {
                if (readCell(currentCell) < 80 || (readCell(currentCell) < 400 && programConcatenation)) {
                    currentCell = readCell(currentCell);
                } else {
                    errorType = 2;
                }
            } else {
                errorType = 1;
            }
        }

        if (program[currentProgramCell] == 14) {
            if (readCell(currentCell + 1) == 0) {
                writeCell(currentCell, readCell(readCell(currentCell)), 0);
            } else {
                errorCell = 0;

                for (int i = 0; i < 9; i++) {
                    errorCell += (readCell(currentCell + (i + 2)) * powBase2[i]);
                }

                writeCell(currentCell, errorCell, 0);
            }
        }

        if (program[currentProgramCell] == 15) {
            if (readCell(currentCell) == 0) {
                writeCell(currentCell, 1, 0);
            } else {
                writeCell(currentCell, 0, 0);
            }
        }

        if (program[currentProgramCell] == 16) {
            if (readCell(currentCell) < 0) {
                writeCell(currentCell, 0, 0);
            } else {
                writeCell(currentCell, 1, 0);
            }
        }

        if (program[currentProgramCell] == 17) {
            if (inputPIN == devicePIN) {
                if (readCell(currentCell) > -1 && readCell(currentCell) < 4) {
                    if (readCell(currentCell) == 0) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program0[i];
                        }
                    }

                    if (readCell(currentCell) == 1) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program1[i];
                        }
                    }

                    if (readCell(currentCell) == 2) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program2[i];
                        }
                    }

                    if (readCell(currentCell) == 3) {
                        for (int i = 0; i < 80; i++) {
                            program[i] = program3[i];
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
                    errorType = 7;
                }
            } else {
                errorType = 8;
            }
        }

        if (program[currentProgramCell] == 18) {
            if (readCell(currentCell) == 0) {
                if (inputPIN == devicePIN) {
                    programConcatenation = true;
                } else {
                    errorType = 8;
                }
            } else {
                clearCells = false;
            }
        }

        if (program[currentProgramCell] == 19) {
            bufferCell = readCell(currentCell);
        }

        if (program[currentProgramCell] == 20) {
            writeCell(currentCell, bufferCell, 0);
        }

        if (program[currentProgramCell] == 21) {
            debug[currentDebugCell] = readCell(currentCell);

            if (currentDebugCell == 9) {
                currentDebugCell = 0;
            } else {
                currentDebugCell++;
            }
        }

        if (program[currentProgramCell] == 22) {
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
                    program[i] = program0[i];
                }
            }

            if (currentProgram == 1) {
                for (int i = 0; i < 80; i++) {
                    program[i] = program1[i];
                }
            }

            if (currentProgram == 2) {
                for (int i = 0; i < 80; i++) {
                    program[i] = program2[i];
                }
            }

            if (currentProgram == 3) {
                for (int i = 0; i < 80; i++) {
                    program[i] = program3[i];
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
