# Ray Tracer

## Introduction
This simple ray tracer is implemented in C++,
following the tutorial from *Ray Tracing in One Weekend* as well as *Ray Tracing the Next Week*
from [raytracing.github.io](https://github.com/RayTracing/raytracing.github.io).
All books are free and open source. The aim of the project is to build a simple ray tracer from
 the ground up, while trying implementing some common-used techniques in the field of rendering.
It also serves as a playground for experimenting with other rendering algorithms.

## Features
### Materials
Available materials include:
* Lambertian
* Metal
* Dielectric
* Diffuse light: acting as light source
* Isotropic

### Textures
* Constant texture
* Checker texture
* Image texture
* Noise texture: using perlin noise

### Algorithms
Use bouding volume hierachy(BVH) to achieve faster ray intersection.

## Sample outputs

## Improvements
The original books mentioned before has provided working code of the system.
However, I've rewroted all the code, trying to improve its performance as well as readability.
* Follow google c++ guide
* Rewrite some of the if-else logic
