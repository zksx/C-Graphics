// header files
#include "raycast.h"


// constants

// number of members per object type.
// maybe try dynimcally calculating the amount of members some how
const int CAM_MEMS = 2;
const int SPH_MEMS = 7;
const int PLA_MEMS = 4;
const int NUM_ARGS = 5;
const float COLOR_DEFAULT[3] = {0,0,0};
const int DEFAULT_BOUNCES = 5;

// global constants
const int MAX_COLOR_VAL = 255;

enum error_codes
{
   NOT_FIVE_ARGS = 1,
   JSON_FILE_MISSING,
   PPM_FILE_MISSING,
   FILE_NOT_FOUND,
   CAMERA_NOT_FOUND,
   INVALID_OBJECT
};


// main
int main(int argc, char **argv)
{
   // check that args are valid
   check_args(argc, argv);

   char file_out[256];

   // open scene file
   FILE *scene_handle = fopen(argv[3], "r");

   // check for successful opening of scene file
   if (scene_handle != NULL)
   {
      int img_width = atoi(argv[1]);  // convert argv[ 1 ] to integer
      int img_height = atoi(argv[2]); // convert argv[ 2 ] to integer


      // allocate memory for image buffer; tried making this into a regular array and
      // it breaks when the image is a large size like 5000x5000
      uint8_t *img_buffer = (uint8_t *)malloc(img_width * img_height * CHANNELS * sizeof(uint8_t));

      // zero array
      for (int i=0; i<img_width * img_height * CHANNELS; ++i)
      {
         img_buffer[i] = 0;
      }

      // make space for camera
      Camera *camera = NULL;

      // set object list equal to result of
      // method: get_scene
      ObjectList *obj_list = get_scene(&camera, scene_handle);

      //increment through number of frames

      for( int i = 0; i < camera->frames; ++i )
      {
         sprintf( file_out, "%s_%d.ppm", argv[4], i);

               // open file to write to
         FILE *fh_w = fopen(file_out, "w");

         // open file header
         file_header *header = init_file_header();

         header->width = img_width;
         header->height = img_height;

         get_image(img_buffer, img_width, img_height, camera, obj_list);

         main_write( file_out, "3", header, img_buffer );         move_objects(obj_list);

         free(header);
         fclose(fh_w);
      }

      // free things
      free_objects(obj_list);
      free(camera->position);
      free(camera->direction);
      free(camera);
      free(obj_list);
      free(img_buffer);
      fclose(scene_handle); // causing errors when trying to close
   }
   // otherwise, assume file could not be found
   else
   {
      // print to stderr that file could not be opened
      fprintf(stderr, "\nError: scene file could NOT be opened\n\n");

      // abort with file not found err code
      exit(FILE_NOT_FOUND);
   }

   return 0;
}

// implementation
float cast_ray( float *ro, float *rd, ObjectListNode *curr_obj,
                  ObjectListNode **near_obj, ObjectList *obj_list )
{
   float curr_t = HUGE_VAL;
   float near_t = HUGE_VAL;

   ObjectListNode *wrkng_obj = obj_list->o_head;

   // loop across objects
   while ( wrkng_obj != NULL )
   {
      // check working object does not point to curr_obj
      if ( wrkng_obj != curr_obj )
      {
         // check the working object is a plane
         if ( wrkng_obj->code == PLANE )
         {
            // test for intersection with plane
            curr_t = intersect_plane( (Plane *)wrkng_obj->data, ro, rd );
         }
         // otherwise, assume working object is a sphere
         else
         {
            // test for intersection with sphere
            curr_t = intersect_sphere( (Sphere *)wrkng_obj->data, ro, rd );
         }

         // check current intersection distance is less than the nearest
         if ( 0 < curr_t && curr_t < near_t )
         {

            // update near_t
            near_t = curr_t;

            if ( near_obj != NULL )
            {
               // update near_obj
               *near_obj = wrkng_obj;
            }
         }
      }

      // advance working object
      wrkng_obj = wrkng_obj->next_node;
   }

   // release memory
   free( wrkng_obj ); // assumed to always be NULL

   // return near_t
   return near_t;
}

