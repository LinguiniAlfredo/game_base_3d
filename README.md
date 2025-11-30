# Game Base 3D

<img src="https://github.com/user-attachments/assets/72d9afd3-fdec-4015-adf3-ffa2b3428131" width=50% height=50% />

## Installation (Linux)
You'll need SDL2, OpenGL, and Assimp libraries to compile. Use your corresponding package manager.
### Arch example
```bash
sudo pacman -S sdl2
sudo pacman -S glew
sudo pacman -S glm
sudo pacman -S assimp
```

Then just run:
```bash
make clean # cleans root directory of build files
make       # builds game executable

# optional
make debug # runs with valgrind enabled (requires valgrind)
```
