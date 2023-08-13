#include <sys/types.h>
#include <stdio.h>

#include <time.h>

#include "screen.h"

#define ROM_MEMORY_START 0xFA0000;
#define ROM4_MEMORY_START ROM_MEMORY_START
#define ROM3_MEMORY_START ROM_MEMORY_START + 0x20000
#define ROM4_BANK 0
#define ROM3_BANK 1
#define ROM_SIZE_BYTES 128 * 1024
#define ROM_SIZE_WORDS ROM_SIZE_BYTES / 2
#define ROMBANK_SIZE_BYTES 64 * 1024
#define ROMBANK_SIZE_WORDS ROMBANK_SIZE_BYTES / 2

#ifdef _DEBUG
#define TEST_ROM_FILE "HATARROM.BIN"
#define RANDOM_ACCESS_ITERATIONS 1000
#define ADDRESS_LINE_ITERATIONS 1000
#else
#define TEST_ROM_FILE "TESTROM.BIN"
#define RANDOM_ACCESS_ITERATIONS 1000000
#define ADDRESS_LINE_ITERATIONS 1000000
#endif

const int SPINNER_UPDATE_FREQUENCY = 4096;
char spinner[] = {'\\', '|', '/', '-'};

int testDifferentVersions(unsigned char *rom_data)
{
    // Check if the content of the version string is the same in the ROM
    // and in the executable

    printf("- Testing version string...  ");

    // Create a buffer to store the version from the ROM
    char rom_version[12]; // 11 characters + null terminator
    memcpy(rom_version, &rom_data[4], 11);
    rom_version[11] = '\0'; // Null terminate the string

    if (strcmp(rom_version, VERSION) != 0)
    {
        printf("\r\n    x Error: ROM version string mismatch. Expected: %s, got: %s\r\n", VERSION, rom_version);
        return 1;
    }

    printf("Matches: %s.\r\n", VERSION);
    return 0;
}

/**
 * Tests if the data read sequentially from ROM 4 matches the expected data.
 *
 * This function compares 16-bit words from the `rom_data` to the expected words
 * in `file_data`. If any mismatch is found, it prints an error message with the
 * address of the mismatch and the expected vs. actual data.
 *
 * @param rom_data A pointer to the start address of the data read from ROM 4.
 * @param file_data A pointer to the start address of the expected data.
 * @param rombank The ROM bank number to be tested. Use the constants ROM4_BANK and ROM3_BANK.
 * @return Returns 0 if all data matches, 1 if a mismatch is found.
 */
int testSequentialReadROM(unsigned char *rom_data, unsigned char *file_data, int rombank)
{
    printf("- Testing sequential read ROM %s...  ", rombank == ROM4_BANK ? "4" : "3");
    __uint16_t *rom_data_words = (__uint16_t *)rom_data;   // Assuming rom_data was previously defined as unsigned char*
    __uint16_t *file_data_words = (__uint16_t *)file_data; // Assuming file_data was previously defined as unsigned char*
    rom_data_words += rombank * ROMBANK_SIZE_WORDS;        // Move the pointer to the start of the ROM bank
    file_data_words += rombank * ROMBANK_SIZE_WORDS;       // Move the pointer to the start of the ROM bank in the file
    for (int i = 0; i < ROMBANK_SIZE_WORDS; i++)
    {
        __uint16_t rom_word = rom_data_words[i];
        __uint16_t file_word = file_data_words[i];
        if (rom_word != file_word)
        {
            unsigned long real_memory = rombank * ROMBANK_SIZE_BYTES + ROM4_MEMORY_START;
            real_memory += i * 2;
            printf("\r\n    x Error: Data mismatch at address %p. Expected: %p, got: %p\r\n", real_memory, file_word, rom_word);
            return 1;
        }
        if (i % SPINNER_UPDATE_FREQUENCY == 0)
        {
            printf("\b%c", spinner[(i / SPINNER_UPDATE_FREQUENCY) % 4]);
        }
    }
    printf("\bSuccess.\r\n");
    return 0;
}

/**
 * Tests if the data read sequentially from ROM matches the expected data.
 *
 * This function compares 16-bit words from the `rom_data` to the expected words
 * in `file_data`. If any mismatch is found, it keeps track of the mismatches without printing immediately.
 *
 * @param rom_data A pointer to the start address of the data read from ROM.
 * @param file_data A pointer to the start address of the expected data.
 * @param rombank The ROM bank number to be tested. Use the constants ROM4_BANK and ROM3_BANK.
 * @return Returns 0 if all data matches, 1 if there were mismatches.
 */
int testSequentialReadROMStats(unsigned char *rom_data, unsigned char *file_data, int rombank)
{
    int successful_requests = 0;
    int failed_requests = 0;

    printf("- Testing stats seq read ROM %s...  ", rombank == ROM4_BANK ? "4" : "3");

    __uint16_t *rom_data_words = (__uint16_t *)rom_data;
    __uint16_t *file_data_words = (__uint16_t *)file_data;
    rom_data_words += rombank * ROMBANK_SIZE_WORDS;
    file_data_words += rombank * ROMBANK_SIZE_WORDS;

    for (int i = 0; i < ROMBANK_SIZE_WORDS; i++)
    {
        __uint16_t rom_word = rom_data_words[i];
        __uint16_t file_word = file_data_words[i];

        if (rom_word != file_word)
        {
            failed_requests++;
        }
        else
        {
            successful_requests++;
        }

        if (i % SPINNER_UPDATE_FREQUENCY == 0)
        {
            printf("\b%c", spinner[(i / SPINNER_UPDATE_FREQUENCY) % 4]);
        }
    }

    printf("\bSuccess: %d, Fail: %d\r\n",
           successful_requests,
           failed_requests);

    return failed_requests > 0 ? 1 : 0;
}

/**
 * Tests if randomly accessed data from ROM matches the expected data.
 *
 * This function randomly selects positions within the ROM data and compares
 * 16-bit words from the `rom_data` to the expected words in `file_data`.
 * If any mismatch is found, it prints an error message with the address of
 * the mismatch and the expected vs. actual data.
 *
 * @param rom_data A pointer to the start address of the data read from ROM.
 * @param file_data A pointer to the start address of the expected data.
 * @param rombank The ROM bank number to be tested. Use constants like ROM4_BANK and ROM3_BANK.
 * @param num_requests The number of random access requests to be performed.
 * @return Returns 0 if all data matches, 1 if a mismatch is found.
 */
int testRandomReadROM(unsigned char *rom_data, unsigned char *file_data, int rombank, int num_requests)
{
    printf("- Testing %i random access from ROM %s...  ", num_requests, rombank == ROM4_BANK ? "4" : "3");

    __uint16_t *rom_data_words = (__uint16_t *)rom_data;
    __uint16_t *file_data_words = (__uint16_t *)file_data;
    rom_data_words += rombank * ROMBANK_SIZE_WORDS;
    file_data_words += rombank * ROMBANK_SIZE_WORDS;

    srand(Random()); // Initialize random seed

    for (int i = 0; i < num_requests; i++)
    {
        unsigned long random_position = rand() & 0x7FFF; // Mask the value to be within the ROM size in words

        __uint16_t rom_word = rom_data_words[random_position];
        __uint16_t file_word = file_data_words[random_position];

        if (rom_word != file_word)
        {
            unsigned long real_memory = rombank * ROMBANK_SIZE_BYTES + ROM4_MEMORY_START;
            real_memory += random_position * 2;
            printf("\r\n    x Error: Data mismatch at %p. Expected: %p, got: %p\r\n", real_memory, file_word, rom_word);
            return 1;
        }

        if (i % SPINNER_UPDATE_FREQUENCY == 0)
        {
            printf("\b%c", spinner[(i / SPINNER_UPDATE_FREQUENCY) % 4]);
        }
    }

    printf("\bSuccess.\r\n");
    return 0;
}

