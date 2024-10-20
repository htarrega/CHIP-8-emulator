# 🤖 CHIP-8 Emulator

## 🌟 Overview

Welcome to my CHIP-8 Emulator project! This is a project to emulate the classic CHIP-8 environment using modern C++20. The reasons for building this simulator are to go deeper into CPU architectures, to implement first-hand a cycle-by-cycle simulator and to have fun!

With this emulator, you can run vintage CHIP-8 games and programs. 

## ✨ Features

- 🚀 Full implementation of the CHIP-8 instruction set.
- 🛠️ Modular design for easy maintenance and extension.

## 📋 Prerequisites

Before you start, make sure you have the following installed:

- **CMake** (version 3.0 or higher) 📦
- **Clang** (or another C++20 compatible compiler, but you will need to modify the CMakeLists.txt) 🛠️

## ⚙️ Building and running the project

### 🐛 Debug Build
From the source of the project:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make 
```
### 🚀 Release Build
From the source of the project:
```bash
mkdir build
cd build
cmake ..
make 
```
### ▶️ Running the Emulator
In order to run the emulator, place <name>.ch8 in the rom folder. The emulator will search and load the first binary in that folder with ch8 extension. Then just execute, from bin folder:
```bash
./emu 
```

## 🤝 Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.

## 📄 License

This project is licensed under the MIT License. Check out the LICENSE file for more details.

## 🙏 Acknowledgements
[Tobias V. Langhoff's Chip-8 Technical Reference](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#annn-set-index) - An awesome guide to the CHIP-8 architecture.
