# maya-raycast
Maya raycast node that will return the closest intersection on an array of 
type `mesh` and `nurbsSurface` and return the closest intersection as a 
matrix. When `useDistance` is enabled the output matrix will be positioned at 
the closest intersection. When `useNormal` is enabled the normal at the 
closest intersection is used to calculate the rotation, the `inputMatrix` is
used to determine the forward and side axis.

## Installation
* Extract the content of the .rar file anywhere on disk.
* Build the plugin for a specific Maya version.
* Drag the raycast.mel file in Maya to permanently install the script.

## Compiling
Building the plugin using cmake will place the plugin in the plug-ins folder 
with a maya version divider. This will ensure the plug-in is compatible with 
the launched version of Maya.

1. Open Terminal
```
cd <PATH_TO_MODULE>
mkdir build/<MAYA_VERSION>
cd build/<MAYA_VERSION>
cmake -A x64 -T v141 -DMAYA_VERSION=<MAYA_VERSION> ../../
cmake --build . --target install --config Release
```

## Usage
Once the plug-in is build and loaded a new `raycast` node can be created.
A matrix is required to drive the `inputMatrix`, in respect to that matrix the
raycast can be performed along one of its axis, this axis is positive. The
`reverse` attribute can be used reverse the ray. When `bidirectional` is used
both directions will be queried regardless of the reverse state. 

<p align="center"><img src="icons/raycast-network-example.png?raw=true"></p>

The `inputMesh` and `inputSurface` attributes are arrays and such can contain
more than one connection, it is also fine to mix meshes and surfaces. All 
these inputs will be tested, and the closest intersection returned.

<p align="center"><img src="icons/raycast-scene-example.png?raw=true"></p>

Using `envelope`, will allow the `outputMatrix` to be blended to the original
`inputMatrix`, `useDistance` and `useNormal` have the same effect but work 
only the translation or rotation component.

The `hit` attribute is a boolean attribute that indicates if an intersection
has been performed successfully. The `hitDistance` will be a float value of
the distance, this is set to -1 if no intersection has been performed 
successfully.