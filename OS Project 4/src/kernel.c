#include "./io.h"
#include "./multitasking.h"
#include "./irq.h"
#include "./isr.h"
#include "./fat.h"

void prockernel();
void proca();
void procb();
void procc();
void procd();
void proce();
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
	createproc(proca, (void *) 0x10000);
	createproc(procb, (void *) 0x11000);
	createproc(procc, (void *) 0x12000);
	createproc(procd, (void *) 0x13000);
	createproc(proce, (void *) 0x14000);

	// Count how many processes are ready to run
	//int userprocs = ready_process_count();

	printf("Kernel Process Started\n");
	
	// As long as there is 1 user process that is ready, yield to it so it can run
	while(ready_process_count() > 0)
	{
		// Yield to the user process
		yield();
		// Count the remaining ready processes (if any)
		//userprocs = ready_process_count();
	}

	printf("\nKernel Process Terminated\n");
}

// The user processes Project 4
void proca()
{
	printf("A");
	exit();
}
void procb()
{
	printf("B");
	yield();
	printf("B");
	exit();
}
void procc()
{
	printf("C");
	yield();
	printf("C");
	yield();
	printf("C");
	yield();
	printf("C");
	exit();
}
void procd()
{
	printf("D");
	yield();
	printf("D");
	yield();
	printf("D");
	exit();
}
void proce()
{
	printf("E");
	yield();
	printf("E");
	exit();
}

// The user processes Project 3
// void proc_a()
// {
// 	printf("User Process A Start\n");
// 	exit();
// }

// void proc_b()
// {
// 	printf("User Process B Start\n");
// 	yield();
// 	printf("User Process B Resumed 1st\n");
// 	exit();
// }

// void proc_c()
// {
// 	printf("User Process C Start\n");
// 	yield();
// 	printf("User Process C Resumed 1st\n");
// 	yield();
// 	printf("User Process C Resumed 2nd\n");
// 	exit();
// }

// void proc_d()
// {
// 	printf("User Process D Start\n");
// 	yield();
// 	printf("User Process D Resumed 1st\n");
// 	yield();
// 	printf("User Process D Resumed 2nd\n");
// 	yield();
// 	printf("User Process D Resumed 3rd\n");
// 	exit();
// }