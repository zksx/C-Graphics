// header files
#include "objects.h"
#include <string.h>
#include "raycast.h"
#include "file_reader.h"


// global constants
const int DEFAULT_SHININESS = 20;

// implementation

// camera functions

void c_set(char *obj_str, Camera *c, int mem_num)
{
   int str_len = strlen(obj_str);
   int offset = 0;
   char prop_str[str_len];
   memset(prop_str, '\0', sizeof(prop_str));

   // read past "camera," string
   read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

   for (int i = 0; i < mem_num; i+=1)
   {
      // camera, width: 0.69, height: 0.69
      offset += 1;

      // read to attribute name
      read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

      // check if first word contains width
      if (strstr(prop_str, "height"))
      {

         // sscanf float values into sphere-color
         sscanf(prop_str, "%*s %f", &c->vp_height);
      }

      // otherwise check it's width
      else if(strstr(prop_str, "width"))
      {

         // sscanf float values into sphere-color
         sscanf(prop_str, "%*s %f", &c->vp_width);
      }

   }
}

Camera *new_camera()
{
   Camera *c = (Camera *)malloc( sizeof( Camera ) );

   c->position = v3_default_vector();
   c->direction = v3_init_vector( 0, 0, -1 );
   c->vp_width = 0;
   c->vp_height = 0;
   c->frames = 24;

   return c;
}

Camera *copy_camera( Camera *c )
{
   Camera *_c = (Camera *)malloc( sizeof( Camera ) );
   _c->position = (float *)malloc( VTR_COMP_COUNT * sizeof( float ) );
   _c->direction = (float *)malloc( VTR_COMP_COUNT * sizeof( float ) );

   v3_copy_vector( _c->position, c->position );
   v3_copy_vector( _c->direction, c->direction );
   _c->vp_width = c->vp_width;
   _c->vp_height = c->vp_height;
   _c->frames = c->frames;

   return _c;
}


// light functions
Light *new_light()
{
   Light *l = (Light *)malloc( sizeof( Light ) );

   l->position[0] = 0;
   l->position[1] = 0;
   l->position[2] = 0;

   l->color[0] = 1;
   l->color[1] = 1;
   l->color[2] = 1;

   l->radial0 = 0;
   l->radial1 = 0;
   l->radial2 = 0;

   l->theta = 0;
   l->angular0 = 0;

   l->direction[0] = 0;
   l->direction[1] = 0;
   l->direction[2] = 0;

   return l;
}

