cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(main C CXX ASM)
set(CMAKE_C_STNDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(main
	main.cpp
	func-pwm.cpp
	func-i2c.cpp
	func-MS5837-02BA.cpp
	func-BNO055.cpp
	  )

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

pico_add_extra_outputs(main)

target_link_libraries(main pico_stdlib hardware_pwm hardware_i2c)  