// checks args
void check_args(int argc, char **argv)
{
   // check for 5 arguments
   if (argc != NUM_ARGS)
   {
      // print program usage and exit
      p_r_usage();
      exit(NOT_FIVE_ARGS);
   }
}

void get_image( uint8_t *img_buffer, int img_width, int img_height, Camera *cam,
               ObjectList *obj_list )
{
   // pixel information
   float dx = cam->vp_width / img_width;
   float dy = cam->vp_height / img_height;
   float pixel[3] = {0,0,-1};

   // allocate mem for ray vector
   float ray[3] = {0,0,0};

   // illumination and collision information
   float t = 0;
   float *f_color = NULL;
   float t_point[ 3 ] = { 0, 0, 0 };
   //float color[ 3 ] = { 0, 0, 0 };
   ObjectListNode *near_obj = NULL;

   // iterate through image rows
   for ( int row = 0; row < img_height; row += 1 )
   {
      // get current pixel y coord
      pixel[ 1 ] = ( cam->vp_height / 2 ) - 0.5 * dy - row * dy;

      // iterate through image columns
      for ( int col = 0; col < img_width; col += 1 )
      {
         // get current pixel x coord
         pixel[ 0 ] = -( cam->vp_width / 2 ) + 0.5 * dx + col * dx;

         // normalize ray vector; assume orthographic image
         v3_normalize( ray, pixel );

         // get intersection
         // method: cast_ray
         t = cast_ray( pixel, ray, NULL, &near_obj, obj_list );

         // calculate collision point
         t_point[ 0 ] = pixel[ 0 ] + ray[ 0 ] * t;
         t_point[ 1 ] = pixel[ 1 ] + ray[ 1 ] * t;
         t_point[ 2 ] = pixel[ 2 ] + ray[ 2 ] * t;

         f_color = illuminate( obj_list, ray, t_point, near_obj, 20, false );

         //printf( "%d", col );

         v3_scale(f_color, MAX_COLOR_VAL);


         // illuminate pixel
         illuminate_pixel( img_buffer, img_width, row, col, f_color );

         // reset near object pointer
         near_obj = NULL;

         // free and set f_color to NULL
         free(f_color);
         f_color = NULL;
      }
   }
}

ObjectList *get_scene(Camera **cam, FILE *scene_h)
{
   Camera *c = NULL;
   ObjectList *obj_list = init_obj_list();
   int s_size = size_of_file(scene_h);
   char obj_str[s_size ];
   bool valid_obj = true;

   memset(obj_str, '\0', sizeof(obj_str));



   // while NOT at end-of-file
   while (!feof(scene_h))
   {

      // read object to string
      read_obj(obj_str, scene_h);

      // check for camera
      if (strstr(obj_str, "camera"))
      {
         // new camera
         c = new_camera();

         // find # of members for objects
         int mem_num = obj_mem_num(obj_str);

         // set camera members
         c_set(obj_str, c, mem_num);

      }

      // otherwise, check for plane
      else if (strstr(obj_str, "plane"))
      {
         Plane *p = new_plane();

         // find # of members for objects
         int mem_num = obj_mem_num(obj_str);

         // set plane members
         p_set(obj_str, p, mem_num);

         // append plane to object list
         append_object(obj_list, p, PLANE);
      }

      // otherwise, check for sphere
      else if (strstr(obj_str, "sphere"))
      {
         // set sphere members
         Sphere *s = new_sphere();

         // find # of members for objects
         int mem_num = obj_mem_num(obj_str);

         s_set(obj_str, s, mem_num);

         //set step
         float tmp[3];
         v3_subtract(tmp, s->final_position, s->position);
         v3_scale(tmp, 1.0f / c->frames);
         v3_copy_vector(s->step, tmp);

         // append plane to object list
         append_object(obj_list, s, SPHERE);
      }

      // check for light
       else if (strstr(obj_str, "light"))
      {
         // set sphere members
         Light *l = new_light();

         // find # of members for objects
         int mem_num = obj_mem_num(obj_str);

         // set light properties
         l_set(obj_str, l, mem_num);


         // append light to object list
            //method: appendObject
         append_object(obj_list, l, LIGHT);

      }


      // otherwise assume valid may be invalid
      else
      {
         // check for invalid objects
         valid_obj = check_obj(obj_str);

         if (!valid_obj)
         {
            printf("ERROR: INVALID OBJECT OR FORMAT\n");
            exit(INVALID_OBJECT);
         }

      }

   // clear string
   memset(obj_str, '\0', sizeof(obj_str));

   }

   // check temporary camera is NULL
   if (c == NULL)
   {
      // abort with CAMERA_NOT_FOUND
      exit(CAMERA_NOT_FOUND);
   }
   // otherwise, assume a camera was found
   *cam = copy_camera(c); // this function has the possibility of causeing mem
                          // leaks because of how the v3_copy_vector()
                          // function works

   // release temporary camera mem to system
   free(c->position);
   free(c->direction);
   free(c);

   // return object list
   return obj_list;
}

