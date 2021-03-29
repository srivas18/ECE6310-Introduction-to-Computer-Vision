# Interactive Region Growing

## Problem Statement
In this project I implemented interactive region growing by developing a Windows program using the Win32 library. The program allows the user to load and display an image, and demonstrates several GUI and event handling techniques. The program was written using Microsoft Visual Studio 2019.

Please read `lab4.pdf` for more details.

## Implementation
Some of the salient features of the application are:
- Entering the values of the predicates the region growing is dependent on, to study its effects.
- The user can click on any location to visualize the region growing.
- Multiple region grows can be accomplished at a single time.
- The user can select the color of the pixels being drawn while region growing.
- The user can reset the results of a previous region grow.
- Two modes of region growing:
    - Auto: The new pixels of a region are drawn every 1 millisecond.
    - Step: Pixels are drawn in a region every time the user presses the key 'J'. These modes can be switched as per the users liking during the region growing too.
    
## Results
- Working video of the application
<p float="left">
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab4-Region%20Interaction/result.gif" width="500" />
</p>

## Instructions
The folder “lab4” contains the project files for the application. Open MS Visual Studio and load the file `plus.sln` into it to open the project.

Now build the solution and run it.