################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ADC_program.c \
../APP_program.c \
../DIO_program.c \
../EXTI_program.c \
../GIE_program.c \
../KPD_program.c \
../LCD_program.c \
../LDR_program.c \
../MATH_HELPER.c \
../PORT_program.c \
../SERVO_program.c \
../TIMER_program.c \
../ULTRASONIC_program.c \
../ULTSONIC_program.c \
../USART_program.c \
../main.c 

OBJS += \
./ADC_program.o \
./APP_program.o \
./DIO_program.o \
./EXTI_program.o \
./GIE_program.o \
./KPD_program.o \
./LCD_program.o \
./LDR_program.o \
./MATH_HELPER.o \
./PORT_program.o \
./SERVO_program.o \
./TIMER_program.o \
./ULTRASONIC_program.o \
./ULTSONIC_program.o \
./USART_program.o \
./main.o 

C_DEPS += \
./ADC_program.d \
./APP_program.d \
./DIO_program.d \
./EXTI_program.d \
./GIE_program.d \
./KPD_program.d \
./LCD_program.d \
./LDR_program.d \
./MATH_HELPER.d \
./PORT_program.d \
./SERVO_program.d \
./TIMER_program.d \
./ULTRASONIC_program.d \
./ULTSONIC_program.d \
./USART_program.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


