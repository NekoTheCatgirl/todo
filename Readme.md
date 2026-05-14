# Todo

A stupidly simple TUI todolist made in pure C using ncurses.

## Features

- Simple and intuitive TUI
- Add, remove, and toggle todo entries
- Cross-platform (Linux/Windows)

## Installation

### GitHub Releases

You can download pre-built binaries from the [GitHub Releases](https://github.com/NekoTheCatgirl/todo/releases).

### Building from source

#### Prerequisites

- CMake 4.3 or higher
- A C compiler supporting C23
- ncurses (Linux/Unix)

#### Build instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

To install the binary system-wide:

```bash
sudo cmake --install .
```

## License

This project is licensed under the MIT License - see the [LICENSE](License.md) file for details.
