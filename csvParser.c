#include <stdio.h>
#include <stdlib.h>

#define RED "\033[0;31m"
#define DEFAULT "\033[0m"

// These are called protoypes. They allow us to implement main first without the compiler freaking out.
// The functions are implemented below the main function.

// STRCMP is basically ripped from the string.h library. Didn't feel like importing string.h
int STRCMP(const char* str1, const char* str2);
// push allows us to use an array as if it were a stack, this way the average allows us to 
void push(int* set, int new, int length);

int main(int argc, char *argv[]) {
    
    static char fileBuffer[4096]; // A 4096 character buffer for fread to feed.
    static char *fieldBuffer = malloc()
    int size, lineLength;
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
    int positions[] = {-1, -1, -1, -1, -1};

    FILE *pFile = fopen (argv[1], "r");
    if (pFile == NULL) {
        fprintf(stderr, RED);
        fprintf(stderr, "Could not open %s.", argv[1]);
        fprintf(stderr, DEFAULT);
        exit(1);
    }
    //Feed the file into buffer, 4096 bytes at a time.
    while (0 < (size = fread(fileBuffer, 1, sizeof(fileBuffer), pFile))) {

        int position = 0;
        // Read lines until told to break
        while (TRUE) {
            // Read until the end of the line or the end of the file
            lineLength=0;
            for(; position < size && buffer[position] != '\n'; position++) {
                if (buffer[position] != ',') {
                    fieldBuffer[fieldPos]
                }
                lineLength++;
            }
            //At the end of the buffer, rewind the file to the beginning of the current line
            if(position == size) {
                if (size==sizeof(fileBuffer)) {
                    fseek(pFile, (-1 * lineLength), SEEK_CUR)
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
    for(int i = 1; i < length; i++) {
        set[i-1] = set[i];
    }
    set[length-1] = new;
}