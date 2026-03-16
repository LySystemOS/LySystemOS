#include <LySys/LySystem.h>
#include <LySys/types.h>
#include <asm/io.h>
#include <LySys/video.h>

#define kbd_ready() while (inb(0x64) & 2)

#define KEY_ESC 0x0001
#define KEY_1 0x0002
#define KEY_2 0x0003
#define KEY_3 0x0004
#define KEY_4 0x0005
#define KEY_5 0x0006
#define KEY_6 0x0007
#define KEY_7 0x0008
#define KEY_8 0x0009
#define KEY_9 0x000A
#define KEY_0 0x000B
#define KEY_MINUS 0x000C
#define KEY_EQUAL 0x000D
#define KEY_BACKSPACE 0x000E
#define KEY_TAB 0x000F
#define KEY_Q 0x0010
#define KEY_W 0x0011
#define KEY_E 0x0012
#define KEY_R 0x0013
#define KEY_T 0x0014
#define KEY_Y 0x0015
#define KEY_U 0x0016
#define KEY_I 0x0017
#define KEY_O 0x0018
#define KEY_P 0x0019
#define KEY_LEFT_BRACKET  0x001A
#define KEY_RIGHT_BRACKET 0x001B
#define KEY_ENTER 0x001C
#define KEY_LCTRL 0x001D
#define KEY_A 0x001E
#define KEY_S 0x001F
#define KEY_D 0x0020
#define KEY_F 0x0021
#define KEY_G 0x0022
#define KEY_H 0x0023
#define KEY_J 0x0024
#define KEY_K 0x0025
#define KEY_L 0x0026
#define KEY_SEMICOLON 0x0027
#define KEY_QUOTE     0x0028
#define KEY_Z 0x002C
#define KEY_X 0x002D
#define KEY_C 0x002E
#define KEY_V 0x002F
#define KEY_B 0x0030
#define KEY_N 0x0031
#define KEY_M 0x0032
#define KEY_COMMA 0x0033
#define KEY_DOT 0x0034
#define KEY_SLASH 0x0035
#define KEY_LALT 0x0038
#define KEY_SPACE 0x0039
#define KEY_CAPSLOCK 0x003A

#define KEY_LSHIFT 0x2A
#define KEY_RSHIFT 0x36
#define KEY_LSHIFT_REL 0xAA
#define KEY_RSHIFT_REL 0xB6

#define KEY_UP 0xE048
#define KEY_LEFT 0xE04B
#define KEY_RIGHT 0xE04D
#define KEY_DOWN 0xE050

bool CapslockActive = false;
bool ShiftActive = false;

void update_kbd_leds() {
    kbd_ready();
    outb(0x60, 0xED);
    kbd_ready();
    outb(0x60, CapslockActive ? 0x04 : 0x00);
}

char keyboard_transform(uint16_t code) {
    uint8_t raw_code = (uint8_t)(code & 0xFF);

    if (raw_code == KEY_LSHIFT || raw_code == KEY_RSHIFT) {
        ShiftActive = true;
        return '\0';
    }
    
    if (raw_code == KEY_LSHIFT_REL || raw_code == KEY_RSHIFT_REL) {
        ShiftActive = false;
        return '\0';
    }

    if (raw_code == KEY_CAPSLOCK) {
        CapslockActive = !CapslockActive;
        update_kbd_leds();
        return '\0';
    }

    if ((code & 0x80) && (code & 0xFF00) != 0xE000) {
        return '\0';
    }

    bool upper = (CapslockActive != ShiftActive);

    switch (code) {
        case KEY_1: return ShiftActive ? '!' : '1';
        case KEY_2: return ShiftActive ? '@' : '2';
        case KEY_3: return ShiftActive ? '#' : '3';
        case KEY_4: return ShiftActive ? '$' : '4';
        case KEY_5: return ShiftActive ? '%' : '5';
        case KEY_6: return ShiftActive ? '^' : '6';
        case KEY_7: return ShiftActive ? '&' : '7';
        case KEY_8: return ShiftActive ? '*' : '8';
        case KEY_9: return ShiftActive ? '(' : '9';
        case KEY_0: return ShiftActive ? ')' : '0';
        case KEY_MINUS: return ShiftActive ? '_' : '-';
        case KEY_EQUAL: return ShiftActive ? '+' : '=';

        case KEY_A: return upper ? 'A' : 'a';
        case KEY_B: return upper ? 'B' : 'b';
        case KEY_C: return upper ? 'C' : 'c';
        case KEY_D: return upper ? 'D' : 'd';
        case KEY_E: return upper ? 'E' : 'e';
        case KEY_F: return upper ? 'F' : 'f';
        case KEY_G: return upper ? 'G' : 'g';
        case KEY_H: return upper ? 'H' : 'h';
        case KEY_I: return upper ? 'I' : 'i';
        case KEY_J: return upper ? 'J' : 'j';
        case KEY_K: return upper ? 'K' : 'k';
        case KEY_L: return upper ? 'L' : 'l';
        case KEY_SEMICOLON: return ShiftActive ? ':' : ';';
        case KEY_QUOTE:     return ShiftActive ? '"' : '\'';
        case KEY_M: return upper ? 'M' : 'm';
        case KEY_N: return upper ? 'N' : 'n';
        case KEY_O: return upper ? 'O' : 'o';
        case KEY_P: return upper ? 'P' : 'p';
        case KEY_LEFT_BRACKET: return ShiftActive ? '[' : '{';
        case KEY_RIGHT_BRACKET: return ShiftActive ? ']' : '}';
        case KEY_Q: return upper ? 'Q' : 'q';
        case KEY_R: return upper ? 'R' : 'r';
        case KEY_S: return upper ? 'S' : 's';
        case KEY_T: return upper ? 'T' : 't';
        case KEY_U: return upper ? 'U' : 'u';
        case KEY_V: return upper ? 'V' : 'v';
        case KEY_W: return upper ? 'W' : 'w';
        case KEY_X: return upper ? 'X' : 'x';
        case KEY_Y: return upper ? 'Y' : 'y';
        case KEY_Z: return upper ? 'Z' : 'z';

        case KEY_TAB: return '\t';
        case KEY_COMMA: return ShiftActive ? '<' : ',';
        case KEY_DOT: return ShiftActive ? '>' : '.';
        case KEY_SLASH: return ShiftActive ? '?' : '/';
        case KEY_SPACE: return ' ';
        case KEY_ENTER: return '\n';
        case KEY_BACKSPACE: return '\b';
        case KEY_ESC: clear_screen(0x000000); ListTask(); '\0';

        default: return '\0';
    }
}