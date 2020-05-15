#include <stdio.h>
#include <stdlib.h>

#define RED "\033[0;31m"
#define DEFAULT "\033[0m"

#define FBUFLEN 4096
#define LBUFLEN 1024

// TODO: Get to a point where we can verify code is being read correctly by outputting the csv to terminal.
// TODO: Create a struct that organizes all 5 fields.
// TODO: Establish a dynamically allocated array of covid_struct
// TODO: Date Parsing, need to be able to parse dates from text to ensure dates are in order.
// TODO: Implement a sort algorithm to sort the array by date from earliest to latest.
// TODO: Generate output from the sorted array. 

// These are called protoypes. They allow us to implement main first without the compiler freaking out.
// The functions are implemented below the main function.

// STRCMP is basically ripped from the string.h library. Didn't feel like importing string.h
int STRCMP(const char* str1, const char* str2);
// push allows us to use an array as if it were a stack, this way the average allows us to 
void push(int* set, int new, int length);

int main(int argc, char *argv[]) {
    
    static char fileBuffer[FBUFLEN]; // A 4K character buffer for fread to feed.
    static char fieldBuffer[LBUFLEN]; // A generous line buffer.
    int size; // The size in bytes of the data read back from fread.
    int fieldLength=0;
    char* current;
    if (argc != 3) {
        fprintf(stderr, RED);
        fprintf(stderr, "It would appear that you supplied the wrong number of arguments.\n");
        fprintf(stderr, "%s file_name.csv ISO_Code\n", argv[0]);
        fprintf(stderr, DEFAULT);
        exit(1);
    }
    char *ISO = argv[2];
    char *path = argv[1];

    // index 0: ISO country code
    // index 1: Location
    // index 2: Date
    // index 3: total_cases
    // Index 4: new_cases
    int positions[] = {-1, -1, -1, -1, -1};

    FILE *pFile = fopen (argv[1], "r"); // Open the file

    // Error handling for failed open.
    if (pFile == NULL) {
        fprintf(stderr, RED);
        fprintf(stderr, "Could not open %s.", argv[1]);
        fprintf(stderr, DEFAULT);
        exit(1);
    }

    //Feed the file into buffer, 4096 chars at a time until there is nothing to read.
    while (0 < (size = fread(fileBuffer, 1, sizeof(fileBuffer), pFile))) {
        // Reset the position of the current character, every buffer pull.
        int position = 0;
        // Read lines until told to break
        while (1) {

            // Read until the end of the line or the end of the buffer
            int lineLength; // The farthest position of the current line
            for(lineLength = 1; position < size && fileBuffer[position] != '\n'; position++) {
                // Check for the end of the current field.
                if (fileBuffer[position] != ',') {
                    //TODO: Implement checking ISO
                }
                lineLength++;
            }
            //At the end of the buffer, rewind the file to the beginning of the current line
            if(position == size) { // TODO: Make sure the position would be the size at the end.
                if (size==FBUFLEN) {
                    // lineLength should be one greater than the index of the farthest character, allowing fread to start at the beginning of the next line.
                    fseek(pFile, (-1 * lineLength), SEEK_CUR);
                }
                break;
            } else {
                //process averages...
            }
        }
    }
}

int STRCMP(const char* str1, const char* str2) {
    const unsigned char *s1 = (const unsigned char *) str1;
    const unsigned char *s2 = (const unsigned char *) str2;
    unsigned char c1, c2;

    do {
        c1 = (unsigned char) *s1++;
        c2 = (unsigned char) *s2++;
        
        if (c1 =='\0')
            return c1-c2;
    } while (c1 == c2);

    return c1 - c2;
}

void push(int* set, int new, int length) {
    // set every value from index 0 through index 6 to be the value thats ahead of it.
    for(int i = 1; i < length; i++) {
        set[i-1] = set[i];
    }
    // set the value of the final index to be new
    set[length-1] = new;
}