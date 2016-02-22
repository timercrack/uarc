################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CollDataSubsystem.cpp \
../src/CollTaskSubsystem.cpp \
../src/RecvDataSubsystem.cpp \
../src/SendCmdSubsystem.cpp \
../src/SendScheduleSubsystem.cpp \
../src/SendZhaoceSubsystem.cpp \
../src/uarc_colld.cpp 

OBJS += \
./src/CollDataSubsystem.o \
./src/CollTaskSubsystem.o \
./src/RecvDataSubsystem.o \
./src/SendCmdSubsystem.o \
./src/SendScheduleSubsystem.o \
./src/SendZhaoceSubsystem.o \
./src/uarc_colld.o 

CPP_DEPS += \
./src/CollDataSubsystem.d \
./src/CollTaskSubsystem.d \
./src/RecvDataSubsystem.d \
./src/SendCmdSubsystem.d \
./src/SendScheduleSubsystem.d \
./src/SendZhaoceSubsystem.d \
./src/uarc_colld.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C++ 编译器'
	g++ -L$(PATH_LIB) -I$(PATH_INCLUDE) -O3 -fno-strict-aliasing -Wall -std=c++0x -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


