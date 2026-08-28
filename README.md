# BNN-console-games

A modular C++ console game platform containing multiple playable games with shared statistics, leaderboard, and security components.

## Features

- 12 console games
- Modular game architecture using DLLs
- Statistics system
- Leaderboard support
- Security module
- Automated compilation with Makefile
- C++17

## Games

| Game | Source |
|---|---|
| 2048 | `games/game_2048.cpp` |
| Chrome Dino | `games/game_dino.cpp` |
| Flappy Bird | `games/game_flappy.cpp` |
| Guess Number | `games/game_guess.cpp` |
| Guess Number Plus | `games/game_guess_plus.cpp` |
| Guess Number Ultimate | `games/game_guess_ultimate.cpp` |
| Minesweeper | `games/game_mine.cpp` |
| Minesweeper Plus | `games/game_mine_plus.cpp` |
| Minesweeper Ultimate | `games/game_mine_ultimate.cpp` |
| Snake | `games/game_snake.cpp` |
| Space Invaders | `games/game_space.cpp` |
| Tetris | `games/game_tetris.cpp` |

## Project Structure

```text
BNN-console-games/
│
├── games/
│   ├── game_2048.cpp
│   ├── game_dino.cpp
│   ├── game_flappy.cpp
│   ├── game_guess.cpp
│   ├── game_guess_plus.cpp
│   ├── game_guess_ultimate.cpp
│   ├── game_mine.cpp
│   ├── game_mine_plus.cpp
│   ├── game_mine_ultimate.cpp
│   ├── game_snake.cpp
│   ├── game_space.cpp
│   └── game_tetris.cpp
│
├── libs/
│   ├── lib_leaderboard.cpp
│   ├── security.cpp
│   └── stats.cpp
│
├── main.cpp
├── Makefile
├── README.md
├── LICENSE
└── .gitignore
```

## Libraries

The `libs/` directory contains shared components used by the game platform:

- `lib_leaderboard.cpp` — Leaderboard functionality
- `security.cpp` — Security-related functionality
- `stats.cpp` — Statistics functionality

## Requirements

- Windows
- MinGW-w64 / g++
- C++17 support
- GNU Make

## Build

Clone the repository:

```bash
git clone https://github.com/sfessx/BNN-console-games.git
cd BNN-console-games
```

Build the project:

```bash
mingw32-make
```

The Makefile compiles:

- `main.cpp` into the main executable
- Game source files into game DLLs
- Library source files into library DLLs

## Run

After compilation, run:

```bash
main.exe
```

The generated DLL files are placed in the `games/` and `libs/` directories.

## Makefile Commands

### Build

```bash
ming32-make
```

Build the complete project.

### Clean

```bash
mingw32-make clean
```

Remove compiled binaries.

### Rebuild

```bash
mingw32-make rebuild
```

Clean and rebuild the project.

### Help

```bash
mingw32-make help
```

Display available Makefile commands.

## Repository Notes

Compiled files such as `.exe` and `.dll` are excluded from the source repository using `.gitignore`.

Runtime data files such as `users.txt` and `stats.txt` are also excluded from version control.

## License

This project is licensed under the WTFPL license.

See [LICENSE](LICENSE) for details.
