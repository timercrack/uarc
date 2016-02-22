################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/VIEC104/Control.cpp \
../src/VIEC104/Device.cpp \
../src/VIEC104/Frame.cpp \
../src/VIEC104/TableFile.cpp \
../src/VIEC104/VIEC104Subsystem.cpp 

OBJS += \
./src/VIEC104/Control.o \
./src/VIEC104/Device.o \
./src/VIEC104/Frame.o \
./src/VIEC104/TableFile.o \
./src/VIEC104/VIEC104Subsystem.o 

CPP_DEPS += \
./src/VIEC104/Control.d \
./src/VIEC104/Device.d \
./src/VIEC104/Frame.d \
./src/VIEC104/TableFile.d \
./src/VIEC104/VIEC104Subsystem.d 


# Each subdirectory must supply rules for building sources it contributes
src/VIEC104/%.o: ../src/VIEC104/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C++ 编译器'
	g++ -L$(PATH_LIB) -I$(PATH_INCLUDE) -O3 -fno-strict-aliasing -Wall -std=c++0x -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


