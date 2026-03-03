CXX = g++
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SRCDIR = src
INCDIR = include
TARGET = poker

# Ajouter poker2.cpp aux sources
SOURCES = $(SRCDIR)/TableDePoker.cpp $(SRCDIR)/main.cpp $(SRCDIR)/PokerGameBridge.cpp poker2.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) $(SOURCES) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: clean