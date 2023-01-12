// preprocessors
#ifndef RAYCAST_H
#define RAYCAST_H


// header files
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "file_reader.h"
#include "ppmrw.h"
#include "objects.h"
#include "v3math.h"


// global constants
extern const int MAX_COLOR_VAL;


// prototypes
/*
Shoots a ray into the scene and gives back the distance to the nearest
intersection point, and corresponding object

ObjectList *obj_list : list of objects in the scene
float *ro : origin of the ray
float *rd : direction of ray
ObjectListNode *curr_obj : the object to test with respect to; if not NULL, then
                           curr_obj is skipped in the intersection loop
ObjectListNode **near_obj

Returns distance from ray origin to intersection point
*/
float cast_ray( float *ro, float *rd, ObjectListNode *curr_obj,
                  ObjectListNode **near_obj, ObjectList *obj_list );

/*
Fills an image buffer with data that describes the scene from the specified
camera's angle

uint8_t *img_buffer : block of mem to put the constructed image into
Camera *cam : the camera to render the scene from
ObjectList *obj_list : list of objects in the scene
*/
void get_image( uint8_t *img_buffer, int img_width, int img_height, Camera *cam,
                  ObjectList *obj_list );

/*
Reads the scene file for objects and camera; assumes the scene file has already
been successfully opened

Camera *cam : the variable that will be a reference to the camera
FILE *scene_h : handle to the scene file

Returns a list of objects that are in the scene file
*/
ObjectList *get_scene( Camera **cam, FILE *scene_h );

/*
Uses a color vector to fill in image buffer

uint8_t *img_buffer : block of mem for the image
int pixel_x : x coord of the current pixel
int pixel_y : y coord of the current pixel
float *color : the color vector
*/
void illuminate_pixel( uint8_t *img_buffer, int img_width, int pixel_x,
                        int pixel_y, float *color );

/*
Uses an object to fill in image buffer

uint8_t *img_buffer : block of mem for the image
int pixel_x : x coord of the current pixel
int pixel_y : y coord of the current pixel
ObjectListNode *obj : object to paint the pixel with respect to
*/
void paint_pixel( uint8_t *img_buffer, int img_width, int pixel_x, int pixel_y,
                  ObjectListNode *obj );

/*
Prints the usage documentation for raycast program to the console
*/
void p_r_usage();

/*
checks to make sure the args of the program are legal
int argc : count of arguments
char **argv : values of argument
*/
void check_args( int argc, char **argv );

/*
moves all objects in a secene by their step
*/
void move_objects(ObjectList *objects);

ObjectList *get_scene_new(Camera **cam, FILE *scene_h);


#endif /* RAYCAST_H */
