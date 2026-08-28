# ============================================================================
# BNN Console Games Platform - Makefile
# ============================================================================
# This Makefile compiles the main launcher and all DLL libraries/games
# ============================================================================

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -D_WIN32_WINNT=0x0601
DLLFLAGS = -shared -fPIC
LDFLAGS = -lws2_32 -lwinmm -lkernel32 -luser32

# Directories
SRC_DIR = .
BIN_DIR = bin
LIBS_DIR = libs
GAMES_DIR = games

# Main executable
MAIN_TARGET = $(BIN_DIR)/GameLauncher.exe

# System DLLs
SYSTEM_DLLS = \
	$(LIBS_DIR)/security.dll \
	$(LIBS_DIR)/stats.dll \
	$(LIBS_DIR)/lib_leaderboard.dll

# Game DLLs
GAME_DLLS = \
	$(GAMES_DIR)/game_2048.dll \
	$(GAMES_DIR)/game_dino.dll \
	$(GAMES_DIR)/game_flappy.dll \
	$(GAMES_DIR)/game_guess.dll \
	$(GAMES_DIR)/game_guess_plus.dll \
	$(GAMES_DIR)/game_guess_ultimate.dll \
	$(GAMES_DIR)/game_mine.dll \
	$(GAMES_DIR)/game_mine_plus.dll \
	$(GAMES_DIR)/game_mine_ultimate.dll \
	$(GAMES_DIR)/game_snake.dll \
	$(GAMES_DIR)/game_space.dll \
	$(GAMES_DIR)/game_tetris.dll

# All build targets
ALL_TARGETS = $(MAIN_TARGET) $(SYSTEM_DLLS) $(GAME_DLLS)

# ============================================================================
# Build Rules
# ============================================================================

# Default target
all: directories $(ALL_TARGETS)
	@echo ""
	@echo "✅ Build completed successfully!"
	@echo "📁 Output locations:"
	@echo "   Main program: $(MAIN_TARGET)"
	@echo "   System DLLs:  $(LIBS_DIR)/"
	@echo "   Game DLLs:    $(GAMES_DIR)/"
	@echo ""

# Create necessary directories
directories:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(LIBS_DIR)
	@mkdir -p $(GAMES_DIR)
	@echo "📁 Directories created"

# ============================================================================
# Main Executable
# ============================================================================

$(MAIN_TARGET): main.cpp
	@echo "🔨 Compiling main launcher: main.cpp"
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)
	@echo "✅ Main launcher compiled"

# ============================================================================
# System DLLs
# ============================================================================

$(LIBS_DIR)/security.dll: libs/security.cpp
	@echo "🔨 Compiling security module: libs/security.cpp"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)
	@echo "✅ Security module compiled"

$(LIBS_DIR)/stats.dll: libs/stats.cpp
	@echo "🔨 Compiling stats system: libs/stats.cpp"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)
	@echo "✅ Stats system compiled"

$(LIBS_DIR)/lib_leaderboard.dll: libs/lib_leaderboard.cpp
	@echo "🔨 Compiling leaderboard: libs/lib_leaderboard.cpp"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)
	@echo "✅ Leaderboard compiled"

# ============================================================================
# Game DLLs
# ============================================================================

$(GAMES_DIR)/game_2048.dll: games/game_2048.cpp
	@echo "🎮 Compiling game: 2048"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)

$(GAMES_DIR)/game_dino.dll: games/game_dino.cpp
	@echo "🎮 Compiling game: Chrome Dino"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)

$(GAMES_DIR)/game_flappy.dll: games/game_flappy.cpp
	@echo "🎮 Compiling game: Flappy Bird"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)

$(GAMES_DIR)/game_guess.dll: games/game_guess.cpp
	@echo "🎮 Compiling game: Guess Number"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)

$(GAMES_DIR)/game_guess_plus.dll: games/game_guess_plus.cpp
	@echo "🎮 Compiling game: Guess Number Plus"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)

$(GAMES_DIR)/game_guess_ultimate.dll: games/game_guess_ultimate.cpp
	@echo "🎮 Compiling game: Guess Number Ultimate"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)

$(GAMES_DIR)/game_mine.dll: games/game_mine.cpp
	@echo "🎮 Compiling game: Minesweeper"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)

$(GAMES_DIR)/game_mine_plus.dll: games/game_mine_plus.cpp
	@echo "🎮 Compiling game: Minesweeper Plus"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)

$(GAMES_DIR)/game_mine_ultimate.dll: games/game_mine_ultimate.cpp
	@echo "🎮 Compiling game: Minesweeper Ultimate"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)

$(GAMES_DIR)/game_snake.dll: games/game_snake.cpp
	@echo "🎮 Compiling game: Snake"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)

$(GAMES_DIR)/game_space.dll: games/game_space.cpp
	@echo "🎮 Compiling game: Space Invaders"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)

$(GAMES_DIR)/game_tetris.dll: games/game_tetris.cpp
	@echo "🎮 Compiling game: Tetris"
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)

# ============================================================================
# Maintenance Targets
# ============================================================================

# Clean all build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -rf $(BIN_DIR) $(LIBS_DIR) $(GAMES_DIR)
	@echo "✅ Clean complete"

# Rebuild everything from scratch
rebuild: clean all
	@echo "🔄 Rebuild complete"

# Show help
help:
	@echo ""
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║  BNN Console Games Platform - Makefile Help                ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Available targets:"
	@echo ""
	@echo "  make all       - Build all DLLs and main executable (default)"
	@echo "  make clean     - Remove all build artifacts"
	@echo "  make rebuild   - Clean and build everything from scratch"
	@echo "  make help      - Display this help message"
	@echo ""
	@echo "Build details:"
	@echo "  Compiler:      $(CXX)"
	@echo "  C++ Standard:  C++17"
	@echo "  Output dirs:"
	@echo "    - Main:     $(BIN_DIR)/"
	@echo "    - System:   $(LIBS_DIR)/"
	@echo "    - Games:    $(GAMES_DIR)/"
	@echo ""
	@echo "Example usage:"
	@echo "  make          # Compile everything"
	@echo "  make clean    # Remove compiled files"
	@echo "  make rebuild  # Full rebuild"
	@echo ""

# Mark phony targets (not actual files)
.PHONY: all directories clean rebuild help

# ============================================================================
# Notes
# ============================================================================
# 
# Required environment:
#   - MinGW-w64 or MSVC compatible compiler
#   - Windows development headers
#   - std=c++17 support
#
# To use this Makefile:
#   1. Ensure all .cpp files are in correct directories
#   2. Run: make all
#   3. Output will be in bin/, libs/, and games/ directories
#
# Troubleshooting:
#   - If compilation fails, check that all dependencies are available
#   - Ensure source files are in the correct locations
#   - Check for typos in filenames (case-sensitive on some systems)
#
# ============================================================================
