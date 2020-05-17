// Michael Spracklen & Ricardo Lemus
// 1083154 & 1043159
// ms1083154@swccd.edu  rl1043159@swccd.edu

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define RED "\033[0;31m"
#define DEFAULT "\033[0m"

#define START_DATE "2020-03-01"
#define ISIZE 137
#define FBUFLEN 4096
#define LBUFLEN 1024

// A struct to make it easier to dynamically allocate an array.
typedef struct line {
    char date[11];
    int totalCases;
    int newCases;
} line;

// Some prototypes.
// Push allows us to use an array as if it were a stack, this way we can calculate a rolling average
// In real time without using the dynamically allocated array.
void push(int* set, int new, int length);

// Average calculates the average of the stack.
int average(int arr[], int index);

// Compare two dates, return the sign of the difference.
int compareDates(char* date1, char* date2);

int main(int argc, char *argv[]) {
    char delimiter = ','; // Treat the file as a csv by default.
    int stack2[2]; // The 2 day stack
    int stack7[7]; // The 7 day stack

    // Allocate 137 structs
    int dSize = ISIZE;

    // Dynamic array for those sweet, sweet 4 points.
    line* data = (line*) malloc(dSize * sizeof(*data));

    static char fileBuffer[FBUFLEN]; // A 4K character buffer for fread to feed.
    static char fieldBuffer[LBUFLEN]; // A generous line buffer.
    int size; // The size in bytes of the data read back from fread.
    int firstLine = 1; // firstLine is a boolean, dictates whether we are reading the firstline, ie the field header.

    // Check if there are 3 arguments.
    if (argc != 3) {
        fprintf(stderr, RED);
        fprintf(stderr, "It would appear that you supplied the wrong number of arguments.\n");
        fprintf(stderr, "%s file_name.csv ISO_Code\n", argv[0]);
        fprintf(stderr, DEFAULT);
        exit(1);
    }
    char *path = argv[1]; // The path to the input file.
    char *ISO = argv[2]; // The ISO code to filter with.

    char location[64]; // The place to store the location. Max country name length is 64.
    location[0] = '\0'; // If the first index of location is null, we'll know it hasn't been set yet.

    // Be resistant to variable header ordering. The only requirement is that the ISO field be first.
    // index 0: ISO country code
    // index 1: Location
    // index 2: Date
    // index 3: total_cases
    // Index 4: new_cases
    int positions[5] = {-1, -1, -1, -1, -1};

    // Allows header positions mapping with a for loop.
    char *field_names[5] = {"iso_code", "location", "date", "total_cases", "new_cases"};
    
    int dot = 0;
    int pathLength = 1; //initialize pathLength
    // Check for a .tab file extension. If so, use whitespace to delimit input.
    for (pathLength= 0; path[pathLength] != '\0'; pathLength++) {
        if(path[pathLength] == '.') {
            dot = pathLength;
        }
    }
    if (pathLength - dot ==  4 && path[pathLength-1] == 'b') {
        delimiter = ' ';
    }

    // Open our in and out files for reading and writing.
    FILE *pFile = fopen (path, "r"); // Open the file
    FILE *pOFile = fopen("out.csv", "w"); // Open the output file.
    // Error handling for failed open.
    if (pFile == NULL) {
        fprintf(stderr, RED);
        fprintf(stderr, "Could not open %s.\n", path);
        fprintf(stderr, DEFAULT);
        exit(1);
    }
    if (pFile == NULL) {
        fprintf(stderr, RED);
        fprintf(stderr, "Could not create out.csv.\n ");
        fprintf(stderr, DEFAULT);
        exit(1);
    }

    // Feed the file into buffer, 4096 chars at a time until there is nothing to read.
    // Depending on the OS, this should be faster than fgets due to caching.
    int lCount = 0;
    int isoPos=0;
    while (0 < (size = fread(fileBuffer, 1, sizeof(fileBuffer), pFile))) {

        // Reset the position of the current character, every buffer pull.
        // printf("New Frame!");
        int position = 0;
        // Read lines until told to break
        while (1) {
            //printf("Line: %d Position: %d\n", lCount, position);
            // Read until the end of the line or the end of the buffer
            int lineLength; // The farthest position of the current line
            int isISO = 0; // Is this our line our not?
            int totalCases; // Holds the totalCases for the line
            int newCases; // Holds the new cases for the line.
            char date[11]; // A null terminated date string, 10 characters. YYYY-MM-DD\0
            int fieldNumber = 0; // Tracks which field is being parsed.
            int fieldPos = 0; // The position of the fieldBuffer to write to.
            // Iterate over the cached data.
            for(lineLength = 1; position < size && fileBuffer[position] != '\n'; position++) {

                // Check for the end of the current field. Either a comma, or some whitespace.
                if (fileBuffer[position] == delimiter || (delimiter == ' ' && isspace(fileBuffer[position]))) {

                    // Null terminate the field string.
                    fieldBuffer[fieldPos] = '\0';

                    // Check this is the firstline, handle the header if so.
                    if (lCount == 0) {
                        for(int i = 0; i < 5; i++) { 
                            if (strcmp(fieldBuffer, field_names[i]) == 0) {
                                if(i != 0 && i!= 2) {
                                    printf("%s\t", fieldBuffer);
                                    fprintf(pOFile, "%s,", fieldBuffer);
                                } else if(i == 2) {
                                    printf("%s\t\t\t", fieldBuffer);
                                    fprintf(pOFile, "%s,", fieldBuffer);
                                }
                                positions[i] = fieldNumber;
                                if(i == 4) {
                                    printf("rolling_2day_avg\trolling_7day_avg\n");
                                    fprintf(pOFile, "rolling_2day_avg,rolling_7day_avg\n");
                                }
                                break;
                            }
                        }
                    } else { // Not the header, evaluate the fields.
                        if (fieldNumber == positions[0] && strcmp(fieldBuffer, ISO) == 0) {
                            isISO = 1;
                        } else if (isISO) { // Only parse nonISO fields if the ISO matches our field.
                            if (fieldNumber == positions[1] && location[0] == '\0'){
                                strcpy(location, fieldBuffer);
                            } else if (fieldNumber == positions[2] ) { // Get the position of the date within the buffer.
                                strcpy(date, fieldBuffer);
                            } else if (fieldNumber == positions[3]) { // Get the number of total cases
                                totalCases = atoi(fieldBuffer); // The atoi function parses a string to an integer.
                            } else if(fieldNumber == positions[4]) { // Get the number of new cases
                                newCases = atoi(fieldBuffer); // The atoi function parses a string to an integer
                            }
                        }
                    }
                    // In the event that there are repeated whitespace characters, fastforward.
                    if (delimiter == ' ' && isspace(fieldBuffer[position+1]) && fieldBuffer[position + 1] != '\n') {
                        while (isspace(fieldBuffer[position + 1]) && position + 1 < size) {
                            position++;
                        }
                    }
                    
                    fieldPos=0; // Reset the fieldBuffer Position
                    fieldNumber++; // Move to the next field number
                } else { // If we haven't finished the field, add the current character to the field string. Increment the fieldbuffer position.
                    fieldBuffer[fieldPos] = fileBuffer[position]; 
                    fieldPos++;
                }
                lineLength++; // We need to track the linelength so we know how much to rewind when our buffer splits lines.
            }
            // If the last character processed was a newline, then the buffer either did not split a line or we are not at the end of the buffer.
            // Either way, we can process the new line.
            if(fileBuffer[position] == '\n') {
                lCount++;
                if (isISO) { // If this is the country we're processing, do output stuff.
                    if (dSize <= isoPos) { // If the dynamic array runs out of space, double the space.
                        dSize = dSize * 2;
                        data = realloc(data, dSize * sizeof(line));
                    }
                    strcpy(data[isoPos].date, date);
                    data[isoPos].totalCases = totalCases;
                    data[isoPos].newCases = newCases;
                    // Update the rolling datasets
                    push(stack2, newCases, 2);
                    push(stack7, newCases, 7);
                    // If the date is March 1st 2020 or beyond, write to terminal and file.
                    if(compareDates(date, START_DATE) > -1) {
                        printf("%s:\t\t%s\t\t%d\t\t%d\t\t%d\t\t\t%d\n", location, date, totalCases, newCases, average(stack2, 2), average(stack7, 7));
                        fprintf(pOFile, "%s,%s,%d,%d,%d,%d\n", location, date, totalCases, newCases, average(stack2, 2), average(stack7, 7));
                    }
                    // Move on in our dynamic array.
                    isoPos++;
                }
            }
            //At the end of the buffer, rewind the file to the beginning of the current line
            if(position == size) { // If we are at the end of the buffer, check for line splitting, break out of infinite loop.
                // If we are not at the end of the file, bytes read will be equal to file buffer length. Rewind last line.
                if (size==FBUFLEN && fileBuffer[position] != '\n') { // Don't rewind if the last byte read was a newline. Very unlikely but whatever.
                    // lineLength should be one greater than the index of the farthest character, allowing fread to start at the beginning of the next line.
                    lCount--;
                    fseek(pFile, -1 * lineLength, SEEK_CUR);
                }
                break; // Escape the infinite loop to cache the next frame.
            }
            position++; // Move to the next line.
        }
    }
    // Clean up.
    free(data);
    fclose(pFile);
    fclose(pOFile);
}

void push(int* set, int new, int length) {
    // set every value from index 0 through index 6 to be the value thats ahead of it.
    for(int i = 1; i < length; i++) {
        set[i-1] = set[i]; // age off the oldest value.
    }
    // set the value of the final index to be new
    set[length-1] = new;
}

int average(int arr[], int length) {
    double sum = 0.0;
    for(int i = 0; i < length; i++) {
        sum += (double) arr[i];
    }
    double avg = sum / (double) length;
    // If the difference between avg and the integer below average is less than .5, round down.
    if (avg - (double)(int) avg < .5) {
        return (int) avg; // Use int to truncate the float. Effectively rounding down.
    } else { // Otherwise, round up.
        return (int) (avg + 1.0); // Add one to the truncated float. Effectively rounding up.
    }
}

int compareDates(char* date1, char* date2) {
    // Go through each character in date1, and date2 one at a time.
    // The dash are always in the same place.
    // Every character in each string is more significant than the date after it.
    // So, the first difference represents a larger difference than every character after it.
    // If the numbers in the buffers are different, return the differencence
    for(int i = 0; i < 10; i++) {
        if (date1[i] != date2[i]) {
            return date1[i] - date2[i];
        }
    }
    return 0;
}