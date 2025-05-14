#include "./fat.h"
#include "./fdc.h"
#include "./string.h"

// FAT Copies
// First copy is fat0 stored at 
// Second copy is fat1
// There were issues declaring the FATs as non-pointers
// When they would get read from floppy, it would overwrite wrong areas of memory
fat_t *fat0;
fat_t *fat1;
void *startAddress = (void *) 0x20000;

directory_t currentDirectory;  // The current directory we have opened
directory_entry_t rootDirectoryEntry;   // The root directory's directory entry (this does not exist on the disk since the root is not inside of another directory)
file_t currentFile;            // The current file we have opened

// Initialize the file system
// Loads the FATs and root directory
void init_fs()
{
    // The FATs and directory are loaded into 0x20000, 0x21200, and 0x22400
    // These addresses were chosen because they are far enough away from the kernel (0x01000 - 0x07000)

    // Read the first copy of the FAT (Drive 0, Cluster 1, 512 bytes * 9 clusters)
    fat0 = (fat_t *) startAddress; // Put FAT at 0x20000
    floppy_read(0, 1,  (void *)fat0, sizeof(fat_t));

    // Read the second copy of the FAT (Drive 0, Cluster 10, 512 bytes * 9 clusters)
    fat1 = (fat_t *) (startAddress+sizeof(fat_t)); // Put FAT at 0x21200
    floppy_read(0, 10, (void *)fat1, sizeof(fat_t));

    // Read the root directory (Drive 0, Cluster 19, 512 bytes * 14 clusters)
    currentDirectory.isOpened = 1;
    currentDirectory.directoryEntry = &rootDirectoryEntry;

    currentDirectory.startingAddress = (uint8 *) (startAddress+(sizeof(fat_t)*2)); // Put ROOT at 0x22400
    stringcopy("ROOT    ", (char *)currentDirectory.directoryEntry->filename, 8);

    floppy_read(0, 19, (void *)currentDirectory.startingAddress, 512 * 14);

    // Start our file out blank
    currentFile.isOpened = 0;
    currentFile.directoryEntry = 0;
    currentFile.index = 0;
    currentFile.startingAddress = 0;
}

void renameFile(char *newFilename, char *newExt){
    // Check if file is opened
    if(!currentFile.isOpened)
    {
        printf("Error: File was not opened!\n");
        return;
    }
    // Copy the new filename and extension into the directory entry
    stringcopy(newFilename, (char *)currentFile.directoryEntry->filename, 8);
    stringcopy(newExt, (char *)currentFile.directoryEntry->ext, 3);

    floppy_write(0, 19, (void *)currentDirectory.startingAddress, 512 * 14); // Write the directory to the disk
    floppy_write(0, 1, (void *)fat0, sizeof(fat_t)); // Write the first FAT to the disk
    floppy_write(0, 10, (void *)fat1, sizeof(fat_t)); // Write the second FAT to the disk

    currentFile.isOpened = 0; // Mark the file as closed
}

void moveFile(directory_t *toDirectory){
    // Check if file is opened
    if(!currentFile.isOpened)
    {
        printf("Error: File was not opened!\n");
        return;
    }
    // Copy the directory entry to the new directory
    directory_entry_t *directoryEntry = (directory_entry_t *)toDirectory->startingAddress;
    uint32 maxDirectoryEntryCount = 16;

    // Check for an empty directory entry
    for(uint32 index = 0; index < maxDirectoryEntryCount; index++, directoryEntry++)
    {
        // Check if entry is empty (first byte is null '\0')
        if(directoryEntry->filename[0] == 0)
        {
            // Copy the directory entry to the new directory
            *directoryEntry = *currentFile.directoryEntry;
            break;
        }
    }

    floppy_write(0, 19, (void *)toDirectory->startingAddress, 512 * 14); // Write the directory to the disk
    floppy_write(0, 1, (void *)fat0, sizeof(fat_t)); // Write the first FAT to the disk
    floppy_write(0, 10, (void *)fat1, sizeof(fat_t)); // Write the second FAT to the disk

    currentFile.isOpened = 0; // Mark the file as closed
}

