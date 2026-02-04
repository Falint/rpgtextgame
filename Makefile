# TextRPG Makefile
# Professional build system with automatic dependency tracking

# ============================================
# CONFIGURATION
# ============================================
TARGET     = textrpg
BUILD_DIR  = build
SRC_DIR    = src

CC         = gcc
CFLAGS     = -Wall -Wextra -Werror -std=c99 -I$(SRC_DIR) -MMD -MP

# Debug build (uncomment for debugging)
# CFLAGS += -g -O0 -DDEBUG

# Release build (uncomment for release)
# CFLAGS += -O2 -DNDEBUG

# ============================================
# SOURCE FILES
# ============================================
# Find all .c files recursively
SRCS := $(shell find $(SRC_DIR) -name '*.c')

# Convert source paths to object paths in build directory
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Dependency files for incremental builds
DEPS := $(OBJS:.o=.d)

# ============================================
# TARGETS
# ============================================

# Default target: build the game
all: $(BUILD_DIR)/$(TARGET)

# Link all object files into the executable
$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	@$(CC) $(OBJS) -o $@
	@echo "Build complete: $@"

# Compile each .c file to .o, maintaining directory structure
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Include generated dependency files
-include $(DEPS)

# ============================================
# PHONY TARGETS
# ============================================
.PHONY: all clean run rebuild info

# Remove all build artifacts
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete."

# Build and run
run: $(BUILD_DIR)/$(TARGET)
	@./$(BUILD_DIR)/$(TARGET)

# Full rebuild
rebuild: clean all

# Show build information
info:
	@echo "=== Build Info ==="
	@echo "Target:     $(TARGET)"
	@echo "Compiler:   $(CC)"
	@echo "Flags:      $(CFLAGS)"
	@echo ""
	@echo "Source files:"
	@echo "$(SRCS)" | tr ' ' '\n' | sed 's/^/  /'
	@echo ""
	@echo "Object files:"
	@echo "$(OBJS)" | tr ' ' '\n' | sed 's/^/  /'
