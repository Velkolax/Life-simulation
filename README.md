# Life simulation

## About
Life simulation is an evolutionary multi-species simulator where microorganisms compete for limited resources on a hexagonal grid. 
They can freely move, eat, breed or fight, all depending on the output of their own Neural Network. 
We optimised all the heavy calculations by leveraging the GPU using GLSL compute shaders.
![Evolutionary Simulation in action](sim.gif)

## How to turn it on?
On Windows you just need to clone the repo and use the **life-sim.exe** file included in the main directory. On linux 
you can use linux executable file **life-sim**. Remember to not take them out of the main directory!


## Building instructions
You can also build the program yourself. We recommend cmake with vcpkg to handle all dependencies. All needed 
libraries are included in **vcpkg.json**.

## Controls
While inside the simulation you can use the following controls:

| Key    | Action                                        |
|--------|-----------------------------------------------|
| P      | Toggle to turn the simulation on or off       |
| K      | Toggle to resource visibility (on by default) |
| L      | Toggle to white background (on by default)    |
| WASD   | Controls to move through the simulation map   |
| ENTER  | Go one step through the simulation            |
| SPACE  | Go through the simulation while it is pushed  |
| SCROLL | To change the size of the map                 |
| F      | Toggle to fullscreen (off by default)         |
