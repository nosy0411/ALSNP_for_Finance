/* vim: set ts=4 sw=4: */
/* Filename    : omp_pi_num_integration.c
 * Description : calculate pi
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : numerical integration method
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
int num_steps=800000000; /* integration 횟수: 8억번 (너무 많으면 줄이자.) */

int main()
{
	int i;
	double x, step, sum = 0.0;
	step = 1.0/(double) num_steps;
    omp_set_num_threads(2);
#pragma omp parallel
#pragma omp for private(x) reduction(+:sum) schedule(static) nowait
	for (i=0; i<num_steps; i++) {
		x = (i+0.5) * step;
		sum += 4.0/(1.0 + x*x);
	}
	printf("pi = %.8f (sum = %.8f), 4*atan(1) = %.8f\n", step*sum, sum, atan(1)*4);
	return EXIT_SUCCESS;
}
