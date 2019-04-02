#pragma once

#include <cmath>
void gen_tornado(int xs, int ys, int zs, int time, float *tornado)
/*
 *  Gen_Tornado creates a vector field of dimension [xs,ys,zs,3] from
 *  a proceedural function. By passing in different time arguements,
 *  a slightly different and rotating field is created.
 *
 *  The magnitude of the vector field is highest at some funnel shape
 *  and values range from 0.0 to around 0.4 (I think).
 *
 *  I just wrote these comments, 8 years after I wrote the function.
 *
 * Developed by Roger A. Crawfis, The Ohio State University
 *
 */
{
	float x, y, z;
	int ix, iy, iz;
	float r, xc, yc, scale, temp, z0;
	float r2 = 8;
	float SMALL = 0.00000000001f;
	float xdelta = 1.0f / (xs - 1.0f);
	float ydelta = 1.0f / (ys - 1.0f);
	float zdelta = 1.0f / (zs - 1.0f);

	for (iz = 0; iz < zs; iz++)
	{
		z = iz * zdelta;                        // map z to 0->1
		xc = 0.5f + 0.1f*sin(0.04f*time + 10.0f*z);   // For each z-slice, determine the spiral circle.
		yc = 0.5f + 0.1f*cos(0.03f*time + 3.0f*z);    //    (xc,yc) determine the center of the circle.
		r = 0.1f + 0.4f * z*z + 0.1f * z * sin(8.0f*z); //  The radius also changes at each z-slice.
		r2 = 0.2f + 0.1f*z;                           //    r is the center radius, r2 is for damping
		for (iy = 0; iy < ys; iy++)
		{
			y = iy * ydelta;
			for (ix = 0; ix < xs; ix++)
			{
				x = ix * xdelta;
				temp = sqrt((y - yc)*(y - yc) + (x - xc)*(x - xc));
				scale = fabs(r - temp);
				/*
				 *  I do not like this next line. It produces a discontinuity
				 *  in the magnitude. Fix it later.
				 *
				 */
				if (scale > r2)
					scale = 0.8f - scale;
				else
					scale = 1.0f;
				z0 = 0.1f * (0.1f - temp * z);
				if (z0 < 0.0f)  z0 = 0.0f;
				temp = sqrt(temp*temp + z0 * z0);
				scale = (r + r2 - temp) * scale / (temp + SMALL);
				scale = scale / (1 + z);
				*tornado++ = scale * (y - yc) + 0.1f*(x - xc);
				*tornado++ = scale * -(x - xc) + 0.1f*(y - yc);
				*tornado++ = scale * z0;
			}
		}
	}
}

#pragma once