float *illuminate( ObjectList *obj_list, float *rd, float *point,
                     ObjectListNode *obj, int bounces, bool inside )
{
   // illumination calculation information
   float t = 0;
   float ns = 0;
   float dot_value = 0;
   float radattn = 0;
   float angattn = 1;
   float distance = 0;
   float l[ 3 ] = { 0, 0, 0 };
   float l_reflected[ 3 ] = { 0, 0, 0 };
   float normal[ 3 ] = { 0, 0, 0 };
   float diffuse_color[ 3 ] = { 0, 0, 0 };
   float specular_color[ 3 ] = { 0, 0, 0 };

   // reflection and refraction information
   // allocate mem for reflected color
   float reflectivity = 0;
   float refractivity = 0.0f;
   float rd_reflect[ 3 ] = { 0, 0, 0 };
   float *reflected_color = v3_default_vector(); // this might cause a mem leak
   float refracted_color[3] = {0.0f, 0.0f, 0.0f};

   // color information
   float *final_color = v3_default_vector();

   // looping vars
   Light *light = NULL;
   ObjectListNode *wrkng_light = obj_list->l_head;

   // loop through all lights
   while ( obj != NULL && wrkng_light != NULL )
   {
      light = (Light *)wrkng_light->data;

      // get direction and distance from point to lights
      v3_from_points( l, point, light->position );
      distance = v3_length( l );
      v3_normalize( l, l );

      // cast a ray from point to the current light
      t = cast_ray( point, l, obj, NULL, obj_list );

      // check for no intersection detected
      if ( t < 0 || t == HUGE_VAL )
      {
         t = distance;
      }
      // assume there was an intersection

      // check intersection is the same distance away
      if ( t > 0 && t < distance )
      {
         wrkng_light = wrkng_light->next_node;
         continue;
      }

      // check object is a plane
      if ( obj->code == PLANE )
      {
         Plane *data = (Plane *)obj->data;

         // copy normal
         v3_copy_vector( normal, data->normal );

         // get object colors
         v3_copy_vector( diffuse_color, data->diffuse_color );
         v3_copy_vector( specular_color, data->specular_color );

         // getting shininess
         ns = data->ns;

         // getting reflectivity
         reflectivity = data->reflectivity;
         //spec_diff = 1.0f - reflectivity;

         data = NULL;
         free( data );
      }
      // otherwise, assume object is a sphere
      else
      {
         Sphere *data = (Sphere *)obj->data;

         // calculate normal
         v3_subtract( normal, point, data->position );
         v3_normalize( normal, normal );

         // get object colors
         v3_copy_vector( diffuse_color, data->diffuse_color );
         v3_copy_vector( specular_color, data->specular_color );

         // getting shininess
         ns = data->ns;

         // getting reflectivity
         reflectivity = data->reflectivity;
         refractivity = data->refractivity;

         data = NULL;
         free( data );
      }

      // reflect l relative to surface normal
      v3_reflect( l_reflected, l, normal );

      // calculate radial attenuation
      radattn = 1 / ( light->radial0 + ( light->radial1 * distance ) +
                  ( light->radial2 * ( distance * distance ) ) );

      // check for point light
      if ( light->theta == 0 || v3_length( light->direction ) == 0 )
      {
         // calculate diffuse and add to color
         dot_value = v3_dot_product( l, normal );

         if ( dot_value < 0 )
         {
            dot_value = 0;
         }

         final_color[ 0 ] += dot_value * light->color[ 0 ] * diffuse_color[ 0 ];
         final_color[ 1 ] += dot_value * light->color[ 1 ] * diffuse_color[ 1 ];
         final_color[ 2 ] += dot_value * light->color[ 2 ] * diffuse_color[ 2 ];

         // calculate specular and add to color
         dot_value = v3_dot_product( l_reflected, rd );

         if ( dot_value < 0 )
         {
            dot_value = 0;
         }

         dot_value = pow( dot_value, ns );

         final_color[ 0 ] += dot_value * light->color[ 0 ] * specular_color[ 0 ];
         final_color[ 1 ] += dot_value * light->color[ 1 ] * specular_color[ 1 ];
         final_color[ 2 ] += dot_value * light->color[ 2 ] * specular_color[ 2 ];

         v3_scale( final_color, radattn );
      }
      // otherwise, assume the light is a spotlight
      else
      {
         float acos_theta = cos( light->theta * DEG_TO_RAD );
         float l_opposite[ 3 ] = { 0, 0, 0 };
         float l_dot_l_opp;

         v3_copy_vector( l_opposite, l );
         v3_scale( l_opposite, -1 );

         l_dot_l_opp = v3_dot_product( l_opposite, light->direction );

         // check that light vector falls within cone
         if ( l_dot_l_opp > acos_theta )
         {
            angattn = pow( l_dot_l_opp, light->angular0 );
         }

         // calculate diffuse and add to color
         dot_value = v3_dot_product( l, normal );

         if ( dot_value < 0 )
         {
            dot_value = 0;
         }

         final_color[ 0 ] += dot_value * light->color[ 0 ] * diffuse_color[ 0 ];
         final_color[ 1 ] += dot_value * light->color[ 1 ] * diffuse_color[ 1 ];
         final_color[ 2 ] += dot_value * light->color[ 2 ] * diffuse_color[ 2 ];

         // calculate specular and add to color
         dot_value = v3_dot_product( l_reflected, rd );

         if ( dot_value < 0 )
         {
            dot_value = 0;
         }

         dot_value = pow( dot_value, ns );

         final_color[ 0 ] += dot_value * light->color[ 0 ] * specular_color[ 0 ];
         final_color[ 1 ] += dot_value * light->color[ 1 ] * specular_color[ 1 ];
         final_color[ 2 ] += dot_value * light->color[ 2 ] * specular_color[ 2 ];

         v3_scale( final_color, radattn );
         v3_scale( final_color, angattn );
      }

      // advance to next light
      wrkng_light = wrkng_light->next_node;
   }

   // check alotted bounces are not depleated
   if ( bounces > 0 )
   {
      float t_point[ 3 ] = { 0, 0, 0 };

      ObjectListNode *near_obj = NULL;

      // reflect the view vector (rd)
      v3_reflect( rd_reflect, rd, normal );

      // cast ray along reflected view vector
      // find new intersection point

      if( inside ) {
         t = cast_ray( point, rd_reflect, NULL, &near_obj, obj_list );
      } else {
         t = cast_ray( point, rd_reflect, obj, &near_obj, obj_list );
      }


      // calculate new intersection point
      t_point[ 0 ] = point[ 0 ] + rd[ 0 ] * t;
      t_point[ 1 ] = point[ 1 ] + rd[ 1 ] * t;
      t_point[ 2 ] = point[ 2 ] + rd[ 2 ] * t;

      // capture given illumination from hit object (call with bounces - 1)
      reflected_color = illuminate( obj_list, rd_reflect, t_point, near_obj,
                                    bounces - 1, inside );
      if( refractivity > 0 ) {
         //test if inside of sphere and get refractivity from that
         float ref_index = inside ? 1.0f / refractivity : refractivity;

         //get position
         float *pos = ((Sphere *)obj->data)->position;
         //copy ray for manipulation
         float ray[3];
         v3_copy_vector( ray, t_point );
         v3_normalize(ray, ray);
         //get normal ray
         float normal[3];
         v3_subtract(normal, t_point, pos );
         v3_normalize( normal, normal );
         // necessary to give correct t_point
         if( inside ) v3_scale( normal, -1.0f );
         //get dot product, from research this can be used as some sort of substitution for cosine, or at least
         //the dot product will follow the same rul as in dot of incomming ray * index of refraction 1 / ior 2 = dot
         //of outgoing ray... dot is also much faster than cosine
         float dot_val = v3_dot_product(ray, normal);
         float rd_refract[3];
         //scale up and shift over the index of refraction by dot product. and normal
         rd_refract[0] = (ray[0] * ref_index) - normal[0] * (-dot_val + (ref_index * dot_val));
         rd_refract[1] = (ray[1] * ref_index) - normal[1] * (-dot_val + (ref_index * dot_val));
         rd_refract[2] = (ray[2] * ref_index) - normal[2] * (-dot_val + (ref_index * dot_val));
         //normalize ray -> mitigates over/underflow

         float *ref_color = illuminate( obj_list, rd_refract, t_point, near_obj,
            bounces - 1, !inside );

         refracted_color[ 0 ] = ref_color[ 0 ];
         refracted_color[ 1 ] = ref_color[ 1 ];
         refracted_color[ 2 ] = ref_color[ 2 ];

         free( ref_color );
      }
   }

   v3_scale( reflected_color, reflectivity );
   v3_scale( refracted_color, refractivity );
   //v3_scale( final_color, ( 1.0f - reflectivity - refractivity ) );

   final_color[ 0 ] += ( reflected_color[ 0 ] + refracted_color[ 0 ] );
   final_color[ 1 ] += ( reflected_color[ 1 ] + refracted_color[ 1 ] );
   final_color[ 2 ] += ( reflected_color[ 2 ] + refracted_color[ 2 ] );

   color_clamp( final_color );

   // return final color ( 1 - reflected view vector ) + reflected color
   return final_color;
}

void color_clamp( float *final_color)
{

   for (int i = 0; i < 3; i++)
   {
      // clamping color values
      if(final_color[i] > 1.0)
      {
         final_color[i] = 1.0;
      }

      else if(final_color[i] < 0.0)
      {
         final_color[i] = 0.0;
      }
   }
}

// plane functions
Plane *new_plane()
{
   Plane *p = (Plane *)malloc( sizeof( Plane ) );

   p->position = v3_default_vector();
   p->normal = v3_default_vector();
   p->color = v3_default_vector();
   p->diffuse_color = v3_default_vector();
   p->specular_color = v3_default_vector();
   p->ns = DEFAULT_SHININESS;

   return p;
}

void p_set(char *obj_str, Plane *p, int mem_num)
{
   int str_len = strlen(obj_str);
   int offset = 0;
   char prop_str[str_len];
   memset(prop_str, '\0', sizeof(prop_str));

   // move past the "plane," string
   read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

   // loop while we are not at the end of the string
   for (int i = 0; i < mem_num; i+=1)
   {

      // increase offset by one to account for delim not including
      // ',' in string
      offset += 1;

      // read to attribute name
      read_to_delimiter_str(prop_str, obj_str, ':', &offset, 0);

      // otherwise check for position
      if(strstr(prop_str, "position"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 2);

         // sscanf float values into sphere-position
         sscanf(prop_str, "%*c %*c %f %*c %f %*c %f %*c", &p->position[0],
            &p->position[1], &p->position[2] );

      }
      // otherwise check for position
      else if(strstr(prop_str, "diffuse_color"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 2);

         // sscanf float values into sphere-position
         sscanf(prop_str, "%*c %*c %f %*c %f %*c %f %*c", &p->diffuse_color[0],
            &p->diffuse_color[1], &p->diffuse_color[2] );

      }
            // otherwise check for position
      else if(strstr(prop_str, "specular_color"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 2);

         // sscanf float values into sphere-position
         sscanf(prop_str, "%*c %*c %f %*c %f %*c %f %*c", &p->specular_color[0],
            &p->specular_color[1], &p->specular_color[2] );

      }
      // otherwise assume normal
      else if (strstr(prop_str, "normal"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset , 2);

         // sscanf float values into sphere-normal
         sscanf(prop_str, "%*c %*c %f %*c %f %*c %f %*c", &p->normal[0],
            &p->normal[1], &p->normal[2]);
      }

      else if( strstr(prop_str, "reflectivity"))
      {
            // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

         // sscanf float values into sphere-position
         sscanf(prop_str, "%*s %f", &p->reflectivity);
      }

   }
}

