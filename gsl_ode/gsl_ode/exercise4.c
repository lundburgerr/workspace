/*
 * main.c
 *
 *  Created on: Oct 21, 2013
 *      Author: rolund03
 */

#include <stdlib.h>
#include <stdio.h>

#include <gsl_errno.h>
#include <gsl_vector.h>


int save_vector(gsl_vector *v, const char *file);

int main(int argc, char** argv){
	unsigned int N = 200;
	int n;

	gsl_vector *hare 	= gsl_vector_alloc(N);
	gsl_vector *lynx 	= gsl_vector_alloc(N);
	gsl_vector *t 		= gsl_vector_alloc(N);

	//Set up system parameters
	double h = 0.1;
	double alpha = 2;
	double x = 1;
	double y = 0.1;

	//Solve system
	for(n=0; n<N; n++){
		x = x + h*(1-y)*x;
		y = y + h*alpha*(x-1)*y;

		//Store result
		gsl_vector_set(hare, n, x);
		gsl_vector_set(lynx, n, y);
		gsl_vector_set(t, n, n);
	}

	//Write to file
	save_vector(hare, "hare.txt");
	save_vector(lynx, "lynx.txt");
	save_vector(t, "time.txt");

	//Free memory
	gsl_vector_free(hare);
	gsl_vector_free(lynx);
	gsl_vector_free(t);

	return EXIT_SUCCESS;
}


int save_vector(gsl_vector *v, const char *file){
	FILE *f = fopen(file, "w+");
	if(f){
		gsl_vector_fprintf(f, v, "%g");
	}
	else{
		perror("fopen");
		return EXIT_FAILURE;
	}

	fclose(f);
	return EXIT_SUCCESS;
}
