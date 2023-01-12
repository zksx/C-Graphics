// preprocessors
#ifndef OBJECTS_H
#define OBJECTS_H


// header files
#include <stdbool.h>
#include <stdlib.h>
#include "ppmrw.h"
#include "v3math.h"


// global constants
extern const int DEFAULT_SHININESS;


// object codes
typedef enum ObjectCodes {
   PLANE = 200,
   SPHERE,
   LIGHT
} ObjectCodes;


// structures
typedef struct ObjectList
{
   int size;
   void *o_head;
   void *o_tail;
   void *l_head;
   void *l_tail;

} ObjectList;

typedef struct ObjectListNode
{
   ObjectCodes code;
   void *data;
   void *next_node;
} ObjectListNode;

typedef struct Camera
{
   float *position;
   float *direction;
   float vp_width;
   float vp_height;
   int frames;
} Camera;

typedef struct Light
{
   float position[3];
   float color[3];
   float radial0;
   float radial1;
   float radial2;
   float theta;
   float angular0;
   float direction[3];
}  Light;

typedef struct Plane
{
   float *position;
   float *normal;
   float *color;
   float *diffuse_color;
   float *specular_color;
   float ns;
   float reflectivity;
} Plane;

typedef struct Sphere
{
   float *position;
   float *final_position;
   float *step;
   float radius;
   float *color;
   float *diffuse_color;
   float *specular_color;
   float ns;
   float reflectivity;
   float refractivity;
   float ior;
} Sphere;


// prototypes

// camera functions
/*
Returns a new, default camera
*/
Camera *new_camera();

/*
set the members of the camera according to info given from obj_str

char *obj_str: the object string that contains info pertaining to the camera
Sphere *s: The camera object to fill with data
int mem_num: the amount of members in the camera

Returns, through parameters a camera with filled out member values
*/
void c_set(char *obj_str, Camera *s, int mem_num);


/*
set the members of the camera according to info given from obj_str

char *obj_str: the object string that contains info pertaining to the light
Sphere *s: The light object to fill with data
int mem_num: the amount of members in the light

Returns, through parameters a camera with filled out member values
*/
void l_set(char *obj_str, Light *l, int mem_num);


/*
Returns a new, replica camera

Camera *c : the camera to copy
*/
Camera *copy_camera( Camera *c );

// light functions
/*
Returns a new, default light
*/
Light *new_light();

/*
Calculates the lighting and color of an object

float *rd : direction of view vector to point
float *point : the intersection point on the surface of the object
ObjectListNode *obj : the object that was intersected
int bounces : maximum number of times to reflect the view ray
*/
float *illuminate( ObjectList *obj_list, float *rd, float *point,
                     ObjectListNode *obj, int bounces, bool inside );

// plane functions
/*
Returns a new, default plane
*/
Plane *new_plane();

/*
Determines if a ray, r, intersects a plane, p

Plane *p : the plane in question
float *ro : point the ray originates from
float *rd : the ray to test intersect on

Returns the distance from the ray's origin to the intersection point on the
plane
*/
float intersect_plane( Plane *p, float *ro, float *rd );

/*
set the members of the plane according to info given from obj_str

char *obj_str: the object string that contains info pertaining to the plane
Sphere *s: The plane object to fill with data
int mem_num: the amount of members in the plane

Returns, through parameters a plane with filled out member values
*/
void p_set(char *obj_str, Plane *p, int mem_num);

// sphere functions
/*
Returns a new, default sphere
*/
Sphere *new_sphere();

/*
Determins if a ray, r, intersects a sphere, s

Sphere *s : the sphere in question
float *ro : point the ray originates from
float *rd : the ray to test intersect on

Returns the distance from the ray's origin to the intersection point to the
plane
*/
float intersect_sphere( Sphere *s, float *ro, float *rd );

/*
Determines the ray refracted through a sphere

Sphere *s : the sphere
float *r0 : the point the ray originates
float *rd : the ray to refract

Returns the refracted ray
*/
void refract_ray(float *dst, Sphere *s, float *r0, float *rd );

/*
set the members of the sphere according to info given from obj_str

char *obj_str: the object string that contains info pertaining to the sphere
Sphere *s: The sphere object to fill with data
int mem_num: the amount of members in the sphere

Returns, through parameters a sphere with filled out member values
*/
void s_set(char *obj_str, Sphere *s, int mem_num);


// object collection functions
/*
Appends an object to the LL

ObjectList *obj_list: LL of objcts. Specifically the first node in the LL
void *obj: obj to be added to the LL
ObjectCodes codes: code to assign to the object

Returns, through parameters the object list with the newly appened node
*/
void *append_object( ObjectList *obj_list, void *obj, ObjectCodes codes );

/*
Creates a node for the LL

void *obj: object data
ObjectCodes codes: code of object

Returns the created node
*/
ObjectListNode *create_node(void *obj, ObjectCodes codes);

/*
Frees objects from the LL by cycling throught the LL

ObjectList *obj_list : the object list to free the nodes from
*/
void free_objects(ObjectList *obj_list);

//initailizes list of objects
ObjectList *init_obj_list();

/*
Counts up the amount of members in the object

obj_str: the string object that contains data pertaining to the obeject
*/
int obj_mem_num(char *obj_str);

/*
Counts up the amount of members in the object

obj_str: the string object that contains data pertaining to the obeject
*/
bool check_obj(char *obj_str);

void color_clamp( float *final_color);


#endif /* SHAPES_H */
