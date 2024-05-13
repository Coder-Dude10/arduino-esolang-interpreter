# Arduino Esolang Interpreter
An IDE and interpreter of Brainfk++ for Arduino boards. Try it out in [Wokwi](https://wokwi.com/projects/396988117726154753) (🌐Web-based simulator).
# Brainfk++ Command Set
| Command | Description | Parameters |
| --- | --- | --- |
| + | Increment current cell value by 1. | - |
| - | Decrement current cell value by 1. | - |
| < | Move pointer left by 1 cell.| - |
| > | Move pointer right by 1 cell.| - |
| [ | If **currentCellValue** == 0, goto cell after corresponding ']' command. | currentCellValue |
| ] | If **currentCellValue** != 0, goto cell after corresponding '[' command. | currentCellValue |
| , | Take input and set current and following cells to either (left, right, up, down, enter) (**mode** == 0) or (analogInput1, analogInput2, enter) (**mode** == 1). | mode |
| . | Output **currentCellValue** as an integer (**mode** == 0), character (**mode** == 1), or custom character (**mode** == 2) at character (**x**, **y**) on display, or clear the display (**mode** == 3). | currentCellValue, mode, x, y |
| ? | Set current cell value to random integer from 0 to **maximumIntegerValue**. | maximumIntegerValue |
| ! | Play a tone with a note value of **note** and a duration of (**duration** / 10) seconds. | note, duration |
| : | Output **currentCellValue** as an integer (**mode** == 0), character (**mode** == 1), or custom character (**mode** == 2) over serial, output **additionalValue** as an integer, and wait for response (integer, character, or input) if **expectResponse** == 1. | currentCellValue, mode, additionalValue, expectResponse |
| ~ | Pause for (**duration** / 10) seconds. | duration |
| ^ | Set cell pointer to **currentCellValue**. | currentCellValue |
| = | Set value of current cell to value of cell at **cellIndexNumber** (**mode** == 0) or decimal conversion of concatenated booleans (**mode** == 1). | cellIndexNumber, mode, bool8, bool7, bool6, bool5, bool4, bool3, bool2, bool1, bool0 |
| # | Set the value of the current cell to either 0 (**currentCellValue** != 0) or 1 (**currentCellValue** == 0). | currentCellValue |
| ▏| Set the value of the current cell to either 0 (**currentCellValue** < 0) or 1 (**currentCellValue** > -1). | currentCellValue |
| @ | If global PIN has been set and is correct, execute program at **programIndexNumber**. | programIndexNumber |
| * | Concatenate cell array and program arrays (**mode** == 0) or do not clear cell memory when program is finished (**mode** == 1). | mode |
| ; | Set the value of the buffer cell to **currentCellValue**. | currentCellValue |
| _ | Set the value of the current cell to the value of the buffer cell. | - |
| / | Push **currentCellValue** to debug array. | currentCellValue |
| " | Output debug array at character (0, 0). | - |
# Brainfk++ Character Set
![Brainfk++ Character Set](https://github.com/Coder-Dude10/arduino-esolang-interpreter/blob/main/Brainfk%2B%2B%20Character%20Set.png?raw=true)
# Brainfk++ Program Concatenation
![Brainfk++ Program Concatenation](https://github.com/Coder-Dude10/arduino-esolang-interpreter/blob/main/Brainfk%2B%2B%20Program%20Concatenation.png?raw=true)
# Brainfk++ Error Codes
| Error | Description |
| --- | --- |
| 1 | Minimum of cell memory reached. |
| 2 | Maxiumum of cell memory reached. |
| 3 | Corresponding ']' command not found. |
| 4 | Corresponding '[' command not found. |
| 5 | Invalid character value. |
| 6 | Invalid note value. |
| 7 | Invalid program index value. |
| 8 | Access denied. |
