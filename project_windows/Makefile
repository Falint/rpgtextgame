# TextRPG Windows Makefile
# Compatible with MinGW/MSYS2 — uses cmd.exe-safe commands
# Build: mingw32-make   or   make  (in MSYS2)

# ============================================
# CONFIGURATION
# ============================================
TARGET    = textrpg.exe
BUILD_DIR = build
SRC_DIR   = src

CC        = gcc
CFLAGS    = -Wall -Wextra -Werror -std=c99 -I$(SRC_DIR) -MMD -MP

# Debug build (uncomment for debugging)
# CFLAGS += -g -O0 -DDEBUG

# Release build (uncomment for release)
# CFLAGS += -O2 -DNDEBUG

# ============================================
# SOURCE FILES (manually listed — no 'find')
# ============================================
SRCS = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/core/element.c \
	$(SRC_DIR)/core/enemy.c \
	$(SRC_DIR)/core/inventory.c \
	$(SRC_DIR)/core/item.c \
	$(SRC_DIR)/core/loot.c \
	$(SRC_DIR)/core/player.c \
	$(SRC_DIR)/core/weapon.c \
	$(SRC_DIR)/data/items.c \
	$(SRC_DIR)/data/monsters.c \
	$(SRC_DIR)/data/registry.c \
	$(SRC_DIR)/data/shop_stock.c \
	$(SRC_DIR)/data/upgrades.c \
	$(SRC_DIR)/data/weapons.c \
	$(SRC_DIR)/game/battle.c \
	$(SRC_DIR)/game/shop.c \
	$(SRC_DIR)/game/upgrade.c \
	$(SRC_DIR)/utils/utils.c

# Convert source paths to object paths in build directory
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Dependency files for incremental builds
DEPS = $(OBJS:.o=.d)

# ============================================
# TARGETS
# ============================================

# Default target: build the game
all: $(BUILD_DIR)/$(TARGET)

# Link all object files into the executable
$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo Linking $(TARGET)...
	@$(CC) $(OBJS) -o $@
	@echo Build complete: $@

# Compile each .c file to .o, maintaining directory structure
# Uses cmd.exe-compatible mkdir (no -p flag)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	@echo Compiling $<...
	@$(CC) $(CFLAGS) -c $< -o $@

# Include generated dependency files
-include $(DEPS)

# ============================================
# PHONY TARGETS
# ============================================
.PHONY: all clean run rebuild info

# Remove all build artifacts (cmd.exe compatible)
clean:
	@echo Cleaning build directory...
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@echo Clean complete.

# Build and run
run: $(BUILD_DIR)/$(TARGET)
	@$(BUILD_DIR)\$(TARGET)

# Full rebuild
rebuild: clean all

# Show build information
info:
	@echo === Build Info ===
	@echo Target:     $(TARGET)
	@echo Compiler:   $(CC)
	@echo Flags:      $(CFLAGS)
	@echo.
	@echo Source files:
	@echo $(SRCS)
