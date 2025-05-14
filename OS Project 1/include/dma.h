#include "./types.h"

void maskChannel(uint8 channel, int masked);
void initFloppyDMA(uint32 address, uint16 count);
void prepare_for_floppyDMA_read();
void prepare_for_floppyDMA_write();