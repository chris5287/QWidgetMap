# What is QWidgetMap?
QWidgetMap is a mapping library that provides a Qt QWidget class that you can use in your own applications

The library design and functionality was originally based on the QMapControl project (http://www.medieninf.de/qmapcontrol)

Features:
- Drawable Items:
 - Maps: Supports tile map services (also known as 'Slippy Map')
 - Geometries: Supports points, lines, ellipses, polygons and other custom shapes (arrows, circles, images and text)
 - Other: Supports ESRI Shapefile ([see below](#optional-external-dependencies))
- Layers: 
 - Maps, geometries and other drawable items can be added to a layer, which can be shown or hidden as required

## License
[GNU Lesser General Public License (LGPL) version 3](LICENSE)

## Prerequisites
### Compiler
- A modern C++ compiler that includes support for the C++11 standard
  - Tested with GCC 4.9, Clang 3.6 and MSVC 12 (VS 2013)

### Required External Dependencies
- Qt 5 (http://qt.io)
  - Tested with Qt 5.4

### Optional External Dependencies
- GDAL (http://www.gdal.org)
  - Adds support for ESRI Shapefile
  - To enable this feature, add `CONFIG+=with-gdal` to the `qmake` command
    - You can specify the include path for GDAL with the environment variable `QWM_GDAL_INC`
    - You can specify the library path for GDAL with the environment variable `QWM_GDAL_LIB`
  - Tested with GDAL 1.11
  
### Internal Dependencies
- QProgressIndicator (https://github.com/mojocorp/QProgressIndicator)
  - Version: [Commit hash - 0b54d4d](https://github.com/mojocorp/QProgressIndicator/tree/0b54d4d/) with modifications
  - License type: [GNU Lesser General Public License (LGPL) version 3](https://github.com/mojocorp/QProgressIndicator/blob/0b54d4d/LICENSE)

## Building QWidgetMap
Note: It is recommended to use a 'shadow build' folder when building QWidgetMap

From the root directory run the following commands to build QWidgetMap within a 'shadow build' folder (replace `make` with `nmake` for MSVC):
```Shell
mkdir build
cd build
qmake ../src
make
````
To install QWidgetMap, run:
````
make install
````
### Installation directory
Add `prefix=`**INSTALL_LOCATION** to the `qmake` command to specify where QWidgetMap is installed

Note: **INSTALL_LOCATION** must be an absolute path to the install directory required

### Build the Example project
Add `CONFIG+=with-example` to the `qmake` command 

### Build the Qt Designer plugin projects
Add `CONFIG+=with-plugins` to the `qmake` command