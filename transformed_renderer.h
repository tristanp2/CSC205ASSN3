/* transformed_renderer.h

   A wrapper around the SDL rendering functions which applies
   a transformation before rendering.
   
   B. Bird - 05/26/2010 (Updated 02/08/2016)
*/

#ifndef TRANSFORMED_RENDERER_H
#define TRANSFORMED_RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cmath>
#include <cstring>
#include "matrix.h"

#ifdef __GNUC__
#include <alloca.h>
#define ALLOCATE_SINT16_ARRAY(n) ((Sint16*)alloca(n*sizeof(Sint16)))
#define DEALLOCATE_SINT16_ARRAY(A) 
#else
#define ALLOCATE_SINT16_ARRAY(n) new Sint16[n]
#define DEALLOCATE_SINT16_ARRAY(A) delete[] A
#endif

class TransformedRenderer{
public:
	static const int CIRCLE_POINTS = 16;
	TransformedRenderer(SDL_Renderer* renderer){
		this->renderer = renderer;
	}
	
	void set_transform(Matrix3& newTransform){
		this->transform = newTransform;
	}
    Matrix3 get_transform(){
        return transform;
    }
	
	void drawLine(float x1, float y1, float x2, float y2, Uint8 width, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
		Sint16 ix1,iy1,ix2,iy2;
		TransformVector(x1,y1,ix1,iy1);
		TransformVector(x2,y2,ix2,iy2);

		thickLineRGBA(renderer, ix1, iy1, ix2, iy2, width,r,g,b,a);
	}
	
	void drawCircle(float x, float y, float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
		Matrix2 M( cos(2*M_PI/CIRCLE_POINTS), sin(2*M_PI/CIRCLE_POINTS), -sin(2*M_PI/CIRCLE_POINTS), cos(2*M_PI/CIRCLE_POINTS) );
		Vector2 V(radius, 0);

		float vx[CIRCLE_POINTS], vy[CIRCLE_POINTS];
		for (int i = 0; i < CIRCLE_POINTS; i++){
			vx[i] = x + V.x;
			vy[i] = y + V.y;
			V = M*V;
		}
		drawPolygon(vx,vy,CIRCLE_POINTS, r, g, b, a);
	}
	
	void fillCircle(float x, float y, float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
		Matrix2 M( cos(2*M_PI/CIRCLE_POINTS), sin(2*M_PI/CIRCLE_POINTS), -sin(2*M_PI/CIRCLE_POINTS), cos(2*M_PI/CIRCLE_POINTS) );
		Vector2 V(radius, 0);

		float vx[CIRCLE_POINTS], vy[CIRCLE_POINTS];
		for (int i = 0; i < CIRCLE_POINTS; i++){
			vx[i] = x + V.x;
			vy[i] = y + V.y;
			V = M*V;
		}
		fillPolygon(vx,vy,CIRCLE_POINTS, r, g, b, a);
	}
	
	void drawRectangle(float x1, float y1, float x2, float y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
		float vx[] = {x1,x2,x2,x1};
		float vy[] = {y1,y1,y2,y2};
		drawPolygon(vx,vy,4, r,g,b,a );
	}
	
	void fillRectangle(float x1, float y1, float x2, float y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
		float vx[] = {x1,x2,x2,x1};
		float vy[] = {y1,y1,y2,y2};
		fillPolygon(vx,vy,4, r,g,b,a );
	}
	
	void drawPolygon(const float *vx, const float *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
		
		Sint16* new_vx = ALLOCATE_SINT16_ARRAY(n);
		Sint16* new_vy = ALLOCATE_SINT16_ARRAY(n);
		for(int i = 0; i < n; i++)
			TransformVector(vx[i], vy[i], new_vx[i],new_vy[i]);
		
		polygonRGBA(renderer, new_vx, new_vy,n,r,g,b,a);
		
		DEALLOCATE_SINT16_ARRAY(new_vx);
		DEALLOCATE_SINT16_ARRAY(new_vy);
	}
	
	void fillPolygon(const float *vx, const float *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
		
		Sint16* new_vx = ALLOCATE_SINT16_ARRAY(n);
		Sint16* new_vy = ALLOCATE_SINT16_ARRAY(n);
		
		for(int i = 0; i < n; i++)
			TransformVector(vx[i],vy[i],new_vx[i],new_vy[i]);
		
		filledPolygonRGBA(renderer, new_vx, new_vy,n,r,g,b,a);
		
		DEALLOCATE_SINT16_ARRAY(new_vx);
		DEALLOCATE_SINT16_ARRAY(new_vy);
	}

private:
	void TransformVector(float in_x, float in_y, Sint16& out_x, Sint16& out_y){
		Vector3 V = transform*Vector3(in_x,in_y,1);
		out_x = (Sint16)roundf(V.x);
		out_y = (Sint16)roundf(V.y);
	}
	
	SDL_Renderer* renderer;
	Matrix3 transform;
};


#endif
