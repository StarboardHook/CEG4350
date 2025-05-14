#include "../include/io.h"
#include "./types.h"

// Track the current cursor's row and column
volatile int cursorCol = 0;
volatile int cursorRow = 0;

// Define a keymap to convert keyboard scancodes to ASCII
char keymap[128] = {};

// C version of assembly I/O port instructions
// Allows for reading and writing with I/O
// The keyboard status port is 0x64
// The keyboard data port is 0x60
// More info here:
// https://wiki.osdev.org/I/O_Ports
// https://wiki.osdev.org/Port_IO
// https://bochs.sourceforge.io/techspec/PORTS.LST

// outb (out byte) - write an 8-bit value to an I/O port address (16-bit)
void outb(uint16 port, uint8 value)
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
	return;
}

// outw (out word) - write an 16-bit value to an I/O port address (16-bit)
void outw(uint16 port, uint16 value)
{
    asm volatile ("outw %1, %0" : : "dN" (port), "a" (value));
	return;
}

// inb (in byte) - read an 8-bit value from an I/O port address (16-bit)
uint8 inb(uint16 port)
{
   uint8 ret;
   asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

// inw (in word) - read an 16-bit value from an I/O port address (16-bit)
uint16 inw(uint16 port)
{
   uint16 ret;
   asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

char getchar(){
    uint8 status;
    uint8 scancode;

    while(1){
        status = inb(0x64); // Get keyboard Status
        if(status & 0x01){ // Check if ready
            scancode = inb(0x60);
            if(!(scancode & 0x80)){ // Check if key pressed
                return keymap[scancode];
            }
        }
    }
}

void scanf(char string[]){
    int i = 0;
    char character;
    while (i < 99){ // Limiting to 99 characters. NULL terminator is 100th character
        character = getchar();
        if (character == '\n'){ // Check if character is 'enter' key
            break; // Escape while loop
        }
        string[i] = character; // Place character in string
        putchar(character); // print to screen.
        i++;
    }
    string[i] = '\0'; // Adding null character to the end.
}

// Setting the cursor does not display anything visually
// Setting the cursor is simply used by putchar() to find where to print next
// This can also be set independently of putchar() to print at any x, y coordinate on the screen
void setcursor(int x, int y)
{	
    cursorCol = x % SCREEN_WIDTH;  // Horizonal position on display
    cursorRow = y + (x / SCREEN_WIDTH);   // Vertical position on display
}

// Using a pointer to video memory we can put characters to the display
// Every two addresses contain a character and a color
char putchar(char character)
{
    volatile char *video = (volatile char *) VIDEO_MEM; // Getting address of VIDEO_MEM
    if(character == '\n'){  // If character is newline, move to next row and reset column
        setcursor(0, cursorRow + 1);
    }else{ // else type character with color
        int pos = (cursorRow * SCREEN_WIDTH + cursorCol) * 2; // Get position of cursor
        video[pos] = character; // Set character
        video[pos + 1] = TEXT_COLOR; // Set color
        setcursor(cursorCol + 1, cursorRow); //move to next column
    }
	return character;
}

// Print the character array (string) using putchar()
// Print until we find a NULL terminator (0)
int printf(char string[]) 
{
    int charCount = 0; // keep track of character count.
    for (int i=0;string[i] != '\0';i++){ // loop through array and print using putchar() until end of string
        putchar(string[i]); // print character, handles newlines (\n)
        charCount++; // increment character count
    }
	return charCount; // return # of characters printed to display
}

// Prints an integer to the display
// Useful for debugging
int printint(uint32 n) 
{
	int characterCount = 0;
	if (n >= 10)
	{
        characterCount = printint(n / 10);
    }
    putchar('0' + (n % 10));
	characterCount++;

	return characterCount;
}

// Clear the screen by placing a ' ' character in every character location
void clearscreen()
{
    volatile char *video = (volatile char *) VIDEO_MEM; // Get address of VIDEO_MEM again.
    for(int i=0;i<SCREEN_HEIGHT * SCREEN_WIDTH; i++){ // go through the screen and clear everything
        video[i*2]=' '; // clear by replacing with whitespace
        video[i*2+1] = TEXT_COLOR; // set color
    }
	setcursor(0,0); //reset cursor
}