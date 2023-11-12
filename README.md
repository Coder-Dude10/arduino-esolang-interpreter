# Arduino Esolang Interpreter
An IDE and interpreter of esolang syntax for Arduino boards.
# Brainfk++ Command Set
| Command | Definition | Parameters |
| --- | --- | --- |
| + | Increment current cell value by 1. | - |
| - | Decrement current cell value by 1. | - |
| < | Move pointer left by 1 cell.| - |
| > | Move pointer right by 1 cell.| - |
| [ | If current cell value == 0, goto cell after corresponding ] command. | currentCellValue |
| ] | If current cell value != 0, goto cell after corresponding [ command. | currentCellValue |
| , | Take input and set current and following cells to either (left, right, up, down, enter) or (analogInput1, analogInput2, enter). | mode |
| . | Output integer or character at character (x, y) on display, or clear the display. | currentCellValue, mode, x, y |
| ? | Set current cell value to random integer from 0 to (maximumIntegerValue). | maximumIntegerValue |
| ! | Output value of a cell as a tone with a frequency of (frequency * 100) Hz and a duration of (duration / 10) seconds. | frequency, duration |
| : | Output integer or character over serial. | mode |
| ~ | Pause for (duration / 10) seconds. | duration |
| ^ | Set pointer to value of current cell. | currentCellValue |
| = | Set value of current cell to value of cell (currentCellValue). |  currentCellValue |
| @ | If correct PIN is given, execute program (programIndexNumber). | programIndexNumber, PINDigit1, PINDigit2, PINDigit3, PINDigit4, PINDigit5 |
| * | Do not clear cell memory when program is finished. | - |