/*
 *  © 2014 Louis-Guillaume Gagnon <louis.guillaume.gagnon@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <complex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

int NITER = 500;
size_t XDIM = 3000;
size_t YDIM = 2000;
double REALMIN = -2;
double REALMAX = 1;
#define REALSTEP ((REALMAX - REALMIN) / XDIM)
double IMAGMIN = -1;
double IMAGMAX = 1;
#define IMAGSTEP ((IMAGMAX - IMAGMIN) / YDIM)

#define OUTFILE stdout

struct Color color0(int niter)
{
	struct Color col;
	
	if (niter < 0) {
		col.r = col.g = col.b = 0;
	} else {
		col.r = niter * 255.0 / NITER;
		col.g = (col.r * 2) % 255;
		col.b = (col.r * 10) % 255;
	}

	col.a = 255;

	return col;
}

struct Color (*color)(int niter) = color0;

/* if return value < 0, c is in the set
 * otherwise, number of iterations before diverging is returned
 * (i.e. returns n such as z_n > 2 */
int mandelbrot_bounded(double complex c)
{
	int i;
	double complex z = 0;

	for (i = 0; i < NITER && cabs(z) < 2; i++)
		z = z * z + c;

	return (cabs(z) < 2)? -1 : i;
}

void print_header()
{
	uint32_t tmp; 

	fprintf(OUTFILE, "imagefile");
	tmp = htonl(XDIM);
	fwrite(&tmp, sizeof(uint32_t), 1, OUTFILE);
	tmp = htonl(YDIM);
	fwrite(&tmp, sizeof(uint32_t), 1, OUTFILE);
}

void print_mandelbrot()
{
	double real, imag;
	size_t k, x, y, row_len;
	uint8_t *row;

	row_len = XDIM * 4;
	row = malloc(row_len);

	for (y = 0, imag = IMAGMIN; y < YDIM; y++, imag += IMAGSTEP) {
		if (y % 100 ==  0) 
			fprintf(stderr, "column %d / %d\n", y, YDIM);
		for (x = 0, k = 0, real = REALMIN; x < XDIM; x++, k += 4, real += REALSTEP) {
			double complex c = real + imag * I;				
			struct Color col = color(mandelbrot_bounded(c));
			row[k] = col.r;
			row[k+1] = col.g;
			row[k+2] = col.b;
			row[k+3] = col.a;
		}
		fwrite(row, 1, row_len, OUTFILE);
	}
	free(row);
}

int main(int argc, char *argv[])
{
	print_header();
	print_mandelbrot();

	return 0;	
}
