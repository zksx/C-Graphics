// preprocessors
#ifndef FILE_READER_H
#define FILE_READER_H


// header files
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


// global constants
extern const char NULL_CHAR;
extern const char COMMA;
extern const char COLON;
extern const char NEW_LINE;
extern const char SPACE;


// prototypes
/*
Reads from a file to a specified delimiter

char *buffer : block of mem to put the string into from the file
const FILE *file_handle : the file to read from
const char delimiter : the character that signifies the end of a string
*/
void read_to_delimiter( char *buffer, FILE *file_handle, const char delimiter );

/*
Reads from a string to a specified delimiter

char *buffer : block of mem to put the string into from the file
const char *str_in : the string to read from
const char delimiter : the character that signifies the end of a string
int offset : number of chars to offset the cursors start by
int dem_skip : number of delimters that can go by without stopping
*/
void read_to_delimiter_str( char *buffer, char *str_in, const char delimiter, 
                                                            int *offset, int dem_skip );

/*
Gets the size of the file

FILE *file: file to get size of

Returns, size of file
*/
int size_of_file(FILE *file);

void read_obj( char *buffer, FILE *file_handle );

#endif /* FILE_READER_H */
