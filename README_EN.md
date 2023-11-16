<div align="center">

<img alt="LOGO" src="assets/sys.jpg" width="256" height="346" />

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

[简体中文](README_ZH.md) | [English](README_EN.md)

OS File System Based on libfuse.  
A UFS-like file system based on libfuse.  
If you find it useful, please give it a star! :wink:​  


<br>

</div>

<br>

## Features
- Innovative use of shell scripts for dependency installation; one command for easy installation
- Use cmake to generate makefile for building, simple and efficient, strong cross-platform capabilities
- Use gtest for unit testing, making the code robust
- Utilize FUSE framework to create an SFS file system managed by inodes
- Similar to UFS, manages free blocks and free inodes using bitmaps; file data blocks are indexed directly and indirectly, supporting multi-level directories
- For ease of implementation, file names follow the 8.3 format: 8 bytes for the file name and 3 bytes for the extension. Future work involves implementing long file names.

<br>

## File Structure

- doc => Contains development documents
  - develop_log.md => Development log
- include => Header files
- src => Source code
- assets => Resource files
- build => Compilation files
- example => Stores example files
- out => Stores disk files
- test => Test code
- CMakeLists.txt => CMake configuration file
- build.sh => Build script

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

### One-line build script
```shell
sudo chmod +x ~/GanshinFUSE/build.sh && ~/GanshinFUSE/build.sh
```

<br>

### Create disk file
8MB in size
```shell
dd bs=1M count=8 if=/dev/zero of=~/GanshinFUSE/out/diskimg
```
  
### Set disk address
Modify `~/GanshinFUSE/include/config.h`  
```c
#define DISK_PATH "/home/keeser/GanshinFUSE/out/diskimg"
```
Set the absolute path to your disk

<br>

## Compilation
### Set debug or release version
Modify in CMakeList.txt  
```c
option(USE_DEBUG "Build with debug flags" ON)
```
debug version--ON  
release version--OFF  

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

## Running
### In the build folder
```shell
cd build/
```

### Initialization

```shell
./GanshinInit
```

You should see the logo of this project

<div align="left" >
<img alt="logo2" src="assets/logo2.png" />
</div>

<br>

### Start GanshinFS
```shell
./GanshinFS
```

### Run tests
```shell
./GanhsinTest
```

## Debugging Tool
LLDB
For detailed usage, refer to [Development Documentation](doc/develop_log.md)

<br>

## User Guide
- Ensure all dependencies are installed

<br>

## How to Contribute
If you come across this project and wish to improve it, we highly recommend reading the following documents:

- [How to Participate in Open Source Projects with GitHub](doc/github参与开源项目流程.md)
- [Development Documentation](doc/develop_log.md)

<br>

## About the Author

keeser

<br>

## TODO

- [x] Understand the development plan first
- [x] Develop GanshinInit
- [ ] Develop GanshinFS
- [x] See if unit testing is feasible
- [ ] Write all steps into scripts
- [ ] Try integrating GanshinFuse with buildroot for burning on embedded boards
- [ ] Organize the open-source repository after successful testing
- [ ] Write the experiment report

<br>

## License

MPL 2.0


