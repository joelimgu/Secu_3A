# Copyright (C) 2021, INSA-TOULOUSE
# This file is part of the TP-QSE-INSA project, under the Apache License v2.0
# See https://www.apache.org/licenses/LICENSE-2.0 for license information.
# SPDX-License-Identifier: Apache-2.0

TARGET = stm32f3
FIRMWARE=hardened-digicode
NAME=hardened-digicode

RANDCODE=$(shell python -c 'import numpy as np; print(str(np.random.randint(0,9999)).zfill(4));')

# =====================================================================================
# Host compiler and flags.
# -------------------------------------------------------------------------------------
HOST_CC=gcc
HOST_CXX=g++
HOST_CFLAGS=-O2 -Wall
HOST_CXXFLAGS=-O2 -Wall -std=c++11

# =====================================================================================
# Cross compiler and flags.
# -------------------------------------------------------------------------------------
TRIPLE	?= /opt/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi
XCC      := ${TRIPLE}-gcc
XAS      := ${TRIPLE}-gcc
XLD      := ${TRIPLE}-gcc
XOBJDUMP := ${TRIPLE}-objdump
XOBJCOPY := ${TRIPLE}-objcopy

CPU:=cortex-m4
PLATFORM:=stm32f3
PLATFORM_CPPFLAGS:=-DSTM32F303xC -DSTM32F3 -DSTM32 -DHAL_TYPE=HAL_stm32f3 -DPLATFORM=CW308_STM32F3 -DF_CPU=7372800UL
PLATFORM_ASFLAGS:=-DF_CPU=7372800
PLATFORM_SRCS:=${PLATFORM}_hal.c ${PLATFORM}_hal_lowlevel.c ${PLATFORM}_sysmem.c
PLATFORM_INCLUDES=$(addprefix -Itarget_prog/${PLATFORM}/, . CMSIS CMSIS/core CMSIS/device Legacy)
PLATFORM_LDSCRIPT=target_prog/${PLATFORM}/LinkerScript.ld

XOPTFLAGS:=-Os -g
XCPUFLAGS:=-mcpu=${CPU} -mthumb -mfloat-abi=soft
XCPPFLAGS:=-I. -DDEBUG ${PLATFORM_CPPFLAGS} -DSS_VER_2_0=2 -DSS_VER_1_1=1 -DSS_VER_1_0=0
XASFLAGS:=-I. -x assembler-with-cpp -fmessage-length=0 -ffunction-sections ${PLATFORM_ASFLAGS}
XCFLAGS:=-Wall -fmessage-length=0 -ffunction-sections -gdwarf-2 -funsigned-char -funsigned-bitfields -fshort-enums
XLDFLAGS:=--specs=nano.specs --specs=nosys.specs -T ${PLATFORM_LDSCRIPT}

# Store cross-compiled objects in a per TARGET direcory to prevent mixing different kind of objects.
XOBJDIR:=build/${TARGET}/${FIRMWARE}
XEXEDIR:=build/${TARGET}/${FIRMWARE}

# Source files:

