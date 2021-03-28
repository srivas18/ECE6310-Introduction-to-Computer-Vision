# Thinning Algorithm

## Problem Statement
In this project, I had to implement a Zhang-Suen thinning algorithm which would help in weeding out false positives in detecting the letter "e" in the image provided. This is project builds up on the previous one to make the optical character recognition more robust. Please read `lab3.pdf` for more details.
    
## Results
- From left to right: Original Image, Matched Spatial Filter Image
<p float="left">
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab3-Thinning%20Algorithm/Images/parenthood.PNG" width="300" />
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab3-Thinning%20Algorithm/Images/msf.png" width="300" />
</p>

- Binary image of letter "b"
<p float="left">
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab3-Thinning%20Algorithm/Images/binary.PNG" width="150" />
</p>

- Thinned image of the letter "b"
<p float="left">
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab3-Thinning%20Algorithm/Images/thinned.PNG" width="150" />
</p>

- Branchpoints and Endpoints detected in the thinned image
<p float="left">
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab3-Thinning%20Algorithm/Images/endpoint_branchpoint.png" width="150" />
</p>

- Receiver Operator Characteristic Curve 
<p float="left">
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab3-Thinning%20Algorithm/Images/ROC.png" width="450" />
</p>

## Instructions
Compile the code (same instructions for 'Ubuntu' and 'Linux')
 
`gcc -o executable_file_name code_file -lm`

`./executable_file_name`
