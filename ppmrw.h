// preprocessors
#ifndef PPMRW_H
#define PPMRW_H


// header files
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>


// constants
extern const char END_LINE;
extern const char POUND;
extern const char SPACE;
extern const int CHANNELS;
extern const int NUM_OF_ARGS;


// typedefs
typedef struct file_header
{
    char magic_num[ 2 ];
    int width;
    int height;
    int chars_to_body;
} file_header;


// prototypes
/*
Finds the number of characters it takes to get to the data of the PPM file

FILE *file : the stream to read from
char char_store : the first number found
*/
int chars_to_data(FILE *in_file);

/*
Converts a character string of numbers to an integer data type

FILE *file : the stream to read from
char char_store : the first number found
*/
int read_number( FILE *file, char char_store);

/*
Reads the header of a file, constructs a file_header, and determines the format
(P3 or P6)

FILE *file : the stream to read from
file_header *header : store for file header data from incoming file
*/
uint8_t *main_read( FILE *file, file_header *header );

/*
Writes header data to new file, and determines which how to write the data
(P3 or P6)

char *out_file_name : name of the file to be written to
char format : the format specified by the user
file_header *header : header data from incoming file
data stream
*/
void main_write( const char *out_file_name, const char *format,
             file_header *header, uint8_t *pixmap );

/*
Returns an default intialized file_header
*/
file_header *init_file_header();

/*
Prints program call formatting and option(s) to the terminal
*/
void ppmrw_usage();

/*
Reads image data from a P3 file

FILE *file : the stream to read from
int width : width of image
int height : height of image
*/
uint8_t *read_p3( FILE *in_file, int width, int height);

/*
Reads image data from a P6 file

FILE *file : the stream to read from
int width : width of image
int height : height of image
*/
uint8_t *read_p6( FILE *in_file, int width, int height);

/*
Writes to a file in P3 format

int width : width of the image
int height : height of the image
data
*/
void write_p3( FILE *out_file, int width, int height, uint8_t *pixmap );

/*
Writes to a file in P6 format

int width : width of the image
int height : height of the image
data
*/
void write_p6( FILE *out_file, int width, int height, uint8_t *pixmap );

#endif /* PPMRW_H */
