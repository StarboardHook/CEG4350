#include "./types.h"

typedef struct
{
    // FAT12 Bios Parameter Block
    uint8   jumpInstruction[3];
    uint8   oem[8];
    uint16  bytesPerSector;
    uint8   sectorsPerCluster;
    uint16  ReservedSectors;
    uint8   fatCount;
    uint16  rootDirectoryEntries;
    uint16  sectorCount;
    uint8   mediaDescriptorType;
    uint16  sectorsPerFat;
    uint16  sectorsPerTrack;
    uint16  headCount;
    uint32  hiddenSectorCount;
    uint32  largeSectorCount;

    // Extended Boot Record
    uint8   driveNumber;
    uint8   reserved;
    uint8   signature;
    uint32  volumeID;
    uint8   volumeLabel[11];
    uint8   systemID[8];

} __attribute__((packed)) boot_sector_t;

typedef struct
{
    // File Allocation Table (FAT)
    // We will use 16-bits for our entries
    uint16 clusters[2304];

} __attribute__((packed)) fat_t;

typedef struct
{
    // Directory entry contents
    uint8   filename[8];    //x22400 - x22407   (8) (0 - 7)
    uint8   ext[3];         //x22408 - x2240A   (3) (8 - 10)
    uint8   attributes;     //x2240B            (1) (11)
    uint16  reserved;       //x2240C - x2240D   (2) (12 - 13)
    uint16  creationTime;   //x2240E - x2240F   (2) (14 - 15)
    uint16  creationDate;   //x22410 - x22411   (2) (16 - 17)
    uint16  lastAccessDate; //x22412 - x22413   (2) (18 - 19)
    uint16  ignored;        //x22414 - x22415   (2) (20 - 21)
    uint16  lastWriteTime;  //x22416 - x22417   (2) (22 - 23)
    uint16  lastWriteDate;  //x22418 - x22419   (2) (24 - 25)
    uint16  startingCluster;//x2241A - x2241B   (2) (26 - 27)
    uint32  fileSize;       //x2241C - x2241F   (4) (28 - 31)

} __attribute__((packed)) directory_entry_t;

typedef struct
{
    uint32 index;
    uint8 *startingAddress;

    // Set to 0 if not opened
    // Set to non-zero if opened
    char isOpened;

    // The directory entry for the file, containing all its metadata
    directory_entry_t *directoryEntry;

} __attribute__((packed)) file_t;

typedef struct
{   
    uint8 *startingAddress;

    // Set to 0 if not opened
    // Set to non-zero if opened
    char isOpened;

    // The directory entry for the directory, containing all its metadata
    directory_entry_t *directoryEntry;

} __attribute__((packed)) directory_t;

void init_fs();
int openDirectory(directory_t *directory);
int openFile(char *filename, char* ext);
int closeFile();
int createDirectory(directory_t *directory);
int createFile(char *filename, char* ext);
void deleteDirectory(directory_t *file);
int deleteFile();
uint8 readByte(uint32 index);
uint8 readNextByte();
int writeByte(uint8 byte, uint32 index);
int writeBytes(uint8 byte, uint32 count);
int writeNextByte(uint8 byte);
int findFile(char *filename, char* ext, directory_t directory, directory_entry_t *foundEntry);