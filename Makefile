CC = g++
CFLAGS = 

SRCS = main.cpp func.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = KmapReduction

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)