// header files
#include "ppmrw.h"


// constants
const char END_LINE = '\n';
const char POUND = '#';
const char SPACE = ' ';
const int CHANNELS = 3;
const int NUM_OF_ARGS = 4;

int chars_to_data(FILE *in_file)
{
   int s_nl_count = 0;
   int chars_to_body = 0;
   int const spaces_nl_to_body = 4;

   // use fgetc to find 255 channel
   char char_store = fgetc( in_file );
   
   // while the fourth space or newline hasn't be parsed
   while (s_nl_count != spaces_nl_to_body)
   {
      // check if it's a space or endline
      if (char_store == SPACE || char_store == END_LINE)
      {
         s_nl_count++;
      }

      // otherwise check for a comment
      else if (char_store == POUND)
      {
         // loop to the end of the comment
         while (char_store != END_LINE)
         {
            char_store = fgetc( in_file );
            chars_to_body++;
         }
      }
      
      char_store = fgetc( in_file );
      chars_to_body++;
   }
   // return number of chars to body
   return chars_to_body;
}

// implementation
int read_number( FILE *file, char char_store )
{
   // converting int num char to num int
   int num = (int)( char_store - '0' );

   // place the the next character into char_store
   char_store = fgetc( file );

   // read until separating space
   while ( isdigit( char_store ) )
   {
      // advance number to next 10's spot
      num *= 10;

      // convert and add found number
      num += (int)( char_store - '0' );

      // place the the next character into char_store
      char_store = fgetc( file );
   }

   // return the integer version of char_store
   return num;
}

uint8_t *main_read( FILE *file, file_header *header )
{
   bool found_mn = false; // flag for finding the magic number
   bool found_w_n_h = false; // flag for finding width and height
   bool in_header = true;

   // getting first character of the file, should be 'P'
   char char_store = fgetc( file );

   header->chars_to_body += 1;

   // read the header, while the char_store is not at the end of the file
   // or the width/height and magic number are not found
   while ( char_store != EOF || ( !found_w_n_h && !found_mn ) )
   {

      // check if the char_store is an endline and in the header
      if ( char_store == END_LINE && in_header )
      {
         header->chars_to_body += 1;
      }

      // check for magic number - line beginning with P
      if ( char_store == 'P' && !found_mn )
      {
         // stores the 'P' into magic_num at index 0
         header->magic_num[ 0 ] = char_store;

         // move to next character in file and store in char_store
         char_store = fgetc( file );

         // store the number beind 'P' in this case '3' or '6' into magic_num
         // index 1
         header->magic_num[ 1 ] = char_store;

         // set found magic number to true
         found_mn = true;
      }
      // otherwise check for a pound sign indicating a comment
      else if ( char_store == POUND )
      {
         // advance to end of line
         while( char_store != END_LINE )
         {
            char_store = fgetc( file );

         }
      }
      // otherwise check for a digit starting the line
      else if ( isdigit( char_store ) && !found_w_n_h )
      {
         // get the widths
         header->width = read_number( file, char_store);

         // advance to ideally get the height
         char_store = fgetc( file );

         // check for a comment between the height and width
         while ( char_store == POUND )
            {
               // advance to end of line
               while( char_store != END_LINE )
               {
                  // moving char store to end line
                  char_store = fgetc( file );

               }
               char_store = fgetc( file );
            }

         // get the height
         header->height = read_number( file, char_store);
         found_w_n_h = true;
         in_header = false;
      }

      // advance to next character in file
      char_store = fgetc( file );
   }


   // check magic number is "P3"
   if ( header->magic_num[ 1 ] == '3'  )
   {
      // fill file_body_data with the body data from the file by using read_p3
      return read_p3( file, header->width,  header->height);
   }
   // assume magic number is "P6"
   else
   {
      // read_p6
      return read_p6( file, header->width,  header->height);
   }
}

void main_write( const char *out_file_name, const char *format,
             file_header *header, uint8_t *pixmap )
{
   // file handle to out file
   FILE *ofh = fopen( out_file_name, "w" );

   // check format is "P3"
   if ( *format == (char)( '3' ) )
   {
      // write header data
      fprintf( ofh, "P3\n" );
      fprintf( ofh, "%d %d\n%d\n", header->width, header->height, 255 );

      // write data
      write_p3( ofh, header->width, header->height, pixmap );
   }
   // assume format is "P6"
   else
   {
      // write header data
      fprintf( ofh, "P6\n" );
      fprintf( ofh, "%d %d\n%d\n", header->width, header->height, 255 );

      // write data
      write_p6( ofh, header->width, header->height, pixmap );
   }

    // close file
   fclose( ofh );
}

file_header *init_file_header()
{
   file_header *h = ( file_header * )malloc( sizeof( file_header ) );

   h->width = 0;
   h->height = 0;

   return h;
}

void ppmrw_usage()
{
   fprintf(stderr,
   "usage: ./ppmrw [format] [in-file name] [out-file name]\n"
   "format : 3 or 6 - specifies out-file is P3 or P6\n"
   "in-file name : name of the file to convert\n"
   "out-file name : name of file to be written to\n\n");
}

// returns an char array of the bodydata
uint8_t *read_p3( FILE *in_file, int width, int height )
{
   int body_len = width * height * CHANNELS;
   int mapdex = 0;
   int cursor_set = 0;

   // allocate mem for pixel map
   uint8_t *pixmap = (uint8_t *)malloc( body_len * sizeof( uint8_t ) );

   // reset cursor
   fseek( in_file, 0, SEEK_SET );

   // set cursor to body data
   cursor_set = chars_to_data(in_file);

   // move past header
   fseek( in_file, cursor_set, SEEK_SET );

   // while the mapdex is less then the body length
   while( mapdex < body_len )
   {
      // use scanf to place
      fscanf( in_file, "%hhu", &pixmap[ mapdex ] );

      mapdex += 1;
   }

   // return the array of pixmap data
     return pixmap;
}

uint8_t *read_p6( FILE *in_file, int width, int height )
{
   // grab body length of the file
   int body_len = width * height * CHANNELS;

   int cursor_set = 0;

   // allocate mem for pixel map
   uint8_t *pixmap = (uint8_t *)malloc( body_len * sizeof( uint8_t ) );

   // reset cursor
   fseek( in_file, 0, SEEK_SET );

   // set cursor to body data
   cursor_set = chars_to_data(in_file);
   
   // fseek to where the
   fseek( in_file, cursor_set, SEEK_SET );

   // place data from the body of in_file into pixmap using fread
   fread( pixmap, sizeof( uint8_t ), width * height * CHANNELS, in_file );

   // return pixmap
   return pixmap;
}

void write_p3( FILE *out_file, int width, int height, uint8_t *pixmap )
{
   // grab body length of the file
   int body_len = width * height * CHANNELS;
   int mapdex = 0;

   // while the index is less then the length of the body file
   while ( mapdex < body_len )
   {
      // use fprintf to output data from pixmap to the out file
      fprintf( out_file, "%hhu\n", pixmap[ mapdex ] );

      // increase index
      mapdex += 1;
   }
}

void write_p6( FILE *out_file, int width, int height, uint8_t *pixmap )
{
   // use fwrite to the the data from pixmap into the out_file
   fwrite( pixmap, sizeof( uint8_t ), width * height * CHANNELS, out_file );
}

