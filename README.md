# AutoTile GPGPU  

![image](https://github.com/user-attachments/assets/b0e0aa8c-6ed4-47a0-bb2f-86424ffe678f)


https://github.com/user-attachments/assets/6ad1c45c-b93a-4fc6-a61c-c3ed9ddf941a



A tile automata simulator that takes advantage of GPU compute for large-scale simulations.  

## Requirements  

- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#linux) (must be installed and added to `PATH`)  
- CMake  
- A Vulkan-compatible GPU  

## Cloning the Repository  

Make sure to clone the repository with submodules to include all dependencies:  

```
git clone --recursive https://github.com/athaun/autotile_gpu.git
```

If you already cloned the repo without submodules, initialize them manually:  

```
git submodule update --init --recursive
```

## Building the Project  

Compile only
 ```
./build.sh compile [cores]
```

Compile and run
```
./build.sh run [cores]
```

Run the executable without compiling
```
./build.sh runonly
```

Clean the build directory
```
./build.sh clean
```

## License  
[MIT License](LICENSE)  
