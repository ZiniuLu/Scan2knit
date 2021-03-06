# Scan2knit project

The implementation of my master thesis "Automatische Herstellung von 3D-gestrichten technischen Textilien" (Automated Production of 3D knitted Technical Textiles).



## Status

![Scan2knit](image/20210609_Scan2knit_GUI.JPG)

![GUI](image/20210609_Scan2knit_Console.JPG)

![mesh](image/20210609_mesh.JPG)

![skel_ext](image/20210609_skel_ext.JPG)

![skeleton](image/20210609_skeleton.JPG)

![hand with extensions](image/20210415_hand_with_extension_1.png)

![Console output](image/20210415_Output_from_VSConsole.png)



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