float intersect_plane( Plane *p, float *ro, float *rd )
{
   float divisor = v3_dot_product( p->normal, rd );

   // check for ray parallel to plane
   if ( divisor == 0 )
   {
      // ray misses the plane
      return HUGE_VAL;
   }


   float d = -v3_dot_product( p->normal, p->position );
   float dividend = -( v3_dot_product( p->normal, ro ) + d );

   // return calculation for plane
   return dividend / divisor;
}


// sphere functions
Sphere *new_sphere()
{
   Sphere *s = (Sphere *)malloc( sizeof( Sphere ) );

   s->radius = 1;
   s->position = v3_default_vector();
   s->color = v3_default_vector();
   s->diffuse_color = v3_default_vector();
   s->specular_color = v3_default_vector();
   s->ns = DEFAULT_SHININESS;
   s->reflectivity = 0;
   s->refractivity = 0;
   s->ior = 0;
   s->final_position = v3_default_vector();
   s->step = v3_default_vector();

   return s;
}


void s_set(char *obj_str, Sphere *s, int mem_num)
{
   int str_len = strlen(obj_str);
   int offset = 0;
   char prop_str[str_len];
   memset(prop_str, '\0', sizeof(prop_str));

   // move past the "plane," string
   read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

   // loop while we are not at the end of the string
   for (int i = 0; i < mem_num; i+=1)
   {
      // increase offset by one to account for delim not including
      // ',' in string
      offset += 1;

      // read to attribute name
      read_to_delimiter_str(prop_str, obj_str, ':', &offset, 0);

      // otherwise check for position
       if(strstr(prop_str, "position"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 2);

         // sscanf float values into sphere-position
         sscanf(prop_str, "%*s %*c %f %*c %f %*c %f %*c", &s->position[0],
            &s->position[1], &s->position[2] );
      }
            // otherwise check for position
      else if(strstr(prop_str, "diffuse_color"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 2);

         // sscanf float values into sphere-position
         sscanf(prop_str, "%*c %*c %f %*c %f %*c %f %*c", &s->diffuse_color[0],
            &s->diffuse_color[1], &s->diffuse_color[2] );

      }
            // otherwise check for position
      else if(strstr(prop_str, "specular_color"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 2);

         // sscanf float values into sphere-position
         sscanf(prop_str, "%*c %*c %f %*c %f %*c %f %*c", &s->specular_color[0],
            &s->specular_color[1], &s->specular_color[2] );

      }

      // otherwise assume radius
      else if( strstr(prop_str, "radius"))
      {
            // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

         // sscanf float values into sphere-position
         sscanf(prop_str, "%*s %f", &s->radius);
      }

      else if( strstr(prop_str, "reflectivity"))
      {
            // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

         // sscanf float values into sphere-position
         sscanf(prop_str, "%*s %f", &s->reflectivity);
      }

      else if( strstr(prop_str, "refractivity"))
      {
            // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

         // sscanf float values into sphere-position
         sscanf(prop_str, "%*s %f", &s->refractivity);
      }

      else if( strstr(prop_str, "ior"))
      {
            // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

         // sscanf float values into sphere-position
         sscanf(prop_str, "%*s %f", &s->ior);
      }

   }
}