/**
 * Tests if randomly accessed data from ROM matches the expected data and outputs stats.
 *
 * This function randomly selects positions within the ROM data and compares
 * 16-bit words from the `rom_data` to the expected words in `file_data`.
 * Instead of printing mismatches immediately, it keeps track of successful
 * and failed requests and prints a summary at the end.
 *
 * @param rom_data A pointer to the start address of the data read from ROM.
 * @param file_data A pointer to the start address of the expected data.
 * @param rombank The ROM bank number to be tested. Use constants like ROM4_BANK and ROM3_BANK.
 * @param num_requests The number of random access requests to be performed.
 * @return Returns 0 if all data matches, 1 if there were mismatches.
 */
int testRandomReadROMStats(unsigned char *rom_data, unsigned char *file_data, int rombank, int num_requests)
{
    int successful_requests = 0;
    int failed_requests = 0;

    printf("- Testing stats %i random access ROM %s...  ", num_requests, rombank == ROM4_BANK ? "4" : "3");

    __uint16_t *rom_data_words = (__uint16_t *)rom_data;
    __uint16_t *file_data_words = (__uint16_t *)file_data;
    rom_data_words += rombank * ROMBANK_SIZE_WORDS;
    file_data_words += rombank * ROMBANK_SIZE_WORDS;

    srand(Random()); // Initialize random seed

    for (int i = 0; i < num_requests; i++)
    {
        unsigned long random_position = rand() & 0x7FFF; // Mask the value to be within the ROM size in words

        __uint16_t rom_word = rom_data_words[random_position];
        __uint16_t file_word = file_data_words[random_position];

        if (rom_word != file_word)
        {
            failed_requests++;
        }
        else
        {
            successful_requests++;
        }

        if (i % SPINNER_UPDATE_FREQUENCY == 0)
        {
            printf("\b%c", spinner[(i / SPINNER_UPDATE_FREQUENCY) % 4]);
        }
    }

    printf("\bSuccess: %d, Fail: %d\r\n",
           successful_requests,
           failed_requests);

    return failed_requests > 0 ? 1 : 0;
}

/**
 * Tests if the data read from ROM by setting individual address lines high matches the expected data.
 *
 * This function sets individual address lines high (from A0 to A15) and reads the data
 * from `rom_data` and compares it to the expected data in `file_data`.
 *
 * @param rom_data A pointer to the start address of the data read from ROM.
 * @param file_data A pointer to the start address of the expected data.
 * @param rombank The ROM bank number to be tested. Use the constants ROM4_BANK and ROM3_BANK.
 * @param num_requests The number of times each address line is tested.
 * @return Returns 0 if all data matches, 1 if a mismatch is found.
 */
int testAddressLinesSequentialReadROM(unsigned char *rom_data, unsigned char *file_data, int rombank, int num_requests)
{
    printf("- Testing addr lines seq read ROM %s with %d req x line...  ", rombank == ROM4_BANK ? "4" : "3", num_requests);

    __uint16_t *rom_data_words = (__uint16_t *)rom_data;
    __uint16_t *file_data_words = (__uint16_t *)file_data;
    rom_data_words += rombank * ROMBANK_SIZE_WORDS;
    file_data_words += rombank * ROMBANK_SIZE_WORDS;

    // Can't read from A0 high, so start at A1
    for (int line = 1; line < 15; line++) // Loop over each address line
    {
        for (int request = 0; request < num_requests; request++)
        {
            int address = 1 << line;                              // This will set the current line high and all other lines low
            __uint16_t rom_word = rom_data_words[address >> 1];   // Divide by 2 to get the word address
            __uint16_t file_word = file_data_words[address >> 1]; // Divide by 2 to get the word address
            if (rom_word != file_word)
            {
                unsigned long real_memory = rombank * ROMBANK_SIZE_BYTES + ROM4_MEMORY_START;
                real_memory += address;
                printf("\r\n    x Error: Data mismatch at %p with only A%d high. Expected: %p, got: %p\r\n", real_memory, line, file_word, rom_word);
                return 1;
            }
        }

        // Spinner update
        if (line % SPINNER_UPDATE_FREQUENCY == 0)
        {
            printf("\b%c", spinner[(line / SPINNER_UPDATE_FREQUENCY) % 4]);
        }
    }

    printf("\bSuccess.\r\n");
    return 0;
}

