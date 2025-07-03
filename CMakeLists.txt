# Minimum CMake version
cmake_minimum_required(VERSION 3.22.0)

# Add extra application modules
#set(EXTRA_ZEPHYR_MODULES "${CMAKE_SOURCE_DIR}/modules/say_hello")
set(EXTRA_ZEPHYR_MODULES "${CMAKE_SOURCE_DIR}/modules/say_hello;/workspace/modules/si5351")
set(EXTRA_DTC_OVERLAY_FILE "app.overlay")

# Locate the Zephyr RTOS source
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Name the project
project(blink)

# Locate the source code for the application
target_sources(app PRIVATE src/main.c)
