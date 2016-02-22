################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/NX/Control.cpp \
../src/NX/Device.cpp \
../src/NX/FSMStates.cpp \
../src/NX/Frame.cpp \
../src/NX/FrameNx.cpp \
../src/NX/NxSubsystem.cpp \
../src/NX/ParseFrame.cpp \
../src/NX/TableFile.cpp 

OBJS += \
./src/NX/Control.o \
./src/NX/Device.o \
./src/NX/FSMStates.o \
./src/NX/Frame.o \
./src/NX/FrameNx.o \
./src/NX/NxSubsystem.o \
./src/NX/ParseFrame.o \
./src/NX/TableFile.o 

CPP_DEPS += \
./src/NX/Control.d \
./src/NX/Device.d \
./src/NX/FSMStates.d \
./src/NX/Frame.d \
./src/NX/FrameNx.d \
./src/NX/NxSubsystem.d \
./src/NX/ParseFrame.d \
./src/NX/TableFile.d 


# Each subdirectory must supply rules for building sources it contributes
src/NX/%.o: ../src/NX/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C++ 编译器'
	g++ -L$(PATH_LIB) -I$(PATH_INCLUDE) -O3 -fno-strict-aliasing -Wall -std=c++0x -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