int closeFile()
{
    // Check if file is opened
    if(!currentFile.isOpened)
    {
        printf("Error: File was not opened!\n");
        return -1;

    }
    // Calculate number of clusters needed
    uint32 clustersNeeded = (currentFile.directoryEntry->fileSize + 511) / 512;
    uint8 *dataPtr = currentFile.startingAddress;

    uint16 cluster = currentFile.directoryEntry->startingCluster;
    uint16 prevCluster = 0;

    // Allocate new cluster if needed
    for(uint32 i = 0; i < clustersNeeded; i++)
    {
        
        if(cluster == 0 || cluster == 0xFFFF)
        {
            // search for free cluster
            uint16 newCluster;
            for(newCluster = 2; newCluster < 2304; newCluster++)
            {
                if(fat0->clusters[newCluster] == 0)
                {
                    break;
                }
            }
            // Link previous cluster to new cluster
            fat0->clusters[cluster] = newCluster;
            fat1->clusters[cluster] = newCluster;
            cluster = newCluster; // Update the current cluster to the new cluster
        }
        
        floppy_write(0, cluster + 31, (void *) (dataPtr + (i * 512)), 512); // Write data to floppy disk
        prevCluster = cluster; // Save current cluster for next iteration
        cluster = fat0->clusters[cluster]; // Get next cluster
    }
    // Mark the last cluster as end of file
    fat0->clusters[prevCluster] = 0xFFFF;
    fat1->clusters[prevCluster] = 0xFFFF;
    floppy_write(0, 1, (void *)fat0, sizeof(fat_t)); // Write the first FAT to the disk
    floppy_write(0, 10, (void *)fat1, sizeof(fat_t)); // Write the second FAT to the disk (redundant)
    floppy_write(0, 19, (void *)currentDirectory.startingAddress, 512 * 14); // Write the directory to the disk

    currentFile.isOpened = 0; // Mark the file as closed

    return 0;
}

int createFile(char *filename, char *ext)
{
    // Create a new file in the current directory
    directory_entry_t *directoryEntry = (directory_entry_t *)currentDirectory.startingAddress;
    uint32 maxDirectoryEntryCount = 16;

    // Check for an empty directory entry
    for(uint32 index = 0; index < maxDirectoryEntryCount; index++, directoryEntry++)
    {
        // Check if entry is empty (first byte is null '\0')
        if(directoryEntry->filename[0] == 0)
        {
            // Copy the filename and extension into the directory entry
            stringcopy(filename, (char *)directoryEntry->filename, 8);
            stringcopy(ext, (char *)directoryEntry->ext, 3);

            // Set the starting cluster, Cluster 1 is boot sector.
            for(uint16 startingCluster = 2; startingCluster < 2304; startingCluster++)
            {
                // Check if the cluster is free in both FATs
                if(fat0->clusters[startingCluster] == 0 && fat1->clusters[startingCluster] == 0)
                {
                    directoryEntry->startingCluster = startingCluster; // Set the starting cluster in the directory entry
                    directoryEntry->fileSize = 512; // Set the file size to 512 bytes (1 sector)
                    // Mark the cluster as end of file
                    fat0->clusters[startingCluster] = 0xFFFF;
                    fat1->clusters[startingCluster] = 0xFFFF;
                    break;
                }
            }
            floppy_write(0, 19, (void *)currentDirectory.startingAddress, 512 * 14); // Write the directory to the disk
            floppy_write(0, 1, (void *)fat0, sizeof(fat_t)); // Write the first FAT to the disk
            floppy_write(0, 10, (void *)fat1, sizeof(fat_t)); // Write the second FAT to the disk
            
            currentFile.isOpened = 0; // Mark the file as closed
            return 0;
        }
    }
    
    return -1; // No empty directory entry found
}

int deleteFile()
{
    // Check if file is opened
    if(!currentFile.isOpened)
    {
        printf("Error: File was not opened!\n");
        return -1;

    }
    // Set the cluster to current file's starting address
    uint16 cluster = currentFile.directoryEntry->startingCluster;
    // Loop through and free all clusters in the file
    while(cluster != 0xFFFF)
    {
        uint16 nextCluster = fat0->clusters[cluster]; // Get the next cluster from FAT 0
        // Mark the cluster as free in both FATs
        fat0->clusters[cluster] = 0;
        fat1->clusters[cluster] = 0;
        cluster = nextCluster; // Get the next cluster from FAT 0
    }
    // Clear the directory entry
    currentFile.directoryEntry->filename[0] = 0; // Set the first byte of the filename to null

    floppy_write(0, 19, (void *)currentDirectory.startingAddress, 512 * 14); // Write the directory to the disk
    floppy_write(0, 1, (void *)fat0, sizeof(fat_t)); // Write the first FAT to the disk
    floppy_write(0, 10, (void *)fat1, sizeof(fat_t)); // Write the second FAT to the disk

    currentFile.isOpened = 0; // Mark the file as closed
    return 0;
}

// Returns a byte from a file that is currently loaded into memory
// This does NOT modify the floppy disk
// This function requires the file to have been loaded into memory with floppy_read()
uint8 readByte(uint32 index)
{
    // Are we trying to read from the end of a file?
    if(index >= currentFile.directoryEntry->fileSize)
    {
        return -2;
    }

    // Check if the file is opened and is not a NULL pointer
    if(currentFile.isOpened && currentFile.startingAddress != 0)
    {
        currentFile.index = index + 1;              // Point us to the next index
        // Return the byte at the specified index
        return currentFile.startingAddress[index];
    }

    // If the file was not opened, or was a NULL pointer, return error
    printf("Error: File was not opened or pointed to NULL!\n");
    return -1;
}

