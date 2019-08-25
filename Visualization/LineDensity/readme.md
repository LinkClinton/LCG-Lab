# Line-Density

A simple C++ with Direct3D version of [line-density](https://github.com/domoritz/line-density). The main idea is same, but the implementation is different.

## Optimization

We use texture that can read and write in shader(named RWTexture in Direct3D) to count the number of pixel in scan-line instead of read all texel from texture. If there are two lines are crossed at the same line-series, the count is two. But we need to use interlock to count.

We also use the instance technology to render the lines in line-series. It reduces the performance cost of API and maximize the performance. And we can render lines at merge stage instead of render a texture(instance reduce the draw call cost and the pixel we render lines less than we render a texture at usual).

## Performance

The time to generator