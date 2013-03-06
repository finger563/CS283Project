#ifndef TRACER_H
#define TRACER_H

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "sphere.h"
#include "triangle.h"
#include "plane.h"
#include "camera.h"
#include "color.h"

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

extern "C"
{
#include "lcd.h"
}

#define MAX_RECURSION_DEPTH	((int)2)
#define NUM_SAMPLES             (1)

#define SIZE_X		((float)SCREEN_WIDTH)
#define SIZE_Y		((float)SCREEN_HEIGHT)

class Tracer
{
public:
  float u,v,
        d,
        t0,t1;
  float r,l,t,b;
  int	recursion_depth;
  float ambient_intensity;
  Ray ray;
  Eye eye;
  GeometricObject* rtobjs;
  int num_objs;
  Record hitrec;
  Light* lights;
  int num_lights;
  Tracer(const Eye e,GeometricObject* lst,Light* l_lst)
  {
      eye=e;
      rtobjs=lst;
      hitrec=Record();
      lights=l_lst;
      ray=Ray();
      ambient_intensity=.05;
      num_objs=0;
      GeometricObject* goptr = rtobjs;
      while (goptr != NULL)
      {
          goptr = goptr->next;
          num_objs++;
      }
      num_lights=0;
      Light* lptr = lights;
      while (lptr != NULL)
      {
          lptr = lptr->next;
          num_lights++;
      }
      r=1.28;
      t=1.6;
      l=-1.28;
      b=-1.6;
      t0=1;
      t1=100000;
      d=1.0;
  }
  Tracer(Eye e)
  {
      eye=e;
      rtobjs=NULL;
      hitrec=Record();
      lights=NULL;
      ray=Ray();
      ambient_intensity=.05;
      num_objs=0;
      num_lights=0;
      r=1.28;
      t=1.6;
      l=-1.28;
      b=-1.6;
      t0=1;
      t1=100000;
      d=1.0;
  }
  Tracer()
  {
      eye=Eye();
      rtobjs=NULL;
      hitrec=Record();
      lights=NULL;
      ray=Ray();
      ambient_intensity=.05;
      num_objs=0;
      num_lights=0;
      r=1.28;
      t=1.6;
      l=-1.28;
      b=-1.6;
      t0=1;
      t1=100000;
      d=1.0;
  }
  ~Tracer(){}  

  void AddObject(GeometricObject *rhs)
  {
      if (rtobjs == NULL)
      {
              rtobjs = rhs;
              rtobjs->next = NULL;
              return;
      }
      GeometricObject* ptr = rtobjs;
      while (ptr->next!=NULL)
              ptr = ptr->next;
      ptr->next = rhs;
      rhs->next = NULL;
      num_objs++;
  }

  void AddLight(Light *rhs)
  {
      if (lights == NULL)
      {
              lights = rhs;
              lights->next = NULL;
              return;
      }
      Light* ptr = lights;
      while (ptr->next!=NULL)
              ptr = ptr->next;
      ptr->next = rhs;
      rhs->next = NULL;
      num_lights++;
  }

  void computeUV(const int x,const int y)
  {
    u = l + ((r-l) * ((float)x+.5f))/(float)SIZE_X;
    v = b + ((t-b) * ((float)y+.5f))/(float)SIZE_Y;
  }

  void computeRay(void)
  {
    ray.dir = (eye.w)*(-d) + (eye.u)*u + (eye.v)*v;
    ray.origin = (eye.e);
  }

  Color RayTrace(const Ray r,const float t0,const float t1)
  {
	Color _retclr = Color(0,0,0,0);

	if (recursion_depth>MAX_RECURSION_DEPTH)
		return _retclr;
	recursion_depth++;

        hitrec.Set(t1+100.0f);
	hitrec.Clear();

        GeometricObject* goptr = rtobjs;
	while (goptr != NULL)
        {
		goptr->Hit(&r,t0,t1,&hitrec);
                goptr = goptr->next;
	}

	if (hitrec.t>=t0 && hitrec.t<=t1)
	{
		GeometricObject* obj = (GeometricObject *)hitrec.obj_hit;

		_retclr = _retclr + obj->ka*ambient_intensity;

		float _t = hitrec.t;
		POINT3D o = (r.origin + r.dir * _t);
		VECTOR3D refl_dir = (r.dir - hitrec.normal*2*(r.dir*hitrec.normal));
		VECTOR3D normal = hitrec.normal;
		
		Light* light = lights;
                while (light != NULL)
                {
			VECTOR3D d = normalize((light->pos - o));
			Ray shadow = Ray(o,d);
			hitrec.Set(t1+100.0f);

                        goptr = rtobjs;
                        while (goptr != NULL)
                        {
				if ((goptr->Hit(&shadow,0.001f,magnitude(light->pos - o),&hitrec)) )
					break;
                                goptr = goptr->next;
			}

			if ( goptr == NULL )
			{
				VECTOR3D h = normalize( -(r.dir) + d );
				float result = normal*h;
				float _phongval = 0.0f;
				float diffval = normal * d;

				if (result>0.0f)
					_phongval = pow(result,obj->p);

				if (_phongval>0.0f)
					_retclr = _retclr + obj->ks*(light->lColor)*(light->intensity * _phongval);		//
				
				if (diffval>0.0f)
					_retclr = _retclr + (obj->kd*light->lColor)*(light->intensity*diffval);
			}
                        light = light->next;
		}
		// TODO:  Add recursive code below here for reflectance/refractance
		Ray refl = Ray(o,refl_dir);
		if ( obj->km > 0.0f )
			_retclr = _retclr*(1.0f-obj->km) + RayTrace(refl,0.001f,t1)*obj->km;
		// TODO:  Implement BRDFs
	}
	return _retclr;
  }
  
  Color RenderPixel(const int x,const int y)
  {
	Color _retclr = Color();
	float jittx,jitty;
	for (int nx = 0; nx < NUM_SAMPLES; nx++)
	{
		for (int ny = 0; ny < NUM_SAMPLES; ny++)
		{
			jittx = (float)x + ((float)nx+(float)rand()/(float)RAND_MAX - 0.5f)/((float)NUM_SAMPLES);
			jitty = (float)y + ((float)ny+(float)rand()/(float)RAND_MAX - 0.5f)/((float)NUM_SAMPLES);
			computeUV(jittx,jitty);
			computeRay();
			recursion_depth=0;
			_retclr = _retclr + RayTrace(ray,t0,t1);
		}
	}
	_retclr /= (NUM_SAMPLES*NUM_SAMPLES);
        _retclr = _retclr.powc(1/(float)2.0);
        _retclr.r = min(_retclr.r,1.0f);
        _retclr.g = min(_retclr.g,1.0f);
        _retclr.b = min(_retclr.b,1.0f);
        _retclr.a = min(_retclr.a,1.0f);
	return _retclr.powc(1/(float)2.0);
  }
};

#endif