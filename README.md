# Focus Garden

A 2D isometric productivity game built with C++ and SFML. Grow a virtual garden by staying focused on real-world tasks.

## Dependencies
- `gcc` or `clang` (your compiler of choice)
- `sfml`

## How to run?

1. Make sure the above dependencies are installed on your system.

2. Run the command below to compile:
```sh
g++ *.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o game
```
Make sure to add any additional dependencies (like `l/opt/homebrew/lib` if you used Homebrew on MacOS to install SFML)
If you're using `clang`, just replace `g++` with `clang++`.

3. Run the compiled binary:
```sh
./game
```
