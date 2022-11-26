SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.hpp)
OBJECTS = $(SOURCES:.cpp=.o)

TARGET = rand-test
CXXFLAGS = -pipe -O2 -Wall -Wextra

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

$(OBJECTS): $(HEADERS)

clean:
	$(RM) $(TARGET) $(OBJECTS)
