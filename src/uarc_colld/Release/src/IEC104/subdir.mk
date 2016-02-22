################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/IEC104/ByteDataBuffer.cpp \
../src/IEC104/Control.cpp \
../src/IEC104/Device.cpp \
../src/IEC104/Frame.cpp \
../src/IEC104/IEC104Subsystem.cpp \
../src/IEC104/TableFile.cpp 

OBJS += \
./src/IEC104/ByteDataBuffer.o \
./src/IEC104/Control.o \
./src/IEC104/Device.o \
./src/IEC104/Frame.o \
./src/IEC104/IEC104Subsystem.o \
./src/IEC104/TableFile.o 

CPP_DEPS += \
./src/IEC104/ByteDataBuffer.d \
./src/IEC104/Control.d \
./src/IEC104/Device.d \
./src/IEC104/Frame.d \
./src/IEC104/IEC104Subsystem.d \
./src/IEC104/TableFile.d 


# Each subdirectory must supply rules for building sources it contributes
src/IEC104/%.o: ../src/IEC104/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C++ 编译器'
	g++ -L$(PATH_LIB) -I$(PATH_INCLUDE) -O3 -fno-strict-aliasing -Wall -std=c++0x -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


