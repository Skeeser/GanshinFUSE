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
A class UFS file system based on libfuse.  
If you find it helpful, please give it a star :wink:​  


<br>

</div>

<br>

## Features
- Created a UFS-like file system using the libfuse framework, managing the file system using inodes
- Manage free blocks and free inodes using bitmaps, file data blocks using direct and indirect indexing, and support multi-level directories.
- File and directory addition, deletion, modification, and search all utilize hash tables for higher efficiency compared to conventional sequential storage
- Code style mimics C++ exception mechanisms, making error traceback easy
- Used shell scripts to install dependencies, one command for easy installation of dependencies, farewell to complex environment configurations
- Wrote CMakeList.txt, built with cmake to generate makefiles, simple and efficient, strong cross-platform compatibility
- Utilized gtest framework for unit testing, resulting in stronger code robustness and reliable functionality

<br>

## File Structure

- doc => Stores development documents
  - develop_log.md => Development log
- include => Header files
- src => Source code
- assets => Resource files
- build => Compiled files
- example => Stores example files
- out => Stores disk files
- test => Test code
- CMakeLists.txt => CMake configuration file
- build.sh => Build script
- README.md => Documentation  

<br>

## System Architecture

Linux

<br>


## Dependency Check
- cmake
- gcc
- libfuse
- gtest (for development testing)
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
8MB size
```shell
dd bs=1M count=8 if=/dev/zero of=~/GanshinFUSE/out/diskimg
```
  
<br>

### Set disk address
Need to modify `~/GanshinFUSE/include/config.h`:
```c
#define DISK_PATH "/home/keeser/GanshinFUSE/out/diskimg"
```
Set the absolute path to your disk

<br>

## Compilation
### Set debug or release version
Modify CMakeList.txt:
```c
option(USE_DEBUG "Build with debug flags" ON)
```
Debug version--ON  
Release version--OFF  

<br>

### Perform compilation
```shell
cd ~/GanshinFUSE/
mkdir build
cd build
cmake .. 
make
```

<br>

## Run
### In the build folder
```shell
cd build/
```
<br>

### Initialization

```shell
./GanshinInit
```

You will see the logo of this project

<div align="left" >
<img alt="logo2" src="https://github.com/Skeeser/GanshinFUSE/blob/main/assets/logo.png" />
</div>

<br>

### Create a new mounting folder
Create a folder to mount the file system, any folder in any location will do  
Here, create the folder inside the build folder  
```shell
cd build/
mkdir mountdir
```

<br>

### Mount the file system
GanshinFS start  
```shell
# File system running in the background by default
./GanshinFS ./mountdir

# Force the file system to run in the foreground
./GanshinFS -f ./mountdir

# Run the file system in debug mode
./GanshinFS -d ./mountdir
```

<br>

### Unmount the file system
```shell
fusermount -u testmount
```

<br>

### Run tests
```shell
./GanhsinTest
```

<br>

## Debugging Tools
LLDB
For detailed usage, see [Development Document](doc/develop_log.md)

<br>

## User Guide
- Check if the dependencies are installed

<br>

## How to Contribute
If you've come across this project and want to improve it, strongly recommend reading the documents below:

- [How to participate in open source projects on Github](doc/github参与开源项目流程.md)
- [Development Document](doc/develop_log.md)

<br>

## About the Author

keeser

<br>

## TODO

- [x] Understand the development plan first
- [x] Develop GanshinInit
- [x] Develop GanshinFS
- [x] See if unit testing can be done
- [ ] Try to combine the written GanshinFuse with buildroot to burn it on an embedded board
- [ ] After testing, organize the open source repository
- [ ] Write the experiment report

<br>

## License

MPL 2.0
