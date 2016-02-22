################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CalcSubsystem.cpp \
../src/ClearHistory.cpp \
../src/DataProcess.cpp \
../src/GatherSubsystem.cpp \
../src/StoreExeSubsystem.cpp \
../src/Store_To_RunTime.cpp \
../src/WaitAllPerArchSubsystem.cpp \
../src/uarc_rmemd.cpp 


OBJS += \
./src/CalcSubsystem.o \
./src/ClearHistory.o \
./src/DataProcess.o \
./src/GatherSubsystem.o \
./src/StoreExeSubsystem.o \
./src/Store_To_RunTime.o \
./src/WaitAllPerArchSubsystem.o \
./src/uarc_rmemd.o 

CPP_DEPS += \
./src/CalcSubsystem.d \
./src/ClearHistory.d \
./src/DataProcess.d \
./src/GatherSubsystem.d \
./src/StoreExeSubsystem.d \
./src/Store_To_RunTime.d \
./src/WaitAllPerArchSubsystem.d \
./src/uarc_rmemd.d 

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo '正在构建文件： $<'
	@echo '正在调用： GNU C++ 编译器'
	g++ -O3 -I${PATH_INCLUDE} -Wall -c -fmessage-length=0 --std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '


