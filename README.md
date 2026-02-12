# Life simulation

## About
Life simulation is an evolutionary multi-species simulator where microorganisms compete for limited resources on a hexagonal grid. 
They can freely move, eat, breed or fight, all depending on the output of their own Neural Network. 
We optimised all the heavy calculations by leveraging the GPU using GLSL compute shaders. We also added a simple UI in QT.

<p align="center">
  <img src="sim.gif" width=90% alt="Evolutionary Simulation Demo">
</p>

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

## Config
All the simulation parameters can be easily configured in the [config.txt](config.txt) file where they are formatted as following:
```
parameter_name1=12
parameter_name2=15.6
parameter_name3=value
```