// Returns the next byte from a file that is currently loaded into memory
uint8 readNextByte()
{
    return readByte(currentFile.index);
}

// Writes a byte to the current file that is currently loaded into memory
// This does NOT modify the floppy disk
// To write this to the floppy disk, we have to call floppy_write()
int writeByte(uint8 byte, uint32 index)
{
    // Check if the file is opened and is not a NULL pointer
    if(currentFile.isOpened && currentFile.startingAddress != 0)
    {
        currentFile.startingAddress[index] = byte;  // Place the byte at the address + index
        if(index + 1 > currentFile.directoryEntry->fileSize) currentFile.directoryEntry->fileSize = index + 1;    // Increase the file size
        currentFile.index = index + 1;              // Point us to the next index
        return 0;
    }

    // If the file was not opened, or was a NULL pointer, return error
    printf("Error: File was not opened or pointed to NULL!\n");
    return -1;
}

// Writes a byte to the current file that is currently loaded into memory at the next index
int writeNextByte(uint8 byte)
{
    return writeByte(byte, currentFile.index);
}

// Writes a byte to the current file that is currently loaded into memory at the next index multiple times
int writeBytes(uint8 byte, uint32 count)
{
    for(int i = 0; i < (int)count; i++)
    {
        int error = writeByte(byte, currentFile.index);

        if (error != 0) return error;
    }

    return 0;
}

// Finds a file within our current directory and loads every sector of the file into memory
// Returns 0 if the file was found in the current directory
// Returns -3 if the file was not found in the current directory
// Returns other error codes if something went wrong
int openFile(char *filename, char *ext)
{
	// If a file is open, stop!
    if(currentFile.isOpened)
    {
        printf("A file is already open! Please close this file before opening another!\n");
        return 0;
    }
	
    // Scrub null terminators from our filename and extension and pad with spaces for more accurate comparisons
    char nullFound = 0;
    for(int i = 1; i < 8; i++)
    {
        if (filename[i] == 0 && !nullFound) nullFound = 1;
        if (nullFound) filename[i] = ' ';
    }

    nullFound = 0;
    for(int i = 1; i < 3; i++)
    {
        if (ext[i] == 0 && !nullFound) nullFound = 1;
        if (nullFound) ext[i] = ' ';
    }

    // Get a pointer to the first address of the directory entry in this directory
	directory_entry_t *directoryEntry = (directory_entry_t *)currentDirectory.startingAddress;
    uint32 maxDirectoryEntryCount = 16;
    char fileExists = 0;

    // Assume we can have a maximum of 16 directory entries per directory
    // Check each one to see if our filename and extension match
    // This should actually be checking if the first byte of the directory entry is null
	for(uint32 index = 0; index < maxDirectoryEntryCount; index++)
	{
        // Check if this entry has the same file name and extension
		fileExists = stringcompare((char *)directoryEntry->filename, filename, 8) && stringcompare((char *)directoryEntry->ext, ext, 3);
		
        // If we found the file we can stop looping!
		if(fileExists) break;

		directoryEntry++;
	}

    // If the file exists, let's open it
    if(fileExists)
    {
        // Check if the file has been corrupted
        // Check each entry in the FAT table, ensure both FATs are consistent before opening the file
        uint16 cluster = directoryEntry->startingCluster;
        while(cluster != 0xFFFF)
        {
            // Check if the copies of the FATs are consistent
            if(fat0->clusters[cluster] != fat1->clusters[cluster])
            {
                printf("Error: The file was found BUT the FAT table entries for this file differ!\n");
                return -1;
            }

            // Get the next cluster
            cluster = fat0->clusters[cluster];
        }

        // Set the starting address of the file to some memory location
        uint8 *startingAddress = (void *)0x30000;

        // Starting at the first sector, each each sector from floppy into memory
        cluster = directoryEntry->startingCluster;
        uint16 sectorCount = 0;

        // Loop through every link in the FAT
        while(cluster != 0xFFFF)
        {
            // Convert the cluster to a sector
            uint32 sector = cluster + 31;
            
            // Read the sector from the floppy disk
            floppy_read(0, sector, (void *) startingAddress + (512 * sectorCount), 512);
            sectorCount++;

            // Get the next cluster
            cluster = fat0->clusters[cluster];

            // It is possible to get stuck in an infinite loop, reading FAT entries forever
            // We prevent that here by checking if the amount of sectors could actually fit on disk
            if(sectorCount > 2880)
            {
                printf("Error: The file appears to be bigger than the entire floppy disk!\n");
                return -2;
            }
        }

        // If no error has occured, label the file as opened and point it to all the data we just read in
        currentFile.directoryEntry = directoryEntry;
        currentFile.startingAddress = startingAddress;
        currentFile.index = 0;
        currentFile.isOpened = 1;
        return 0;
    }

    // If we did not find the file return -3
	return -3;
}