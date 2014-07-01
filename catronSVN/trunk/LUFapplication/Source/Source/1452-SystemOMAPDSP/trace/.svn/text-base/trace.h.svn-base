/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : trace.h
* Author            : Roger Vilmunen ÅF
* Description       : Trace interface
*                   :
********************************************************************************
* Revision History
* 2013-06-03        : First Issue
*******************************************************************************/

#include <xdc/runtime/System.h>

/* For internal use */
extern char fstr[200];

/******************************************************************************
* Function Name    : TRACE (macros)
* Function Purpose : Macro for the trace function
*                  : Add filename and line to the trace and can handle formated strings
* Input parameters : String
* Return value     : 0 = ok
******************************************************************************/
#define TRACE(str)         System_sprintf(fstr, "D:%s:%d:" str, __FUNCTION__, __LINE__);trace(fstr);
#define TRACE1(str, a1)    System_sprintf(fstr, "D_%s:%d:" str, __FUNCTION__, __LINE__, a1);trace(fstr);
#define TRACE2(str, a1, a2)    System_sprintf(fstr, "D:%s:%d:" str, __FUNCTION__, __LINE__, a1, a2);trace(fstr);
#define TRACE3(str, a1, a2, a3)    System_sprintf(fstr, "D:%s:%d:" str, __FUNCTION__, __LINE__, a1, a2, a3);trace(fstr);
#define TRACE4(str, a1, a2, a3, a4)    System_sprintf(fstr, "D:%s:%d:" str, __FUNCTION__, __LINE__, a1, a2, a3, a4);trace(fstr);
#define TRACE5(str, a1, a2, a3, a4 , a5)    System_sprintf(fstr, "D:%s:%d:" str, __FUNCTION__, __LINE__, a1, a2, a3, a4 , a5);trace(fstr);

/******************************************************************************
* Function Name    : trace_init
* Function Purpose : Initialize the trace function
*                  : This must be called before any other trace functions/macros
* Input parameters : none
* Return value     : 0 = ok
******************************************************************************/
int trace_init(void);

/******************************************************************************
* Function Name    : trace
* Function Purpose : Add a string into the trace buffer.
*                  : The buffer is processing in the background
*                  : Init muse be called once before
* Input parameters : String
* Return value     : 0 = ok
******************************************************************************/
void trace(char *str);
