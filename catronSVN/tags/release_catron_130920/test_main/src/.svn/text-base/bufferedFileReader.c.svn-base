/*****************************************************************************
 *	\file		bufferedFileReader.c
 *
 *	\date		2013-Aug-01
 *
 *	\brief		A buffered file reader
 *
 *	Copyright	Limes Audio AB
 *
 *	This code should not be modified, copied, duplicated, reproduced, licensed
 *	or sublicensed without the prior written consent of Limes Audio AB.
 *	This code (or any right in the code) should not be transferred or conveyed
 *	without the prior written consent of Limes Audio AB.
 *
 *****************************************************************************/
// ---< Include >---

#include "bufferedFileReader.h"

// ---< Defines >---

// ---< Global variabels >---

// ---< Local function prototypes >---

// ---< Functions >---

/*****************************************************************************
 *	\brief		Creates a handle to a file, this handle is used in all other functions in the buffered file reader
 *	\parameters	filePath - The path to the given file
 * 				handle - The handle will be stored in this parameter
 *	\return		returns non 0 if all is well, returns 0 if the file couldn't be opened.
 *****************************************************************************/
short createBufferedFileReader(char* filePath, bufferedFileHandle_t* handle) {
	handle->file = fopen(filePath, "rb");
	handle->fileOpen = handle->file != 0;
	handle->currentIndex = 0;
	handle->currentDataSize = 0;

	return handle->fileOpen;
}

/*****************************************************************************
 *	\brief		Closes the buffered file reader. The handle is now invalid
 *****************************************************************************/
void closeBufferedFileReader(bufferedFileHandle_t* handle) {
	fclose(handle->file);
	handle->fileOpen = 0;
}

/*****************************************************************************
 *	\brief		Tries to read a number of bytes from the buffered file reader
 *	\parameters	data - where the data should be stored
 * 				size - How many bytes that is requested
 *	\return		returns the number of bytes written to data
 *****************************************************************************/
short readData(void* data, short size, bufferedFileHandle_t* handle) {
	if (handle->fileOpen) {
		if (size <= handle->currentDataSize - handle->currentIndex ) {
			memcpy(data, handle->buffer+handle->currentIndex, size);
			handle->currentIndex += size;
			return size;
		}
		else {
			short memLeft = handle->currentDataSize - handle->currentIndex;
			memcpy(data, handle->buffer+handle->currentIndex, memLeft);
			handle->currentDataSize = fread(handle->buffer, sizeof(char), BUFFERED_FILE_READER_CHUNK_SIZE, handle->file);
			handle->currentIndex = 0;
			if (handle->currentDataSize == 0) {
				return memLeft;
			}
			else {
				return memLeft + readData(((char*)data)+memLeft, size-memLeft, handle);
			}
		}
	}
	else {
		return 0;
	}
}
