# Optical Character Recognition

## Problem Statement
- Read the input image, template image, and ground truth files.
- Calculate the matched-spatial filter (MSF) image
- Normalize the MSF image to 8-bits.
- Loop through a range of threshold values
  - Threshold the normalized MSF image to create a binary image.
  - Loop through the ground truth locations to check a 9-by-15 area centered at the ground truth and check if any pixel in the MSF image within that window is greater than threshold
    to consider the letter detected.
  - Count the detected and undetected letters, and categorize the detections as TP, FP, TN, and FN based on the similarity with the template letter.
    
## Results
- From left to right: Original Image, Matched Spatial Filter Image
<p float="left">
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab2-Optical-Character-Recognition/parenthood.PNG" width="300" />
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab2-Optical-Character-Recognition/msf.png" width="300" />
</p>

- Receiver Operator Characteristic Curve 
<p float="left">
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab2-Optical-Character-Recognition/ROC%20curve.png" width="375" />
</p>

## Instructions
Compile the code (same instructions for 'Ubuntu' and 'Linux')
 
`gcc -o insert_executable_file_name insert_code_file -lm`

`./executable_file_name`
