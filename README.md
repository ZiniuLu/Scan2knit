# Scan2knit project

The implementation of my master thesis "Automatische Herstellung von 3D-gestrichten technischen Textilien" (Automated Production of 3D knitted Technical Textiles).



## Status

![hand with extensions](https://github.com/ZiniuLu/Scan2knit/blob/master/image/20210415_hand%20with%20extension_1.png)

![Console output](https://github.com/ZiniuLu/Scan2knit/blob/master/image/20210415_Output%20from%20VSConsole.png)



## Dependencies

The only dependencies are [CGAL](https://doc.cgal.org/latest/Manual/windows.html) and [libigl](https://libigl.github.io/tutorial/). It is highly recommended to install CGAL via [vcpkg](https://github.com/microsoft/vcpkg).

The cmake-gui build system will attempt to find cgal and libigl according to environment variables and searching in common desitinations.

Compile this project using the standard cmake-gui routine:

    mkdir build
    cd build
    cmake-gui ..

This should find and build the dependencies and create a `Scan2knit` binary.



## Run

From within the `build/Release` or `build/Debug` directory just issue:

    ./Scan2knit.exe "../data/xxx.off"



## Image


