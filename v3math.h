// preprocessors
#ifndef V3MATH_H
#define V3MATH_H


// header files
#include <math.h>


// global constants
extern const int VTR_COMP_COUNT;
extern const float RAD_TO_DEG;
extern const float DEG_TO_RAD;


// prototypes

// mathematic functions
void v3_add( float *dst, float *a, float *b );
float v3_angle( float *a, float *b ); // angle between a and b
float v3_angle_quick( float *a, float *b ); // angle between a and b; no cos-1
void v3_cross_product( float *dst, float *a, float *b );
float v3_dot_product( float *a, float *b );
void v3_from_points( float *dst, float *a, float *b ); // form v3 from a to b
float v3_length( float *a );
void v3_normalize( float *dst, float *a );
void v3_reflect( float *dst, float *v, float *n );
void v3_scale( float *dst, float s );
void v3_subtract( float *dst, float *a, float *b );


// manipulation and building functions
/*
Returns memory address of a vector at scene origin
*/
float *v3_default_vector();

/*
Returns memory address to a new vector

float x : x component
float y : y component
float z : z component
*/
float *v3_init_vector( float x, float y, float z );

/*
Returns memory address to a new duplicate vector

float *dst : destination of new vector
float *cp : vector to copy
*/
void v3_copy_vector( float *dst, float *cp );


#endif /* V3MATH_H */
