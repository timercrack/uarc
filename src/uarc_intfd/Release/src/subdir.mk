################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/UARCServiceImpl.cpp \
../src/uarc_intfd.cpp 

CC_SRCS += \
../src/uarc.pb.cc 

OBJS += \
./src/UARCServiceImpl.o \
./src/uarc.pb.o \
./src/uarc_intfd.o 

CC_DEPS += \
./src/uarc.pb.d 

CPP_DEPS += \
./src/UARCServiceImpl.d \
./src/uarc_intfd.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C++ 编译器'
	g++ -I${PATH_INCLUDE} -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C++ 编译器'
	g++ -I${PATH_INCLUDE} -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


