CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Isrc
TARGET   := textrpg.exe

SRCS := \
  src/main.cpp \
  src/domain/inventory.cpp \
  src/domain/enemy.cpp \
  src/domain/player.cpp \
  src/data/monsters.cpp \
  src/data/weapons.cpp \
  src/data/items.cpp \
  src/data/upgrades.cpp \
  src/game/item_service.cpp \
  src/game/battle_service.cpp \
  src/game/shop_service.cpp \
  src/tui/terminal.cpp \
  src/tui/renderer.cpp \
  src/tui/screens.cpp \
  src/tui/screens2.cpp \
  src/tui/screens3.cpp \
  src/tui/app.cpp

OBJS := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	del /Q $(subst /,\,$(OBJS)) $(TARGET) 2>nul || true

.PHONY: all clean