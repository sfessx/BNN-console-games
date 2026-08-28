# ============================================================================
# BNN Console Games Platform - Makefile
# ============================================================================
# Windows + MinGW-w64 + mingw32-make
#
# Project structure:
#   main.cpp              -> main.exe
#   games/*.cpp           -> games/*.dll
#   libs/*.cpp            -> libs/*.dll
#
# Source files are kept in the repository.
# Compiled .exe and .dll files are ignored by Git.
# ============================================================================

# ----------------------------------------------------------------------------
# Compiler settings
# ----------------------------------------------------------------------------

CXX = g++

CXXFLAGS = -std=c++17 -Wall -O2 -D_WIN32_WINNT=0x0601

# Build DLLs
DLLFLAGS = -shared

# Windows libraries
LDFLAGS = -lws2_32 -lwinmm -lkernel32 -luser32


# ----------------------------------------------------------------------------
# Directories
# ----------------------------------------------------------------------------

LIBS_DIR = libs
GAMES_DIR = games


# ----------------------------------------------------------------------------
# Main executable
# ----------------------------------------------------------------------------

MAIN_TARGET = main.exe


# ----------------------------------------------------------------------------
# System DLLs
# ----------------------------------------------------------------------------

SYSTEM_DLLS = \
	$(LIBS_DIR)/security.dll \
	$(LIBS_DIR)/stats.dll \
	$(LIBS_DIR)/lib_leaderboard.dll


# ----------------------------------------------------------------------------
# Game DLLs
# ----------------------------------------------------------------------------

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


# ----------------------------------------------------------------------------
# All build targets
# ----------------------------------------------------------------------------

ALL_TARGETS = $(MAIN_TARGET) $(SYSTEM_DLLS) $(GAME_DLLS)


# ============================================================================
# Build
# ============================================================================

# Default target
all: directories $(ALL_TARGETS)
	@echo.
	@echo ============================================
	@echo Build completed successfully!
	@echo ============================================
	@echo Main program: $(MAIN_TARGET)
	@echo System DLLs:  $(LIBS_DIR)\
	@echo Game DLLs:    $(GAMES_DIR)\
	@echo ============================================


# Create required directories
directories:
	@if not exist "$(LIBS_DIR)" mkdir "$(LIBS_DIR)"
	@if not exist "$(GAMES_DIR)" mkdir "$(GAMES_DIR)"
	@echo Directories ready.


# ============================================================================
# Main Executable
# ============================================================================

$(MAIN_TARGET): main.cpp
	@echo.
	@echo [BUILD] Main launcher: main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)
	@echo [OK] Main launcher compiled.


# ============================================================================
# System DLLs
# ============================================================================

$(LIBS_DIR)/security.dll: libs/security.cpp
	@echo.
	@echo [BUILD] Security module: libs/security.cpp
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)
	@echo [OK] Security module compiled.


$(LIBS_DIR)/stats.dll: libs/stats.cpp
	@echo.
	@echo [BUILD] Stats system: libs/stats.cpp
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)
	@echo [OK] Stats system compiled.


$(LIBS_DIR)/lib_leaderboard.dll: libs/lib_leaderboard.cpp
	@echo.
	@echo [BUILD] Leaderboard: libs/lib_leaderboard.cpp
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)
	@echo [OK] Leaderboard compiled.


# ============================================================================
# Game DLLs
# ============================================================================

$(GAMES_DIR)/game_2048.dll: games/game_2048.cpp
	@echo.
	@echo [GAME] 2048
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)


$(GAMES_DIR)/game_dino.dll: games/game_dino.cpp
	@echo.
	@echo [GAME] Chrome Dino
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)


$(GAMES_DIR)/game_flappy.dll: games/game_flappy.cpp
	@echo.
	@echo [GAME] Flappy Bird
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)


$(GAMES_DIR)/game_guess.dll: games/game_guess.cpp
	@echo.
	@echo [GAME] Guess Number
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)


$(GAMES_DIR)/game_guess_plus.dll: games/game_guess_plus.cpp
	@echo.
	@echo [GAME] Guess Number Plus
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)


$(GAMES_DIR)/game_guess_ultimate.dll: games/game_guess_ultimate.cpp
	@echo.
	@echo [GAME] Guess Number Ultimate
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)


$(GAMES_DIR)/game_mine.dll: games/game_mine.cpp
	@echo.
	@echo [GAME] Minesweeper
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)


$(GAMES_DIR)/game_mine_plus.dll: games/game_mine_plus.cpp
	@echo.
	@echo [GAME] Minesweeper Plus
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)


$(GAMES_DIR)/game_mine_ultimate.dll: games/game_mine_ultimate.cpp
	@echo.
	@echo [GAME] Minesweeper Ultimate
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)


$(GAMES_DIR)/game_snake.dll: games/game_snake.cpp
	@echo.
	@echo [GAME] Snake
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)


$(GAMES_DIR)/game_space.dll: games/game_space.cpp
	@echo.
	@echo [GAME] Space Invaders
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)


$(GAMES_DIR)/game_tetris.dll: games/game_tetris.cpp
	@echo.
	@echo [GAME] Tetris
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $< $(LDFLAGS)


# ============================================================================
# Maintenance
# ============================================================================

# Remove compiled files only.
# IMPORTANT:
#   This does NOT delete .cpp source files.
clean:
	@echo.
	@echo [CLEAN] Removing compiled files...
	@if exist "$(MAIN_TARGET)" del /Q "$(MAIN_TARGET)"
	-del /Q "$(LIBS_DIR)\*.dll" >nul 2>&1
	-del /Q "$(GAMES_DIR)\*.dll" >nul 2>&1
	@echo [OK] Clean complete.


# Clean and rebuild everything
rebuild: clean all
	@echo.
	@echo [OK] Rebuild complete.


# ============================================================================
# Help
# ============================================================================

help:
	@echo.
	@echo ============================================================
	@echo BNN Console Games Platform - Makefile Help
	@echo ============================================================
	@echo.
	@echo Available commands:
	@echo.
	@echo   mingw32-make         Build the complete project
	@echo   mingw32-make all     Build the complete project
	@echo   mingw32-make clean   Remove EXE and DLL files
	@echo   mingw32-make rebuild Clean and rebuild everything
	@echo   mingw32-make help    Show this help
	@echo.
	@echo Output:
	@echo.
	@echo   Main program:
	@echo     main.exe
	@echo.
	@echo   System DLLs:
	@echo     libs/
	@echo.
	@echo   Game DLLs:
	@echo     games/
	@echo.
	@echo ============================================================


# ============================================================================
# Phony targets
# ============================================================================

.PHONY: all directories clean rebuild help


# ============================================================================
# Environment
# ============================================================================
#
# Required:
#   - MinGW-w64
#   - g++
#   - mingw32-make
#   - C++17 support
#
# Usage:
#
#   mingw32-make
#
#   mingw32-make clean
#
#   mingw32-make rebuild
#
#   mingw32-make help
#
# ============================================================================
