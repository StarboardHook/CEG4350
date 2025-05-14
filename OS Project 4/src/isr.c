#include "./idt.h"
#include "./io.h"
#include "./multitasking.h"

extern  void _isr0();
extern  void _isr1();
extern  void _isr2();
extern  void _isr3();
extern  void _isr4();
extern  void _isr5();
extern  void _isr6();
extern  void _isr7();
extern  void _isr8();
extern  void _isr9();
extern  void _isr10();
extern  void _isr11();
extern  void _isr12();
extern  void _isr13();
extern  void _isr14();
extern  void _isr15();
extern  void _isr16();
extern  void _isr17();
extern  void _isr18();
extern  void _isr19();
extern  void _isr20();
extern  void _isr21();
extern  void _isr22();
extern  void _isr23();
extern  void _isr24();
extern  void _isr25();
extern  void _isr26();
extern  void _isr27();
extern  void _isr28();
extern  void _isr29();
extern  void _isr30();
extern  void _isr31();
extern  void _syscall();
void context_switch_isr(struct regs *r, proc_t **running, proc_t **next);

void isrs_install()
{
	idt_set_gate(0, (unsigned)_isr0, 0x08, 0x8E);
	idt_set_gate(1, (unsigned)_isr1, 0x08, 0x8E);
	idt_set_gate(2, (unsigned)_isr2, 0x08, 0x8E);
	idt_set_gate(3, (unsigned)_isr3, 0x08, 0x8E);
	idt_set_gate(4, (unsigned)_isr4, 0x08, 0x8E);
	idt_set_gate(5, (unsigned)_isr5, 0x08, 0x8E);
	idt_set_gate(6, (unsigned)_isr6, 0x08, 0x8E);
	idt_set_gate(7, (unsigned)_isr7, 0x08, 0x8E);
	idt_set_gate(8, (unsigned)_isr8, 0x08, 0x8E);
	idt_set_gate(9, (unsigned)_isr9, 0x08, 0x8E);
	idt_set_gate(10, (unsigned)_isr10, 0x08, 0x8E);
	idt_set_gate(11, (unsigned)_isr11, 0x08, 0x8E);
	idt_set_gate(12, (unsigned)_isr12, 0x08, 0x8E);
	idt_set_gate(13, (unsigned)_isr13, 0x08, 0x8E);
	idt_set_gate(14, (unsigned)_isr14, 0x08, 0x8E);
	idt_set_gate(15, (unsigned)_isr15, 0x08, 0x8E);
	idt_set_gate(16, (unsigned)_isr16, 0x08, 0x8E);
	idt_set_gate(17, (unsigned)_isr17, 0x08, 0x8E);
	idt_set_gate(18, (unsigned)_isr18, 0x08, 0x8E);
	idt_set_gate(19, (unsigned)_isr19, 0x08, 0x8E);
	idt_set_gate(20, (unsigned)_isr20, 0x08, 0x8E);
	idt_set_gate(21, (unsigned)_isr21, 0x08, 0x8E);
	idt_set_gate(22, (unsigned)_isr22, 0x08, 0x8E);
	idt_set_gate(23, (unsigned)_isr23, 0x08, 0x8E);
	idt_set_gate(24, (unsigned)_isr24, 0x08, 0x8E);
	idt_set_gate(25, (unsigned)_isr25, 0x08, 0x8E);
	idt_set_gate(26, (unsigned)_isr26, 0x08, 0x8E);
	idt_set_gate(27, (unsigned)_isr27, 0x08, 0x8E);
	idt_set_gate(28, (unsigned)_isr28, 0x08, 0x8E);
	idt_set_gate(29, (unsigned)_isr29, 0x08, 0x8E);
	idt_set_gate(30, (unsigned)_isr30, 0x08, 0x8E);
	idt_set_gate(31, (unsigned)_isr31, 0x08, 0x8E);
	
	idt_set_gate(0x80, (unsigned)_syscall, 0x08, 0x8E);
	
}

extern void _syscall_isr(struct regs *r)
{
	uint32 syscall = r->eax;

	if (syscall == 0x01)
	{
		proc_t **running = (proc_t **)r->ebx;
		proc_t **next = (proc_t **)r->ecx;
		context_switch_isr(r, running, next);
	}
}

// Context switching function
// This function will save the context of the running process (proc_t running)
// and switch to the context of the next process we want to run (proc_t next)
// The running and next processes must both be valid for this function to work
// if they are not, our OS will certainly crash
void context_switch_isr(struct regs *r, proc_t **running, proc_t **next)
{
    // Store all the current register values inside the process that is running

    (*running)->eax    = r->eax;
    (*running)->ebx    = r->ebx;
    (*running)->ecx    = r->ecx;
    (*running)->edx    = r->edx;

    (*running)->esi    = r->esi;
    (*running)->edi    = r->edi;

    (*running)->ebp    = (void *)r->ebp;
    (*running)->esp    = (void *)r->esp;

    (*running)->eflags = r->eflags;
	(*running)->cs 	   = r->cs;
	(*running)->eip	   = (void *)r->eip;

	if (*next == 0 || next == 0 || (*next)->eip == 0)
	{
		printf("\nERROR: Could not perform context switch. Process was null!\n");
		for(;;){}
		return;
	}

    // Start running the next process
    
    *running = *next;
    (*running)->status = PROC_STATUS_RUNNING;

    // Reload all the registers previously saved from the process we want to run

	r->eax    = (uint32)(*next)->eax;
    r->ebx    = (uint32)(*next)->ebx;
    r->ecx    = (uint32)(*next)->ecx;
    r->edx    = (uint32)(*next)->edx;

    r->esi    = (uint32)(*next)->esi;
    r->edi    = (uint32)(*next)->edi;

    r->ebp    = (uint32)(*next)->ebp;
    r->esp    = (uint32)(*next)->esp;

	uint32 *new_esp = (uint32 *)((*next)->esp + 8);

    new_esp[2] = (uint32)(*next)->eflags;

	if ((uint32)(*next)->cs == 0)
	{
		new_esp[1] = (uint32)r->cs;
	}
	else
	{
		new_esp[1] = (uint32)(*next)->cs;
	}

	
	new_esp[0] = (uint32)(*next)->eip;
}