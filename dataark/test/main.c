/*
 * main.c
 *
 *  Created on: Nov 28, 2013
 *      Author: lundburgerr
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

	char *num = "0x10";
	int numnum = strtol(num, NULL, 0);
	numnum = strtol("0b101011", NULL, 0);

	return 0;
}
