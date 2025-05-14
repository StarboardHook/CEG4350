#include "../include/io.h"

void initkeymap()
{
    keymap[0x1E] = 'a';
    keymap[0x30] = 'b';
    keymap[0x2E] = 'c';
    keymap[0x20] = 'd';
    keymap[0x12] = 'e';
    keymap[0x21] = 'f';
    keymap[0x22] = 'g';
    keymap[0x23] = 'h';
    keymap[0x17] = 'i';
    keymap[0x24] = 'j';
    keymap[0x25] = 'k';
    keymap[0x26] = 'l';
    keymap[0x32] = 'm';
    keymap[0x31] = 'n';
    keymap[0x18] = 'o';
    keymap[0x19] = 'p';
    keymap[0x10] = 'q';
    keymap[0x13] = 'r';
    keymap[0x1F] = 's';
    keymap[0x14] = 't';
    keymap[0x16] = 'u';
    keymap[0x2F] = 'v';
    keymap[0x11] = 'w';
    keymap[0x2D] = 'x';
    keymap[0x15] = 'y';
    keymap[0x2C] = 'z';

    keymap[0x0B] = '0';
    keymap[0x02] = '1';
    keymap[0x03] = '2';
    keymap[0x04] = '3';
    keymap[0x05] = '4';
    keymap[0x06] = '5';
    keymap[0x07] = '6';
    keymap[0x08] = '7';
    keymap[0x09] = '8';
    keymap[0x0A] = '9';

    keymap[0x39] = ' ';   // Space
    keymap[0x1C] = '\n';  // Enter
}