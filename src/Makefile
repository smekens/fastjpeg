##############################################################################

PROJECT_DIR = ..

SRC_DIR = $(PROJECT_DIR)/src
BIN_DIR = $(PROJECT_DIR)/bin
LIB_DIR = $(PROJECT_DIR)/lib
INC_DIR = $(PROJECT_DIR)/include

##############################################################################

LIB = -lpthread 
INC = -I$(INC_DIR)
CFLAGS = -Wall -Werror -g -O0

##############################################################################

SRCS = \
	$(SRC_DIR)/decode.c \
	$(SRC_DIR)/dht.c \
	$(SRC_DIR)/dqt.c \
	$(SRC_DIR)/encode.c \
	$(SRC_DIR)/fastjpeg.c \
	$(SRC_DIR)/io.c \
	$(SRC_DIR)/jfif.c \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/memory.c \
	$(SRC_DIR)/panic.c \
	$(SRC_DIR)/sof.c

OBJS = $(SRCS:.c=.o)

##############################################################################

all: $(OBJS)
	gcc $(LIB) -o $(BIN_DIR)/fastjpeg $(OBJS)

clean:
	rm -fr $(OBJS) $(BIN_DIR)/fastjpeg

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	gcc $(CFLAGS) $(INC) -c -o $@ $<

##############################################################################

