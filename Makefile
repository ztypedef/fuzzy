PROJ_NAME=app
STM_COMMON=src

CC=g++

CFLAGS  = -g -O1 -Wall -std=c++11 
CFLAGS += -I.

OUTPUT_DIR:=build

CFLAGS += -I$(STM_COMMON)

SRCS = src/main.cpp
SRCS += src/fuzzy.cpp
SRCS += src/memcheck.cpp
SRCS += src/fuzzyplot.cpp
 
SRCSND = $(notdir $(SRCS))
OBJS := $(SRCSND:.cpp=.o)

%.o:
	mkdir -p build
	$(CC) -c -o $(OUTPUT_DIR)/$@ $^ $(filter %$(basename $@).cpp ,$(SRCS)) $(CFLAGS)


.PHONY: all
.PHONY: proj
all: proj


proj: $(PROJ_NAME)

$(PROJ_NAME): $(OBJS)
	$(CC) -o $@ $(addprefix $(OUTPUT_DIR)/, $^) $(CFLAGS) 
.PHONY: run
run: $(PROJ_NAME)
	./$(PROJ_NAME)

.PHONY: clean
clean:
	rm -f $(OUTPUT_DIR)/*.o rm $(PROJ_NAME) rm *.o
