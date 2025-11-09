CXX = g++
CFLAGS = -std=c++11 -O2 -Wall -g

TARGET = main
OBJS = ../src/Formatter.cpp ../src/main.cpp ../src/LogSink.cpp ../src/Logger.cpp

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o ../bin/$(TARGET)

# clean:
#	rm -rf ../bin/$(OBJS) $(TARGET)
