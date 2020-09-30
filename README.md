# core
A collection of common tools for computer graphics applications.

So far it includes a basic opengl abstrction that sits on GLFW and imgui and a variety of general tools for geometry processing and using the eigen.
The implementation is meant to be similar to libigl (libigl.github.io), except a KEY difference for those using both is that this library uses COLUMN vectors while libigl uses ROW vectors.


### Colormap Shaders
Within CMake we currently pull colormap shaders from [kbinani/colormap-shaders](https://github.com/kbinani/colormap-shaders), with two options for accessing the data. 

First off is the raw shaders, which are found in the directory stored by the`COLORMAP_SHADERS_SHADER_DIR` CMake variable. From within that directory GLSL shaders are found in `${COLORMAP_SHADERS_SHADER_DIR}/glsl/*.frag`. 

Alternatively one can access these shaders purely with an include by adding something like 
```
TARGET_INCLUDE_DIRECTORIES(your_target PUBLIC ${COLORMAP_SHADERS_INCLUDE_DIRS})
```
Including `mtao::visualization` will do this for you.
From there one may use something akin to 

```
#include <colormap/colormap.h>

colormap::MATLAB::Parula parula;
colormap::Color test_col = parula.getColor(.4);
fmt::print("{} {} {} {}\n", test_col.r,test_col.g,test_col.b,test_col.a);
addFragmentShader(parula.getSource());

```
