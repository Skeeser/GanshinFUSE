<div align="center">

<img alt="LOGO" src="https://github.com/Skeeser/GanshinFUSE/blob/main/assets/sys.jpg" width="256" height="346" />

# GanshinFUSE

<br>

<div>
    <img alt="C" src="https://img.shields.io/badge/C-gcc-%2300599C">
</div>
<div>
    <img alt="platform" src="https://img.shields.io/badge/platform-Linux%20-blueviolet">
</div>
<div>
    <img alt="license" src="https://img.shields.io/github/license/Skeeser/GanshinFUSE">
    <img alt="commit" src="https://img.shields.io/github/commit-activity/m/Skeeser/GanshinFUSE?color=%23ff69b4">
    <img alt="stars" src="https://img.shields.io/github/stars/Skeeser/GanshinFUSE?style=social">
</div>
<br>

[简体中文](doc/README_ZH.md) | [English](doc/README_EN.md)

OS File System Based on libfuse.  
基于libfuse实现的类UFS文件系统  
If you find it helpful, please consider giving it a star :wink:​  

<br>

</div>

<br>

## Features
- Utilizes the libfuse framework to create a UFS-like file system, managing the file system using inodes.
- Manages free blocks and free inodes using bitmaps, employs direct and indirect indexing for file data blocks, supports multi-level directories.
- Reduces file I/O operations intentionally, caching in memory and writing changes to files only at the end, making file operations quicker and more efficient.
- Uses hash tables for file and directory operations, providing higher efficiency compared to conventional sequential storage.
- Clear and uniform function and variable names, aiming for code clarity even without extensive comments; the function names, purpose, and return values are standardized with default 0 return for success. Examples can be found in (FileOper.h).
- Code style mimics C++ exception handling, facilitating error traceback. Strict memory management is enforced to prevent memory leaks.
- Utilizes shell scripts for dependency installation, enabling easy installation with a single command, eliminating complex environment setups.
- Includes CMakeList.txt for generating makefiles using cmake, offering simplicity, efficiency, and strong cross-platform capabilities.
- Employs the gtest framework for unit testing, enhancing code robustness and reliability.
<br>

## File Structure

- doc => stores development documents
  - develop_log.md => development log
- include => header files
- src => source code
- assets => resource files
- build => compiled files
- example => example files
- out => stores disk files
- test => test code
- CMakeLists.txt => CMake configuration file
- build.sh => build script
- README.md => documentation  

<br>

## System Architecture

Linux

<br>


## Dependency Check
- cmake
- gcc
- libfuse
- gtest (for unit testing)
- lldb (for debugging)

<br>

## Build
### Download this project
```shell
cd ~
git clone https://github.com/Skeeser/GanshinFUSE.git
```

<br>

### One-line script for building
```shell
sudo chmod +x ~/GanshinFUSE/build.sh && ~/GanshinFUSE/build.sh
```

<br>

### Create disk file
8MB in size
```shell
dd bs=1M count=8 if=/dev/zero of=~/GanshinFUSE/out/diskimg
```
  
<br>

### Set disk address
Modify `~/GanshinFUSE/include/config.h`:
```c
#define DISK_PATH "/home/keeser/GanshinFUSE/out/diskimg"
```
Set the absolute path for your disk

<br>

## Compilation
### Set debug or release version
Modify CMakeList.txt:
```c
option(USE_DEBUG "Build with debug flags" ON)
```
debug version--ON  
release version--OFF  
Note: Setting release version won't compile gtest code. If gtest installation is needed, refer to [development document](doc/develop_log.md)  

<br>

### Execute compilation
```shell
cd ~/GanshinFUSE/
mkdir build
cd build
cmake .. 
make
```

<br>

## Execution
### In the build folder
```shell
cd build/
```
<br>

### Initialization

```shell
./GanshinInit
```

You should see the logo of this project

<div align="left" >
<img alt="logo2" src="https://github.com/Skeeser/GanshinFUSE/blob/main/assets/logo.png" />
</div>

<br>

### Create a mount directory
Create a folder to mount the file system, can be any location
Here, create the folder inside the build folder
```shell
cd build/
mkdir mountdir
```

<br>

### Mount the file system
Start GanshinFS
```shell
# File system runs in the background by default
./GanshinFS ./mountdir

# Force the file system to run in the foreground
./GanshinFS -f ./mountdir

# Run the file system in debug mode
./GanshinFS -d ./mountdir
```

<br>

### Unmount the file system
```shell
fusermount -u mountdir
```

<br>

### Run tests
```shell
./GanshinTest
```

<br>

## Debugging Tools
LLDB
For detailed usage, refer to [development document](doc/develop_log.md)

<br>

## User Guide
- Ensure all dependencies are installed

<br>

## How to Contribute
If you come across this project and want to improve it
Strongly suggest reading the following documents

- [How to Participate in Open Source Projects on Github](doc/github参与开源项目流程.md)
- [Development Document](doc/develop_log.md)

<br>

## About the Author

keeser

<br>

## TODO

- [x] Understand the development plan first
- [x] Develop GanshinInit
- [x] Develop GanshinFS
- [x] See if single unit tests can be done
- [ ] Try integrating the written GanshinFuse with buildroot to burn it on an embedded board
- [x] Organize the open source repository after passing the test
- [x] Write experimental report

<br>

## License

MPL 2.0