FIRMWARE_SRCS     = $(patsubst firmware/${FIRMWARE}/%, % , $(wildcard firmware/${FIRMWARE}/*.c))
FIRMWARE_ASM_SRCS = $(patsubst firmware/${FIRMWARE}/%, % , $(wildcard firmware/${FIRMWARE}/*.s))
FIRMWARE_OBJECTS := $(addprefix ${XOBJDIR}/, $(patsubst %.c, %.o, $(FIRMWARE_SRCS)))
FIRMWARE_OBJECTS += $(addprefix ${XOBJDIR}/, $(patsubst %.s, %.o, $(FIRMWARE_ASM_SRCS)))
FIRMWARE_ELF     := $(addprefix bin/,$(addsuffix .elf, ${NAME}))
FIRMWARE_HEX     := $(addprefix bin/,$(addsuffix .hex, ${NAME}))

# Misc common files:
COMMON_HEADERS :=$(wildcard target_prog/*.h)
COMMON_INCLUDES:=-Itarget_prog/
COMMON_SRCS=DWT.c MT_RNG.c io.c
COMMON_OBJECTS:=$(addprefix ${XOBJDIR}/, $(patsubst %.c, %.o, $(COMMON_SRCS)))

PLATFORM_OBJECTS:=$(addprefix ${XOBJDIR}/, $(patsubst %.c, %.o, ${PLATFORM_SRCS}))

.PRECIOUS: ${XOBJDIR}/%.o ${XEXEDIR}/%.S ${XOBJDIR}/${PLATFORM}_startup.o

all: builddir ${FIRMWARE_ELF} ${FIRMWARE_HEX}

builddir:
	@mkdir -p ${XOBJDIR} ${XEXEDIR}

${XOBJDIR}/%.o: target_prog/%.c ${COMMON_HEADERS}
	${XCC} ${PLATFORM_INCLUDES} ${COMMON_INCLUDES} ${XCFLAGS} ${XCPPFLAGS} ${XOPTFLAGS} ${XCPUFLAGS} -c $< -o $@

${XOBJDIR}/%.o: target_prog/${PLATFORM}/%.c
	${XCC} ${PLATFORM_INCLUDES} ${XCFLAGS} ${XCPPFLAGS} ${XOPTFLAGS} ${XCPUFLAGS} -c $< -o $@

${XOBJDIR}/%.o: firmware/${FIRMWARE}/%.c ${COMMON_HEADERS}
ifndef CODE
	${XCC} ${PLATFORM_INCLUDES} ${COMMON_INCLUDES} ${XCFLAGS} ${XCPPFLAGS} ${XOPTFLAGS} ${XCPUFLAGS} -c $< -o $@ -DCODE=$(RANDCODE)
else
	${XCC} ${PLATFORM_INCLUDES} ${COMMON_INCLUDES} ${XCFLAGS} ${XCPPFLAGS} ${XOPTFLAGS} ${XCPUFLAGS} -c $< -o $@ -DCODE=$(CODE)
endif
	
	
${XOBJDIR}/%.o: firmware/${FIRMWARE}/%.s
	${XAS} ${XASFLAGS} ${XCPUFLAGS} -c $< -o $@

${XOBJDIR}/${PLATFORM}_startup.o: target_prog/${PLATFORM}/${PLATFORM}_startup.S
	${XAS} ${XASFLAGS} ${XCPUFLAGS} -c $< -o $@

${FIRMWARE_ELF}: ${XOBJDIR}/${PLATFORM}_startup.o ${PLATFORM_OBJECTS} ${COMMON_OBJECTS} ${FIRMWARE_OBJECTS}
	${XLD} ${XCPUFLAGS} -L${XOBJDIR}/ -o $@ ${XLDFLAGS} $< ${PLATFORM_OBJECTS} ${COMMON_OBJECTS} ${FIRMWARE_OBJECTS}
#	${XLD} ${XCPUFLAGS} -L${XOBJDIR}/ -o $@ ${XLDFLAGS} ${XOBJDIR}/${PLATFORM}_startup.o $< ${PLATFORM_OBJECTS} ${COMMON_OBJECTS} ${FIRMWARE_OJECTS}

%.hex: %.elf
	${XOBJCOPY} -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

%.lst: %.elf
	${XOBJDUMP} -d $< > $@


# =====================================================================================
# Clean targets.
# -------------------------------------------------------------------------------------
.PHONY: clean realclean
clean:
	-rm ${TOOLS} ${TOOLS_SRC_DIR}/*.o
	-rm -f ${XEXEDIR}/*.elf ${XEXEDIR}/*.lst ${XEXEDIR}/*.hex ${XEXEDIR}/*.S
	-rm -f ${XOBJDIR}/*.o
	-rm -Rf idrorng/Idromel.egg-info/ idrorng/build/ idrorng/dist/

realclean: clean
	-rm -Rf generated/
	-rm -Rf ${TRACES_DIR}
