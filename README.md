
# Rock-Paper-Scissors Game

## Introduction

This is a simple multiplayer Rock-Paper-Scissors game developed using C++ and the SFML library. Players can connect and play against each other in real time.

## Prerequisites

Before you begin, ensure you have the following installed:

### 1. C++ Compiler (g++)

To install the GNU C++ Compiler (g++), you can use the following commands:

```bash
sudo apt update
sudo apt install g++
```

### 2. SFML Library

To install the SFML (Simple and Fast Multimedia Library) development files, run the following commands:

```bash
sudo apt update
sudo apt install libsfml-dev
```

### 3. Meson Build System

To install the Meson build system, follow these steps:

```bash
sudo apt update
sudo apt install meson
```

### 4. Just Build Tool

To install Just, follow these steps:

1. Download the Just binary:

   ```bash
   wget https://github.com/casey/just/releases/latest/download/just-linux -O just
   ```

2. Make it executable:

   ```bash
   chmod +x just
   ```

3. Move it to a directory in your PATH:

   ```bash
   sudo mv just /usr/local/bin/
   ```

4. Verify the installation:

   ```bash
   just --version
   ```

After installing these dependencies, you should be ready to build and run the Rock-Paper-Scissors game.

## Building the Game

### Using Just

To build and run the game using Just, navigate to the project directory and run:

```bash
just build
just run-server
just run-client
```

To clean the build files
```bash
just clean
```

### Using g++

If you prefer to use g++ directly, you can compile the project with the following commands:

For the server:

```bash
g++ server.cpp gameLogic.cpp -o server -lsfml-system -lsfml-window -lsfml-graphics -lpthread
```

For the client:

```bash
g++ client.cpp -o client -lsfml-system -lsfml-window -lsfml-graphics
```

Then run the server and client with:

```bash
./server
./client
```

## Contributing

Feel free to submit issues or pull requests to contribute to the project.