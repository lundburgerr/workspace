/*****************************************************************************
 *	\file		bufferedFileReader.h
 *
 *	\date		2013-Aug-01
 *
 *	\brief		Header for a buffered file reader
 *
 *	Copyright	Limes Audio AB
 *
 *	This code should not be modified, copied, duplicated, reproduced, licensed
 *	or sublicensed without the prior written consent of Limes Audio AB.
 *	This code (or any right in the code) should not be transferred or conveyed
 *	without the prior written consent of Limes Audio AB.
 *
 *****************************************************************************/
#ifndef BUFFERED_FILE_READER_H
#define BUFFERED_FILE_READER_H

// ---< Include >---

#include <stdio.h>

// ---< Defines >---

#define BUFFERED_FILE_READER_CHUNK_SIZE 250

// ---< Composites declaration >---

typedef struct {
	FILE* file;
	short fileOpen;
	char buffer[BUFFERED_FILE_READER_CHUNK_SIZE];
	short currentIndex;
	short currentDataSize;
} bufferedFileHandle_t;

// ---< Function prototypes >---

/*****************************************************************************
 *	\brief		Creates a handle to a file, this handle is used in all other functions in the buffered file reader
 *	\parameters	filePath - The path to the given file
 * 				handle - The handle will be stored in this parameter
 *	\return		returns 0 if all is well, returns non 0 if the file couldn't be opened.
 *****************************************************************************/
short createBufferedFileReader(char* filePath, bufferedFileHandle_t* handle);

/*****************************************************************************
 *	\brief		Closes the buffered file reader. The handle is now invalid
 *****************************************************************************/
void closeBufferedFileReader(bufferedFileHandle_t* handle);

/*****************************************************************************
 *	\brief		Tries to read a number of bytes from the buffered file reader
 *	\parameters	data - where the data should be stored
 * 				size - How many bytes that is requested
 *	\return		returns the number of bytes written to data
 *****************************************************************************/
short readData(void* data, short size, bufferedFileHandle_t* handle);

// ---< External variables >---

#endif // BUFFERED_FILE_READER_H
