// header files
#include <stdio.h>
#include <stdbool.h>
#include "file_reader.h"


// global constants
const char NULL_CHAR = '\0';
const char COMMA = ',';
const char COLON = ':';
const char NEW_LINE = '\n';
const char SPACEE = ' ';


// implementation
void read_to_delimiter( char *buffer, FILE *file_handle, const char delimiter )
{
    // index for placing values in buffer

   // getting intial character from file
   char file_c = fgetc( file_handle );

   // loop while the the current character isn't EOF or isn't the delimiter
   for( int i = 0; file_c != EOF && file_c != delimiter; i+=1 )
   {
      // place character in the buffer to form a string
      buffer[ i ] = file_c;

      // move to the next character in the file
      file_c = fgetc( file_handle ); // skip over delimiter
      
   }
}
// implementation
void read_to_delimiter_str( char *buffer, char *str_in, const char delimiter, 
                                                      int *offset, int dem_skip)
{
   int i = 0; // index for placing values in buffer
   int str_i = *offset;
   int dem_num = 0;

   // getting intial character from file
   char cur_char = str_in[*offset];
   
   // loop while the the current character isn't EOF or isn't the delimiter
   while( cur_char != NULL_CHAR && (cur_char != delimiter || dem_num <= dem_skip ) )
   {
      // place character in the buffer to form a string
      buffer[ i ] = cur_char;

      // increase index by 1
      i += 1;

      // increase str_i by 1
      str_i += 1;

      // grab next char from str_in
      cur_char = str_in[str_i];

      if (cur_char == delimiter)
      {
         dem_num += 1;
      }
      
   }

   *offset += i;
}

int size_of_file(FILE *file)
{
   // file can't be NULL at this point

   unsigned int size = 0;
   fseek(file, 0, SEEK_END);
   size = ftell(file);
   rewind(file);

   return size;
}

// implementation
void read_obj( char *buffer, FILE *file_handle )
{
   // getting intial character from file
   char curr_c = fgetc( file_handle );;

   // getting previous char
   char prev_c = curr_c;

   // loop while the the current character isn't EOF and we still have members to scan
   for( int i = 0; (curr_c != EOF); i+=1 )
   {

      // check that the current letter isn't space
      if (curr_c != SPACEE)
      {
         // get previous char
         prev_c = curr_c;
      }

      // place character in the buffer to form a string
      buffer[ i ] = curr_c;

      // move to the next character in the file
      curr_c = fgetc( file_handle );

      if (curr_c == NEW_LINE && prev_c != COMMA)
      {
         break;
      }
      
      
   }
}
