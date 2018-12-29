echo off
echo Compling simple shader...

glslangValidator.exe -V simple.vert -o simple.vert.spv
glslangValidator.exe -V simple.frag -o simple.frag.spv

echo Shader compilation done.