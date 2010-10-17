#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=
CXX=
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile-netbeans

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/hasharray.o \
	${OBJECTDIR}/src/btree.o \
	${OBJECTDIR}/src/problem-ru.o \
	${OBJECTDIR}/src/problem-tsp.o \
	${OBJECTDIR}/src/heap.o \
	${OBJECTDIR}/src/set.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/problem-vp.o \
	${OBJECTDIR}/src/problem-pb.o \
	${OBJECTDIR}/src/graf.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk bin/onp-star

bin/onp-star: ${OBJECTFILES}
	${MKDIR} -p bin
	${LINK.c} -o bin/onp-star ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/hasharray.o: nbproject/Makefile-${CND_CONF}.mk src/hasharray.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/hasharray.o src/hasharray.c

${OBJECTDIR}/src/btree.o: nbproject/Makefile-${CND_CONF}.mk src/btree.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/btree.o src/btree.c

${OBJECTDIR}/src/problem-ru.o: nbproject/Makefile-${CND_CONF}.mk src/problem-ru.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/problem-ru.o src/problem-ru.c

${OBJECTDIR}/src/problem-tsp.o: nbproject/Makefile-${CND_CONF}.mk src/problem-tsp.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/problem-tsp.o src/problem-tsp.c

${OBJECTDIR}/src/heap.o: nbproject/Makefile-${CND_CONF}.mk src/heap.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/heap.o src/heap.c

${OBJECTDIR}/src/set.o: nbproject/Makefile-${CND_CONF}.mk src/set.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/set.o src/set.c

${OBJECTDIR}/src/main.o: nbproject/Makefile-${CND_CONF}.mk src/main.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/main.o src/main.c

${OBJECTDIR}/src/problem-vp.o: nbproject/Makefile-${CND_CONF}.mk src/problem-vp.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/problem-vp.o src/problem-vp.c

${OBJECTDIR}/src/problem-pb.o: nbproject/Makefile-${CND_CONF}.mk src/problem-pb.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/problem-pb.o src/problem-pb.c

${OBJECTDIR}/src/graf.o: nbproject/Makefile-${CND_CONF}.mk src/graf.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/graf.o src/graf.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} bin/onp-star

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
