// header files
#include <stdio.h>
#include <stdlib.h>
#include "v3math.h"


// global constants
const int VTR_COMP_COUNT = 3;
const float RAD_TO_DEG = 180 / 3.141592654;
const float DEG_TO_RAD = 3.141592654 / 180;


// implementation

// mathematic functions
void v3_add( float *dst, float *a, float *b )
{
    dst[ 0 ] = a[ 0 ] + b[ 0 ];
    dst[ 1 ] = a[ 1 ] + b[ 1 ];
    dst[ 2 ] = a[ 2 ] + b[ 2 ];
}

float v3_angle( float *a, float *b ) // angle between a and b
{
    float product_mag = v3_length( a ) * v3_length( b );
    float a_mult_b = ( a[ 0 ] * b[ 0 ] ) + ( a[ 1 ] * b[ 1 ] ) +
                        ( a[ 2 ] * b[ 2 ] );

    return acos( a_mult_b / product_mag );
}

float v3_angle_quick( float *a, float *b ) // angle between a and b; no cos-1
{
    float product_mag = v3_length( a ) * v3_length( b );
    float a_mult_b = ( a[ 0 ] * b[ 0 ] ) + ( a[ 1 ] * b[ 1 ] ) +
                        ( a[ 2 ] * b[ 2 ] );

    return a_mult_b / product_mag;
}

void v3_cross_product( float *dst, float *a, float *b )
{
    float *_dst = (float *)malloc( VTR_COMP_COUNT * sizeof( float ) );

    _dst[ 0 ] = ( a[ 1 ] * b[ 2 ] ) - ( a[ 2 ] * b[ 1 ] );
    _dst[ 1 ] = ( a[ 2 ] * b[ 0 ] ) - ( a[ 0 ] * b[ 2 ] );
    _dst[ 2 ] = ( a[ 0 ] * b[ 1 ] ) - ( a[ 1 ] * b[ 0 ] );

    dst[ 0 ] = _dst[ 0 ];
    dst[ 1 ] = _dst[ 1 ];
    dst[ 2 ] = _dst[ 2 ];

    free( _dst );
}

float v3_dot_product( float *a, float *b )
{
    return ( a[ 0 ] * b[ 0 ] ) + ( a[ 1 ] * b[ 1 ] ) + ( a[ 2 ] * b[ 2 ] );
}

void v3_from_points( float *dst, float *a, float *b ) // form v3 from a to b
{
    float *_dst = (float *)malloc( VTR_COMP_COUNT * sizeof( float ) );

    _dst[ 0 ] = b[ 0 ] - a[ 0 ];
    _dst[ 1 ] = b[ 1 ] - a[ 1 ];
    _dst[ 2 ] = b[ 2 ] - a[ 2 ];

    dst[ 0 ] = _dst[ 0 ];
    dst[ 1 ] = _dst[ 1 ];
    dst[ 2 ] = _dst[ 2 ];

    free( _dst );
}

float v3_length( float *a )
{
    float x_sqr = a[ 0 ] * a[ 0 ];
    float y_sqr = a[ 1 ] * a[ 1 ];
    float z_sqr = a[ 2 ] * a[ 2 ];

    return sqrt( x_sqr + y_sqr + z_sqr );
}

void v3_normalize( float *dst, float *a )
{
    float a_mag = v3_length( a );

    dst[ 0 ] = a[ 0 ] / a_mag;
    dst[ 1 ] = a[ 1 ] / a_mag;
    dst[ 2 ] = a[ 2 ] / a_mag;
}

void v3_reflect( float *dst, float *v, float *n )
{
    float n_v_dot = v3_dot_product( n, v );
    float _dst[ 3 ] = { 0.0f, 0.0f, 0.0f };

    _dst[ 0 ] = -n_v_dot * n[ 0 ];
    _dst[ 1 ] = -n_v_dot * n[ 1 ];
    _dst[ 2 ] = -n_v_dot * n[ 2 ];

    dst[ 0 ] = v[ 0 ] + ( 2 * _dst[ 0 ] );
    dst[ 1 ] = v[ 1 ] + ( 2 * _dst[ 1 ] );
    dst[ 2 ] = v[ 2 ] + ( 2 * _dst[ 2 ] );
}

void v3_scale( float *dst, float s )
{
    dst[ 0 ] = s * dst[ 0 ];
    dst[ 1 ] = s * dst[ 1 ];
    dst[ 2 ] = s * dst[ 2 ];
}

void v3_subtract( float *dst, float *a, float *b )
{
    dst[ 0 ] = a[ 0 ] - b[ 0 ];
    dst[ 1 ] = a[ 1 ] - b[ 1 ];
    dst[ 2 ] = a[ 2 ] - b[ 2 ];
}


// manipulation and building functions

// !!! ALL OF THESE FUNCTIONS MIGHT CAUSE MEMORY LEAKS BECAUSE OF REASSIGNMENT !!!
float *v3_default_vector()
{
   float *_v = (float *)malloc( VTR_COMP_COUNT * sizeof( float ) );

   _v[ 0 ] = 0;
   _v[ 1 ] = 0;
   _v[ 2 ] = 0;

   return _v;
}

float *v3_init_vector( float x, float y, float z )
{
   float *_v = (float *)malloc( VTR_COMP_COUNT * sizeof( float ) );

   _v[ 0 ] = x;
   _v[ 1 ] = y;
   _v[ 2 ] = z;

   return _v;
}

void v3_copy_vector( float *dst, float *cp )
{
   dst[ 0 ] = cp[ 0 ];
   dst[ 1 ] = cp[ 1 ];
   dst[ 2 ] = cp[ 2 ];
}
