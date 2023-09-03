# CongbRenderer

## 生成项目
目前仅支持Windows平台
```cmd
mkdir build
cd build
cmake ..
```

## 效果图例

<center> <h4>1k point lights in Sponza</h4> </center>

<center> <h4>Z slices debug</h4> </center>

<center> <h4>Cluster light count debug</h4> </center>

## 主要Features

* Clustered Forward Renderer
* Physically Based shading
* Metallic
* Cook-Torrance specular BRDF
* Ambient Occlusion & Emissive mapping
* Tangent space normal mapping
* HDR/LDR skyboxes
* Exposure
* Bloom
* Directional & point light shadows
* Compute shader based light culling


## 依赖 

* [ASSIMP](https://github.com/assimp/assimp): 模型导入.
* [imgui](https://github.com/ocornut/imgui): 即时 UI 界面.
* [glad](https://github.com/Dav1dde/glad): GL 加载生成器.
* [JSON for Modern C++](https://github.com/nlohmann/json): 场景描述和序列化.
* [OpenGL Image (GLI)](https://github.com/g-truc/gli): DDS 贴图导入.
* [OpenGL Mathematics (GLM)](https://glm.g-truc.net/0.9.9/index.html): GL 数学库.
* [SDL2](https://www.libsdl.org/download-2.0.php): 多媒体开发库.
* [stb_image](https://github.com/nothings/stb): JPG, PNG, HDR 贴图导入.
* [CMake](https://github.com/Kitware/CMake): 生成项目.
* [OpenGL](https://www.opengl.org/): All things GPU & graphics.

## References

* [HybridRenderingEngine](https://github.com/Angelo1211/HybridRenderingEngine/wiki/References): 源代码仓库.
* [Learn OpenGL](https://learnopengl.com/Introduction): 非常好的图形入门教程，不仅限于 OpenGL 教程.
* [Doom(2016) - Graphics Study](http://www.adriancourreges.com/blog/2016/09/09/doom-2016-graphics-study/): 参考了其中的深度切分.
* [Efficient Real-Time Shading with Many Lights](https://www.zora.uzh.ch/id/eprint/107598/1/a11-olsson.pdf): Cluster Shading 论文.