float intersect_sphere( Sphere *s, float *ro, float *rd )
{
   float B = 2 * ( rd[ 0 ] * ( ro[ 0 ] - s->position[ 0 ] ) +
                   rd[ 1 ] * ( ro[ 1 ] - s->position[ 1 ] ) +
                   rd[ 2 ] * ( ro[ 2 ] - s->position[ 2 ] ) );
   float C = ( ( ro[ 0 ] - s->position[ 0 ] ) * ( ro[ 0 ] - s->position[ 0 ] ) ) +
             ( ( ro[ 1 ] - s->position[ 1 ] ) * ( ro[ 1 ] - s->position[ 1 ] ) ) +
             ( ( ro[ 2 ] - s->position[ 2 ] ) * ( ro[ 2 ] - s->position[ 2 ] ) ) -
             ( s->radius * s->radius );

   float t0 = ( -B - sqrt( ( B * B ) - 4 * C ) ) / 2;
   float t1 = ( -B + sqrt( ( B * B ) - 4 * C ) ) / 2;

   // check for ray missing sphere entirely
   if ( (t0 < 0 && t1 < 0) || ( isnan(t0) && isnan(t1) ) )
   {
      // ray misses sphere
      return HUGE_VAL;
   }
   // otherwise, check if t0 is closest to viewplane
   else if ( t0 > 0 && ( t0 < t1 || t1 < 0 ) )
   {
      return t0;
   }

   // otherwise, assum t1 is the closer intersection
   return t1;
}

void refract_ray(float *dst, Sphere *s, float *r0, float *rd )
{

}

