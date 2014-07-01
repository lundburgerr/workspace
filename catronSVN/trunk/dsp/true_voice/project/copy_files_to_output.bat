@ECHO OFF
REM File:		copy_files_to_output.bat
REM Purpose:	Copy files to output folder
REM	Input:		%1	- Library directory (use "" if name contains spaces)
REM Author:		Markus Borgh
REM Date:		2012-Sep-03
REM Copyright:	Limes Audio AB
REM E-mail:		markus.borgh@limesaudio.com

REM Replace forward slash '/' with '\' to get file paths working
REM '~' removes surrounding double quotes '"'
SET x=%~1
SET x=%x:/=\%

REM Change directory to batch file path
CD %~dp0

REM Prompt info
ECHO.
ECHO --- Copy following files
ECHO     true_voice.h
ECHO     TrueVoice.lib

REM 1. Copy header file
copy "..\include\true_voice.h" "..\output\"
copy "..\include\true_voice.h" "..\..\test_main\include"
REM 2. Copy library file
copy "%x%\TrueVoice.lib" "..\output\"
copy "%x%\TrueVoice.lib" "..\..\test_main\lib"
