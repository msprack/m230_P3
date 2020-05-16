//Process two dates and return a positive value if date1 is greater, 0 if equal and a negative value if date1 is the lesser
int compareDates(char* date1, char* date2)
{
    while (*date1 != '\0')
    {
        if(*date2 > *date1) return -1'
        if (*date1 > *date2) return 1;
        if( *date1 == *date2) return 0;

    }
}