/**
 * Compares sequential byte data read from ROM to expected data.
 *
 * This function performs a byte-by-byte comparison between the data read
 * from a specified ROM bank and the expected data provided. If a mismatch
 * is detected, an error message is displayed indicating the memory address
 * of the mismatch, along with the expected and actual byte values.
 *
 * @param rom_data   A pointer to the start address of the data read from ROM.
 * @param file_data  A pointer to the start address of the expected data.
 * @param rombank    The ROM bank number to be tested (e.g., ROM4_BANK, ROM3_BANK).
 * @return           Returns 0 if all data matches, 1 if a mismatch is found.
 */
int testSequentialReadROMBytes(unsigned char *rom_data, unsigned char *file_data, int rombank)
{
    printf("- Testing seq read bytes ROM %s...  ", rombank == ROM4_BANK ? "4" : "3");

    rom_data += rombank * ROMBANK_SIZE_BYTES;  // Move the pointer to the start of the ROM bank
    file_data += rombank * ROMBANK_SIZE_BYTES; // Move the pointer to the start of the ROM bank in the file

    for (int i = 0; i < ROMBANK_SIZE_BYTES; i++) // Change the loop size to iterate over bytes
    {
        unsigned char rom_byte = rom_data[i];
        unsigned char file_byte = file_data[i];

        if (rom_byte != file_byte)
        {
            unsigned long real_memory = rombank * ROMBANK_SIZE_BYTES + ROM4_MEMORY_START;
            real_memory += i;

            printf("\r\n    x Error: Data mismatch at address %lx. Expected: %02x, got: %02x\r\n", real_memory, file_byte, rom_byte);
            return 1;
        }

        if (i % SPINNER_UPDATE_FREQUENCY == 0)
        {
            printf("\b%c", spinner[(i / SPINNER_UPDATE_FREQUENCY) % 4]);
        }
    }

    printf("\bSuccess.\r\n");
    return 0;
}

/**
 * Compares sequential byte data read from ROM to expected data and
 * accumulates statistics of successful and failed matches.
 *
 * This function performs a byte-by-byte comparison between the data read
 * from a specified ROM bank and the expected data provided. It then tracks
 * and counts the number of successful matches and mismatches. This version
 * is intended to provide diagnostic information without stopping at the
 * first error.
 *
 * @param rom_data   A pointer to the start address of the data read from ROM.
 * @param file_data  A pointer to the start address of the expected data.
 * @param rombank    The ROM bank number to be tested (e.g., ROM4_BANK, ROM3_BANK).
 * @return           Returns the number of successful requests (matches).
 */
int testSequentialReadROMBytesStats(unsigned char *rom_data, unsigned char *file_data, int rombank)
{
    int successful_requests = 0;
    int failed_requests = 0;

    printf("- Testing stats seq read bytes ROM %s...  ", rombank == ROM4_BANK ? "4" : "3");

    rom_data += rombank * ROMBANK_SIZE_BYTES;  // Move the pointer to the start of the ROM bank
    file_data += rombank * ROMBANK_SIZE_BYTES; // Move the pointer to the start of the ROM bank in the file

    for (int i = 0; i < ROMBANK_SIZE_BYTES; i++) // Iterate over bytes
    {
        unsigned char rom_byte = rom_data[i];
        unsigned char file_byte = file_data[i];

        if (rom_byte != file_byte)
        {
            failed_requests++;
        }
        else
        {
            successful_requests++;
        }

        if (i % SPINNER_UPDATE_FREQUENCY == 0)
        {
            printf("\b%c", spinner[(i / SPINNER_UPDATE_FREQUENCY) % 4]);
        }
    }

    printf("\bSuccess: %d, Fail: %d\r\n",
           successful_requests,
           failed_requests);

    return failed_requests > 0 ? 1 : 0;
}

/**
 * Compares random byte data read from ROM to expected data.
 *
 * This function performs a byte-by-byte comparison between random bytes read
 * from a specified ROM bank and the expected data provided for a number of
 * requests. If a mismatch is found at any position, it immediately returns
 * an error.
 *
 * @param rom_data     A pointer to the start address of the data read from ROM.
 * @param file_data    A pointer to the start address of the expected data.
 * @param rombank      The ROM bank number to be tested (e.g., ROM4_BANK, ROM3_BANK).
 * @param num_requests The number of random access requests to perform.
 * @return             Returns 0 if all random bytes match, 1 if a mismatch is found.
 */
