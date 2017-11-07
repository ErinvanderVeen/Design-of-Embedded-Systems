cmake_minimum_required(VERSION 3.5)
project(my-bt-app C CXX ASM)

set(KERNEL_DIR ${CMAKE_SOURCE_DIR}/../../..)

set(CMAKE_CXX_STANDARD 11)

set(GCC_TARGET arm-none-eabi)

set(CMAKE_C_COMPILER ${GCC_TARGET}-gcc)
set(CMAKE_CXX_COMPILER ${GCC_TARGET}-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(COMMON_FLAGS "arm-none-eabi-gcc -fPIC -mno-pic-data-is-text-relative -nostdlib -fno-strict-aliasing -mcpu=arm926ej-s -mlittle-endian -D__TARGET_ARCH_ARM=5")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -std=gnu++11")

set(LDSCRIPT "$(KERNEL_DIR)/target/ev3_gcc/dmloader/app/app.ld")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-nmagic -nostdlib -pie -T ${LDSCRIPT}")

include_directories(
        ${KERNEL_DIR}/
        ${KERNEL_DIR}/include
        ${KERNEL_DIR}/arch
        ${KERNEL_DIR}/sdk/OBJ
        ${KERNEL_DIR}/sdk/common/ev3api/include
        ${KERNEL_DIR}/target/ev3_gcc/dmloader/app
        ${KERNEL_DIR}/target/ev3_gcc
        ${KERNEL_DIR}/target/ev3_gcc/drivers/common/include
        ${KERNEL_DIR}/arch/arm_gcc/am1808
        ${KERNEL_DIR}/arch/arm_gcc/common
        ${KERNEL_DIR}/target/ev3_gcc/TLSF-2.4.6/include
        ${KERNEL_DIR}/target/ev3_gcc/platform/include
        ${KERNEL_DIR}/target/ev3_gcc/pil/include
        ${KERNEL_DIR}/base-workspace/OBJ
)

file(GLOB SOURCE_FILES *.c *.cpp)
file(GLOB HEADER_FILES *.h *.hpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
