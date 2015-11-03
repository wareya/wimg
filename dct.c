/* DCT and IDCT - listing 1
 * Copyright (c) 2001 Emil Mikulic.
 * http://unix4lyfe.org/dct/
 *
 * Feel free to do whatever you like with this code.
 * Feel free to credit me.
 */
/* EDIT: Modified to remove various things that aren't necessary for me!
 * View the original site for the original file.
 * Original: listing1.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>


#ifndef PI
 #ifdef M_PI
  #define PI M_PI
 #else
  #define PI 3.14159265358979
 #endif
#endif



/* S[u,v] = 1/4 * C[u] * C[v] *
 *   sum for x=0 to width-1 of
 *   sum for y=0 to height-1 of
 *     s[x,y] * cos( (2x+1)*u*PI / 2N ) * cos( (2y+1)*v*PI / 2N )
 *
 * C[u], C[v] = 1/sqrt(2) for u, v = 0
 * otherwise, C[u], C[v] = 1
 *
 * S[u,v] ranges from -2^10 to 2^10
 */

#define COEFFS(Cu,Cv,u,v) { \
	if (u == 0) Cu = 1.0 / sqrt(2.0); else Cu = 1.0; \
	if (v == 0) Cv = 1.0 / sqrt(2.0); else Cv = 1.0; \
	}

void dct(int16_t indata[8][8], double outdata[8][8])
{
	int u,v,x,y;
	
	for (v=0; v<8; v++)
	for (u=0; u<8; u++)
	{
		double Cu, Cv, z = 0.0;

		COEFFS(Cu,Cv,u,v);

		for (y=0; y<8; y++)
		for (x=0; x<8; x++)
		{
			double s, q;

			s = indata[x][y];

			q = s * cos((double)(2*x+1) * (double)u * PI/16.0) *
				cos((double)(2*y+1) * (double)v * PI/16.0);

			z += q;
		}

		outdata[v][u] = 0.25 * Cu * Cv * z;
	}
}



/* play with this bit */
void quantize(double dct_buf[8][8])
{
	int x,y;

	for (y=0; y<8; y++)
	for (x=0; x<8; x++)
	{
		int sqdist = x*x + y*y; // 0~128
		sqdist -= 5; // -5~123
		sqdist *= (sqdist > 0); //0~123
		sqdist *= 8; // 0~984
		sqdist += 1; // 1~985
		dct_buf[y][x] = round(dct_buf[y][x]/sqdist)*sqdist;
	}
}



void idct(double indata[8][8], int16_t outdata[8][8])
{
	int u,v,x,y;
    
	for (y=0; y<8; y++)
	for (x=0; x<8; x++)
	{
		double z = 0.0;

		for (v=0; v<8; v++)
		for (u=0; u<8; u++)
		{
			double S, q;
			double Cu, Cv;
			
			COEFFS(Cu,Cv,u,v);
			S = indata[v][u];

			q = Cu * Cv * S *
				cos((double)(2*x+1) * (double)u * PI/16.0) *
				cos((double)(2*y+1) * (double)v * PI/16.0);

			z += q;
		}
		
        z /= 4.0;
		if (z > 1020.0) z = 1020.0;
		if (z < 0) z = 0.0;

		outdata[x][y] = (uint16_t) round(z);
	}
}