int testRandomReadROMBytes(unsigned char *rom_data, unsigned char *file_data, int rombank, int num_requests)
{
    printf("- Testing %i random access bytes from ROM %s...  ", num_requests, rombank == ROM4_BANK ? "4" : "3");

    rom_data += rombank * ROMBANK_SIZE_BYTES;  // Move the pointer to the start of the ROM bank
    file_data += rombank * ROMBANK_SIZE_BYTES; // Move the pointer to the start of the ROM bank in the file

    srand(Random()); // Initialize random seed

    for (int i = 0; i < num_requests; i++)
    {
        unsigned long random_position = rand() & (ROMBANK_SIZE_BYTES - 1); // Mask the value to be within the ROM size in bytes

        unsigned char rom_byte = rom_data[random_position];
        unsigned char file_byte = file_data[random_position];

        if (rom_byte != file_byte)
        {
            unsigned long real_memory = rombank * ROMBANK_SIZE_BYTES + ROM4_MEMORY_START;
            real_memory += random_position;
            printf("\r\n    x Error: Data mismatch at %p. Expected: %p, got: %p\r\n", real_memory, file_byte, rom_byte);
            return 1;
        }

        if (i % SPINNER_UPDATE_FREQUENCY == 0)
        {
            printf("\b%c", spinner[(i / SPINNER_UPDATE_FREQUENCY) % 4]);
        }
    }

    printf("\bSuccess.\r\n");
    return 0;
}

/**
 * Compares random byte data read from ROM to expected data and provides stats.
 *
 * This function performs a byte-by-byte comparison between random bytes read
 * from a specified ROM bank and the expected data provided for a number of
 * requests. It provides statistics of successful and failed requests.
 * If a mismatch is found at any position, the failed request count increments.
 *
 * @param rom_data     A pointer to the start address of the data read from ROM.
 * @param file_data    A pointer to the start address of the expected data.
 * @param rombank      The ROM bank number to be tested (e.g., ROM4_BANK, ROM3_BANK).
 * @param num_requests The number of random access requests to perform.
 * @return             Returns 0 if all random bytes match, 1 if any mismatches found.
 */
int testRandomReadROMBytesStats(unsigned char *rom_data, unsigned char *file_data, int rombank, int num_requests)
{
    int successful_requests = 0;
    int failed_requests = 0;

    printf("- Testing stats %i random access bytes ROM %s...  ", num_requests, rombank == ROM4_BANK ? "4" : "3");

    rom_data += rombank * ROMBANK_SIZE_BYTES;  // Move the pointer to the start of the ROM bank
    file_data += rombank * ROMBANK_SIZE_BYTES; // Move the pointer to the start of the ROM bank in the file

    srand(Random()); // Initialize random seed

    for (int i = 0; i < num_requests; i++)
    {
        unsigned long random_position = rand() & (ROMBANK_SIZE_BYTES - 1); // Ensure the value is within the ROM size in bytes

        unsigned char rom_byte = rom_data[random_position];
        unsigned char file_byte = file_data[random_position];

        if (rom_byte != file_byte)
        {
            failed_requests++;
        }
        else
        {
            successful_requests++;
        }

        if (i % SPINNER_UPDATE_FREQUENCY == 0)
        {
            printf("\b%c", spinner[(i / SPINNER_UPDATE_FREQUENCY) % 4]);
        }
    }

    printf("\bSuccess: %d, Fail: %d\r\n",
           successful_requests,
           failed_requests);

    return failed_requests > 0 ? 1 : 0;
}

