#include "./types.h"
#include "./io.h"
void floppy_detect_drives();
int floppy_init();
int floppy_read(int drive, uint32 lba, void* address, uint16 count);
int floppy_write(int drive, uint32 lba, void* address, uint16 count);