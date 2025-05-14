#include "./io.h"
#include "./multitasking.h"
#include "./irq.h"
#include "./isr.h"
#include "./fat.h"
#include "./string.h"

void prockernel();
void fileproc();

int main() 
{
	// Clear the screen
	clearscreen();

	// Initialize our keyboard
	initkeymap();

	// Initialize interrupts
	idt_install();
    isrs_install();
    irq_install();

	// Start executing the kernel process
	startkernel(prockernel);
	
	return 0;
}

void prockernel()
{
	// Create the user processes
	createproc(fileproc, (void *) 0x10000);

	// Count how many processes are ready to run
	int userprocs = ready_process_count();

	printf("Kernel Process Started\n");
	
	// As long as there is 1 user process that is ready, yield to it so it can run
	while(userprocs > 0)
	{
		// Yield to the user process
		yield();
		
		printf("Kernel Process Resumed\n");

		// Count the remaining ready processes (if any)
		userprocs = ready_process_count();
	}

	printf("Kernel Process Terminated\n");
}

// The user processes

void fileproc()
{	
	init_fs();
	char input;

	do
	{
		// Ask the user to make a selection
		printf("Make a selection (c, d, r, w, q): ");
		input = getchar();
		putchar(input);
		putchar('\n');

		// If the user typed quit, simply break out of the loop
		if(input == 'q')
		{
			break;
		}
		// If the input was invalid, just restart loop
		else if(input != 'c' && input != 'd' && input != 'r' && input != 'w')
		{
			printf("Error: Invalid input!\n");
			continue;
		}

		// Let the user type in a file name and extension
		char filename[9];
		char ext[4];

		printf("Enter filename: ");
		scanf(filename);
		putchar('\n');

		printf("Enter extension: ");
		scanf(ext);
		putchar('\n');

		// Search the directory to see if there exists an entry that contains the file name and extension
		char fileExists = openFile(filename, ext) == 0;

		// If we actually found a file...
		if(fileExists)
		{
			// Delete the file from the file system
			if(input == 'd')
			{
				printf("Deleting File...\n");

				// Open the file
				openFile(filename, ext);
				deleteFile();
			}
			// Read the file and print the contents to the display
			else if(input == 'r')
			{
				clearscreen();
				printf("Reading File...\n");

				// Read one byte from the file
				uint8 byte = readNextByte();

				// Print the contents of the file to the string
				while(byte != (uint8)-2)
				{
					// Print the byte to the screen
					if (byte != 0) putchar((char)byte);

					// Read one byte from the file
					byte = readNextByte();
				}

				// Close the file
				putchar('\n');
				closeFile();
			}
			// Allow the user to type in characters and write those to the file
			else if(input == 'w')
			{
				clearscreen();
				printf("Writing File...\n");
				printf("Type enter once to add an additional sector to the file.\n");
				printf("Type enter twice to close the file.\n");

				uint32 i = 0;
				uint8 prevByte = 0;
				uint8 byte = 0;

				// Let the user type in characters into the file (until they hit ENTER)
				do
				{
					prevByte = byte;
					byte = (uint8)getchar();

					if(byte != '\n')
					{
						// Print the character to the file
						putchar((char)byte);
						writeNextByte(byte);
						i++;
					}
					else if(byte == '\n' && prevByte != '\n')
					{
						clearscreen();
						printf("Writing File...\n");
						printf("Type enter once to add an additional sector to the file.\n");
						printf("Type enter twice to close the file.\n");

						// Fill up the remaining sector with 0's to move onto the next sector
						writeBytes(0, 512 - (i % 512));
						i += 512 - (i % 512);
					}
					
				}while(!(byte == '\n' &&  prevByte == '\n'));
				
				// If we have not overwritten the entire sector, do so now
				// This prevents nasty leftovers in the sector from old writes
				writeBytes(0, 512 - i);
				i += 512 - i;

				// Close the file (save the results to the disk)
				putchar('\n');
				closeFile();

				clearscreen();
			}
			// We cannot create a new file with the same name! (Do nothing)
			else if(input == 'c') printf("Error: Tried to create a file that already exists!\n");
		}
		// If we didn't find the file...
		else
		{
			// If we are creating a new file
			if(input == 'c')
			{
				printf("Creating File...\n");
				// Create the file on our file system (adds the empty file to our floppy disk)
				createFile(filename, ext);
			}
			// None of the following should run, if we couldn't find a file, we cannot delete, read, or write to it!
			else if(input == 'd') printf("Error: Tried deleting a file that doesn't exist!\n");
			else if(input == 'r') printf("Error: Tried reading a file that doesn't exist!\n");
			else if(input == 'w') printf("Error: Tried writing to a file that doesn't exist!\n");
		}	
	}while(input != 'q');

	exit();
}

