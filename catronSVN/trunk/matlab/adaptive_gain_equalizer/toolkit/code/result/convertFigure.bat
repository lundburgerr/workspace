@ECHO off
REM File:		convertFigure.bat
REM Purpose:	Runs epstopdf then pdftops for input file
REM	Input:		%1	- Name of input file (without file extension)
REM				%2	- Name of output file (with file extension)
REM	Example:	convertFigure result4 fig1.eps
REM Author:		Markus Borgh
REM Date:		2011-Jan-12
REM Copyright:	Limes Audio AB
REM E-mail:		markus.borgh@limesaudio.com

REM Check input
REM if %1.==. goto endOfFile

REM	Convert EPS to PDF
epstopdf %1.eps --gsopt=-dPDFSETTINGS=/prepress
REM Convert output PDF to PS file
pdftops -q -paper match -pagecrop -eps -level2 %1.pdf %2
REM Delete intermediate file
del %1.pdf

REM If bad input, jump to this
:endOfFile
@ECHO on