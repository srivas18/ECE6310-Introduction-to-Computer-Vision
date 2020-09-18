# Convolution and Timing

## Problem Statement
- Implement three version of a 7-by-7 mean filter. Ensure handling of edge cases to give an output of zero.
- Perform 2D Convolution on the image and calculate the time it takes to execute the code.
- Use Separable filters of size 1-by-7 and 7-by-1 and calculate the execution time.
- Modify the separable filter code to account for sliding window and observe the improvement in execution time.
- Compare for similarities in the  three images using 'diff'.
- Report average time over ten runs.

## Results 
- Original Image
<p float="left">
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab1-Convolution-and-Timing/bridge.PNG" width="310" />
</p>

- From left to right: 2D Convolution, 1D Separable, 1D Seprabale-Sliding
<p float="left">
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab1-Convolution-and-Timing/smoothed7.png" width="250" />
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab1-Convolution-and-Timing/separ.png" width="250" />
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab1-Convolution-and-Timing/slide.png" width="250" />
</p>

![](https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab1-Convolution-and-Timing/time.PNG)

## Instructions
Compile the code (same instructions for 'Ubuntu' and 'Linux')
 
`gcc -o executable_file_name code_file -lm`

`./executable_file_name`