void illuminate_pixel( uint8_t *img_buffer, int img_width, int pixel_x,
                        int pixel_y, float *color )
{
   int i = ( pixel_x * img_width + pixel_y ) * CHANNELS;

   // set red channel
   img_buffer[ i ] = color[ 0 ];

   // set green channel
   img_buffer[ i + 1 ] = color[ 1 ];

   // set blue channel
   img_buffer[ i + 2 ] = color[ 2 ];
}

void paint_pixel( uint8_t *img_buffer, int img_width, int pixel_x, int pixel_y,
                  ObjectListNode *obj )
{
   // default color is black
   float color[ 3 ] = { 0, 0, 0 };

   // check that obj points to something and check that the object is a plane
   if ( obj != NULL && obj->code == PLANE )
   {
      Plane *data = (Plane *)obj->data;

      v3_copy_vector( color, data->color );
      v3_scale( color, MAX_COLOR_VAL );

      data = NULL;
      free( data );
   }
   // otherwise, assume the object is a sphere and check that obj does not point
   // to NULL
   else if ( obj != NULL )
   {
      Sphere *data = (Sphere *)obj->data;

      v3_copy_vector( color, data->color );
      v3_scale( color, MAX_COLOR_VAL );

      data = NULL;
      free( data );
   }

   // calculate pixel
   int i = ( pixel_x * img_width + pixel_y ) * CHANNELS;

   // set red channel
   img_buffer[ i ] = color[ 0 ];

   // set green channel
   img_buffer[ i + 1 ] = color[ 1 ];

   // set blue channel
   img_buffer[ i + 2 ] = color[ 2 ];
}

void move_objects( ObjectList *objects )
{
   ObjectListNode *object = (ObjectListNode *)objects->o_head;

   while( object != NULL )
   {
      if( object->code == SPHERE )
      {
         float *position = ((Sphere *)(object->data))->position;
         float *step = ((Sphere *)(object->data))->step;

         float tmp[3];
         v3_add(tmp, position, step);
         v3_copy_vector(position, tmp);
      }

      object = (ObjectListNode *)object->next_node;
   }
}

void p_r_usage()
{
   // print program invokation specs
   fprintf(stderr,
           "\nusage: ./raycast [width] [height] [scene-file-name] [out-file-name]\n"
           "   width : the width of the image frame\n"
           "   height : the height of the image frame\n"
           "   scene-file-name : name of the scene file to read\n"
           "   out-file-name : name of the PPM file to write to\n\n");
}
