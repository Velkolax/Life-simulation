# Life simulation

## About
Life simulation is an evolutionary multi-species simulator where microorganisms compete for limited resources on a hexagonal grid. 
They can freely move, eat, breed or fight, all depending on the output of their own Neural Network. 
We optimised all the heavy calculations by leveraging the GPU using GLSL compute shaders. We also added a simple UI in QT.

<p align="center">
  <img src="sim2.gif" width=90% alt="Evolutionary Simulation Demo">
</p>

## How to turn it on?
Go to the releases section and choose the one you prefer. There is the new [Qt](https://github.com/Velkolax/Life-simulation/releases/tag/Qt) release on linux and also older GLFW releases on [linux](https://github.com/Velkolax/Life-simulation/releases/tag/linux) or [windows](https://github.com/Velkolax/Life-simulation/releases/tag/windows)


## Building instructions
You can also build the program yourself. We recommend cmake with vcpkg to handle all dependencies. **Qt** is needed to build the newer versions. The rest of the 
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
