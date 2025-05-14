// The maximum number of user procs
#define MAX_USER_PROCS 5

// The maximum number of kernel procs
#define MAX_KERN_PROCS 1

// The maximum number of total procs
#define MAX_PROCS MAX_USER_PROCS + MAX_KERN_PROCS

// All possible statuses for processes
typedef enum
{
	PROC_STATUS_NONE,
    PROC_STATUS_RUNNING,
	PROC_STATUS_TERMINATED,
	PROC_STATUS_READY
} proc_status_t;

// All possible types of processes
typedef enum
{
	PROC_TYPE_NONE,
	PROC_TYPE_KERNEL,
	PROC_TYPE_USER // Added user process
} proc_type_t;


// Process control block
// Contains all registers and info for each process
typedef struct
{
    int pid;
	proc_type_t type;
	proc_status_t status;
	uint32 eax;
	uint32 ebx;
	uint32 ecx;
	uint32 edx;
	uint32 esi;
	uint32 edi;
	void *ebp;
	void *esp;
	uint32 eflags;
	uint32 cs;
	uint32 cr3;
	void *eip;
} proc_t;

int schedule();
int createproc(void *func, void *stack);
int startkernel(void func());
int ready_process_count();
void runproc(proc_t proc);
void yield();
void contextswitch();
void exit();
void banner();