int load_binary_file(unsigned char **data, long *file_size)
{
    FILE *file;

    // Open the binary file
    file = fopen(TEST_ROM_FILE, "rb");
    if (!file)
    {
        fprintf(stderr, "Failed to open testrom.bin\n");
        return 1;
    }

    // Get the size of the file
    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    rewind(file);

    // Allocate memory for the data
    *data = (unsigned char *)malloc(sizeof(unsigned char) * (*file_size));
    if (!*data)
    {
        fprintf(stderr, "Failed to allocate memory\n");
        fclose(file);
        return 1;
    }

    // Read the file into the data array
    size_t read_size = fread(*data, 1, *file_size, file);
    if (read_size != *file_size)
    {
        fprintf(stderr, "Error reading testrom.bin\n");
        free(*data);
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}

//================================================================
// Main program
int run()
{

    FILE *fp;
    unsigned char *ptr;
    size_t length;

    ScreenContext screenContext;
    initScreenContext(&screenContext);

    if (screenContext.savedResolution == HIGH_RES)
    {
        __uint16_t palette[4] = {0xFFF, 0x000};
        initHighResolution(palette);
    }
    else
    {
        __uint16_t palette[4] = {0xFFF, 0x000, 0x000, 0x000};
        initMedResolution(palette);
    }

    printf("\r");
    printf("ATARI ST SIDECART ROM TEST. V%s - (C)2023 Diego Parrilla / @soyparrilla\r\n", VERSION);

    unsigned char *rom_memory = NULL;
    unsigned char *data = NULL;
    long file_size = 0;

    int result = load_binary_file(&data, &file_size);
    if (result == 0)
    {
        if (file_size != 128 * 1024)
        {
            printf("x Error: testrom.bin must be 128KB\r\n");
        }
        else
        {
            printf("- testrom.bin loaded\r\n");
            printf("- testrom.bin size: %ld bytes\r\n", file_size);

            rom_memory = (unsigned char *)ROM_MEMORY_START;
            printf("- ROM memory address final release: %p\r\n", (void *)rom_memory);

            testDifferentVersions(rom_memory);

            // Sequential access tests
            // Words access
            testSequentialReadROM(rom_memory, data, ROM4_BANK);
            testSequentialReadROM(rom_memory, data, ROM3_BANK);
            testSequentialReadROMStats(rom_memory, data, ROM4_BANK);
            testSequentialReadROMStats(rom_memory, data, ROM3_BANK);
            // Bytes access
            testSequentialReadROMBytes(rom_memory, data, ROM4_BANK);
            testSequentialReadROMBytes(rom_memory, data, ROM3_BANK);
            testSequentialReadROMBytesStats(rom_memory, data, ROM4_BANK);
            testSequentialReadROMBytesStats(rom_memory, data, ROM3_BANK);

            // Random access tests
            // Words access
            testRandomReadROM(rom_memory, data, ROM4_BANK, RANDOM_ACCESS_ITERATIONS);
            testRandomReadROM(rom_memory, data, ROM3_BANK, RANDOM_ACCESS_ITERATIONS);
            testRandomReadROMStats(rom_memory, data, ROM4_BANK, RANDOM_ACCESS_ITERATIONS);
            testRandomReadROMStats(rom_memory, data, ROM3_BANK, RANDOM_ACCESS_ITERATIONS);
            // Bytes access
            testRandomReadROMBytes(rom_memory, data, ROM4_BANK, RANDOM_ACCESS_ITERATIONS);
            testRandomReadROMBytes(rom_memory, data, ROM3_BANK, RANDOM_ACCESS_ITERATIONS);
            testRandomReadROMBytesStats(rom_memory, data, ROM4_BANK, RANDOM_ACCESS_ITERATIONS);
            testRandomReadROMBytesStats(rom_memory, data, ROM3_BANK, RANDOM_ACCESS_ITERATIONS);

            // By address line tests
            testAddressLinesSequentialReadROM(rom_memory, data, ROM4_BANK, ADDRESS_LINE_ITERATIONS);
            testAddressLinesSequentialReadROM(rom_memory, data, ROM3_BANK, ADDRESS_LINE_ITERATIONS);
        }
    }
    else
    {
        printf("x Error: testrom.bin not found\r\n");
    }

    // Clean up
    free(data);

    printf("Press any key to exit...\r\n");

    getchar();

    restoreResolutionAndPalette(&screenContext);
}

//================================================================
// Standard C entry point
int main(int argc, char *argv[])
{
    // switching to supervisor mode and execute run()
    // needed because of direct memory access for reading/writing the palette
    Supexec(&run);
}
