################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/fomular.cpp 

OBJS += \
./src/fomular.o 

CPP_DEPS += \
./src/fomular.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo '正在构建文件： $<'
	@echo '正在调用： GCC C++ Compiler'
	g++ -fPIC -O3 -I${PATH_INCLUDE} -Wall -c -fmessage-length=0 --std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '


