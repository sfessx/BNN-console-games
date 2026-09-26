# BNN-console-games

A modular C++ console game platform containing multiple playable games with shared statistics, leaderboard, and security components.

## Features

- 12 console games
- Modular game architecture using DLLs
- Statistics system
- Leaderboard support
- Security module
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
├── README.md
├── LICENSE
```
## Requirements

- Windows
- MSVC

## Run

After compilation, run:

```bash
main.exe
```

The generated DLL files are placed in the `games/` and `libs/` directories.

## License

This project is licensed under the WTFPL license.

See [LICENSE](LICENSE) for details.

##Author

README was written by sfessx on 26th Sep. 2026.
Release was encoded by Madlc-314.

