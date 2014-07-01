################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main_audio.c \
../src/test_audio.c 

OBJS += \
./src/main_audio.obj \
./src/test_audio.obj 

C_DEPS += \
./src/main_audio.pp \
./src/test_audio.pp 

OBJS__QTD += \
".\src\main_audio.obj" \
".\src\test_audio.obj" 

C_DEPS__QTD += \
".\src\main_audio.pp" \
".\src\test_audio.pp" 

C_SRCS_QUOTED += \
"../src/main_audio.c" \
"../src/test_audio.c" 


# Each subdirectory must supply rules for building sources it contributes
src/main_audio.obj: ../src/main_audio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6740 -g --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/include" --include_path="../../../../bsl/inc/" --include_path="../inc/" --diag_warning=225 --preproc_with_compile --preproc_dependency="src/main_audio.pp" --obj_directory="src" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

src/test_audio.obj: ../src/test_audio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6740 -g --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/include" --include_path="../../../../bsl/inc/" --include_path="../inc/" --diag_warning=225 --preproc_with_compile --preproc_dependency="src/test_audio.pp" --obj_directory="src" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


