################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/main_audio.obj: ../src/main_audio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6740 -g --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/include" --include_path="C:/Documents and Settings/justin.oliver/My Documents/temp/L138 related files/OMAPL138_BSL_WS/OMAPL138_BSL_WS/DSP eXp Audio Test/inc" --include_path="C:/Documents and Settings/justin.oliver/My Documents/temp/L138 related files/OMAPL138_BSL_WS/OMAPL138_BSL_WS/DSP BSL/inc" --diag_warning=225 --preproc_with_compile --preproc_dependency="src/main_audio.pp" --obj_directory="src" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

src/test_audio.obj: ../src/test_audio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6740 -g --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/include" --include_path="C:/Documents and Settings/justin.oliver/My Documents/temp/L138 related files/OMAPL138_BSL_WS/OMAPL138_BSL_WS/DSP eXp Audio Test/inc" --include_path="C:/Documents and Settings/justin.oliver/My Documents/temp/L138 related files/OMAPL138_BSL_WS/OMAPL138_BSL_WS/DSP BSL/inc" --diag_warning=225 --preproc_with_compile --preproc_dependency="src/test_audio.pp" --obj_directory="src" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


