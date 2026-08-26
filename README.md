# GarnetEngine
Garnet Engine is an in-development SDL3 C++ wrapper

## Building
### Requirements
- C++23 Compiler
- CMake 3.23
- [vcpkg](https://github.com/microsoft/vcpkg)

### Instructions
Create CMakeUserPresets.json detailing the location of your vcpkg installation
```json
{
    "version": 3,
    "configurePresets": [
        {
            "name": "default",
            "inherits": "vcpkg",
            "environment": {
                "VCPKG_ROOT": "C:/vcpkg"
            }
        }
    ]
}
```
Then configure and build
```shell
cmake --preset default
cmake --build build
```



# Documentation
Documentation automatically published via github actions to 
[GarnetEngine Documentation](https://matthew2025-byte.github.io/GarnetEngine)