void l_set(char *obj_str, Light *l, int mem_num)
{
   int str_len = strlen(obj_str);
   int offset = 0;
   char prop_str[str_len];
   memset(prop_str, '\0', sizeof(prop_str));

   // move past the "plane," string
   read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

   // loop while we are not at the end of the string
   for (int i = 0; i < mem_num; i+=1)
   {

      // increase offset by one to account for delim not including
      // ',' in string
      offset += 1;

      // read to attribute name
      read_to_delimiter_str(prop_str, obj_str, ':', &offset, 0);

      // check if first word contains color
      if (strstr(prop_str, "color"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 2);

         // sscanf float values into sphere-color
         sscanf(prop_str, "%*c %*c %f %*c %f %*c %f %*c",&l->color[0],
                                       &l->color[1], &l->color[2] );
      }

      // otherwise check for position
      else if(strstr(prop_str, "position"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 2);

         // sscanf float values into sphere-position
         sscanf(prop_str, "%*c %*c %f %*c %f %*c %f %*c", &l->position[0],
            &l->position[1], &l->position[2] );
      }

      // otherwise check for theta
      else if(strstr(prop_str, "theta"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

         // sscanf float values into sphere-position
         sscanf(prop_str,  "%*s %f", &l->theta);

      }

       // otherwise check for radial-a2
      else if(strstr(prop_str, "radial-a2"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

         // sscanf float values into sphere-position
         sscanf(prop_str,  "%*s %f", &l->radial2);

      }

      // otherwise check for theta
      else if(strstr(prop_str, "radial-a1"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

         // sscanf float values into sphere-position
         sscanf(prop_str,  "%*s %f", &l->radial1);

      }

            // otherwise check for theta
      else if(strstr(prop_str, "radial-a0"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

         // sscanf float values into sphere-position
         sscanf(prop_str,  "%*s %f", &l->radial0);

      }

      // otherwise check for angular-a0
      else if(strstr(prop_str, "angular-a0"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 0);

         // sscanf float values into light-angular0
         sscanf(prop_str,  "%*s %f", &l->angular0);

      }

      // otherwise check for direction
      else if(strstr(prop_str, "direction"))
      {
         // read float value line
         read_to_delimiter_str(prop_str, obj_str, ',', &offset, 2);

         // sscanf float values into light-direction
         sscanf(prop_str, "%*c %*c %f %*c %f %*c %f %*c", &l->direction[0],
            &l->direction[1], &l->direction[2] );
      }

   }

}


// object collection functions
void *append_object(ObjectList *obj_list, void *obj, ObjectCodes codes)
{

   // check for light code
   if (codes != LIGHT)
   {
      ObjectListNode *new_node;
      ObjectListNode *temp_node;
      /* code */

      // check if head is null
      if (obj_list->o_head == NULL)
      {
         // make node at headref
         new_node = create_node(obj, codes);

         obj_list->o_head = new_node;

         obj_list->o_tail = new_node;

      }

      else
      // otherwise the head ref is full
      {
         // create temp node for headref
         temp_node = obj_list->o_head;

         // loop until nextref is empty
         while (temp_node->next_node != NULL )
         {
            temp_node = temp_node->next_node;
         }

         // place node at temp_node
         temp_node->next_node = create_node(obj, codes);

         // make node the tail
         obj_list->o_tail = temp_node->next_node;
      }

      // increase object size
      obj_list->size++;
   }

   // otherwise assume light
   else
   {
      ObjectListNode *new_node;
      ObjectListNode *temp_node;

      // check if the head of the LL light is null
      if (obj_list->l_head == NULL)
      {
          // make node at headref
         new_node = create_node(obj, codes);

         obj_list->l_head = new_node;

         obj_list->l_tail = new_node;
      }

      // otherwise the headref has something
      else
      {
         // create temp node for headref
         temp_node = obj_list->l_head;

         // loop until nextref is empty
         while (temp_node->next_node != NULL )
         {
            temp_node = temp_node->next_node;
         }

         // place node at temp_node
         temp_node->next_node = create_node(obj, codes);

         // make node the tail
         obj_list->l_tail = temp_node->next_node;
      }
   }


      return NULL;

}


ObjectListNode *create_node(void *obj, ObjectCodes codes)
{
   ObjectListNode *new_node = (ObjectListNode *)malloc(sizeof(ObjectListNode));
   new_node-> next_node = NULL;
   new_node->data = obj;
   new_node->code = codes;
   return new_node;
}

ObjectList *init_obj_list()
{
   ObjectList *_ol = (ObjectList *)malloc( sizeof( ObjectList ) );

   _ol->size = 0;
   _ol->o_head = NULL;
   _ol->o_tail = NULL;
   _ol->l_head = NULL;
   _ol->l_tail = NULL;

   return _ol;
}

void free_objects(ObjectList *obj_list)
{
   ObjectListNode *temp_node;
   ObjectListNode *temp_nn;


   // check if head is null

   if ( obj_list->o_head != NULL )
   // check if the head ref is full
   {
      // create temp node for headref
      temp_node = obj_list->o_head;
      temp_nn = obj_list->o_head;


      // loop until nextref is empty
      while (temp_node != NULL )
      {

         // set temp_nn to temp_nn's next node
         temp_nn = temp_nn->next_node;

         // check if object is plane
         if (temp_node->code == PLANE)
         {
            // free plane properties
            Plane *destroy = (Plane *)temp_node->data;
            free(destroy->color);
            free(destroy->normal);
            free(destroy->position);
            free(destroy->diffuse_color);
            free(destroy->specular_color);
            free(destroy);
         }

         else
         // otherwise check if object is sphere
         {
            // free plane properties
            Sphere *destroy = (Sphere *)temp_node->data;
            free(destroy->color);
            free(destroy->position);
            free(destroy->diffuse_color);
            free(destroy->specular_color);
            free(destroy);
         }

         free(temp_node);

         temp_node = NULL;

         // set temp_node to temp_nn
         temp_node = temp_nn;

      }

   }

   if ( obj_list->l_head != NULL )
   // check if the head ref is full
   {
      // create temp node for headref
      temp_node = obj_list->l_head;
      temp_nn = obj_list->l_head;


      // loop until nextref is empty
      while (temp_node != NULL )
      {

         // set temp_nn to temp_nn's next node
         temp_nn = temp_nn->next_node;

         // free plane properties
         Light *destroy = (Light *)temp_node->data;
         free(destroy);


         free(temp_node);

         temp_node = NULL;

         // set temp_node to temp_nn
         temp_node = temp_nn;

      }

   }

}

int obj_mem_num(char *obj_str)
{
   // use a tracker to check if the current char is a :
   char curr_c = '\0';
   int mem_num = 0;
   int str_len = strlen(obj_str);

   // while the current char is not at the end of the loop
for (int i = 0; i < str_len; i++)
   {

      // get the next character in the string
      curr_c = obj_str[i];

      // check if it's a ':'
      if (curr_c == ':')
      {
         // increment variable
         mem_num += 1;
      }

   }

   // return counter
   return mem_num;
}

bool check_obj(char *obj_str)
{

   int str_len = strlen(obj_str);
   char curr_c = '\0';

   // while the string is not at the end
   for (int i = 0; i < str_len; i++)
   {
      curr_c = obj_str[i];

      // check if the objects contain at least char '\n' & '\0'
      if (curr_c != '\n')
      {
         return false;
      }

   }

   return true;
}
