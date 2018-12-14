### Environment ###
# Set Compiler
CC := gcc
CFLAGS := -Wall 

# Directories
DIR_SRC := ./src
DIR_OBJ := ./build
DIR_BIN := ./bin

# Targets
TARGETS = server client

### Parse ###
DIR_CREATE := ${DIR_OBJ} ${DIR_BIN}
SRC = $(wildcard ${DIR_SRC}/*.c)
OBJ_TARGETS = $(patsubst %, ${DIR_OBJ}/%.o, ${TARGETS})
OBJ = $(filter-out ${OBJ_TARGETS}, $(patsubst %.c, ${DIR_OBJ}/%.o, $(notdir ${SRC})))
BIN = $(patsubst %, ${DIR_BIN}/%, ${TARGETS})

### Rules ###
all: ${DIR_CREATE} ${BIN} 

# Create Directories if not exsit
${DIR_CREATE}:
	mkdir $@

# Compile
${DIR_OBJ}/%.o: ${DIR_SRC}/%.c
	$(CC) $(CFLAGS) -c  $< -o $@

# Link
${DIR_BIN}/%: ${DIR_OBJ}/%.o ${OBJ}
	$(CC) $^ -o $@

# Remove Object Files
clean: 
	rm -rf ${DIR_OBJ}

# Remove All Files Compiled
distclean: clean
	rm -rf ${DIR_BIN} ${DIR_OBJ}

.PHONY: all clean distclean
.SECONDARY: ${OBJ} 