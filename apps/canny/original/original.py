#
# Original code accessed 06/06/2018 at 10:41am EDT from
# https://rosettacode.org/wiki/Canny_edge_detector#Python
#
# This source code originally take from rosettacode.org licensed unde
# the GNU Free Documentation License, version 1.2. Please find that
# license next to this file in the same directory.
# 
# Per the GNU Free Documentation License, version 1.2, this code is also
# licensed under GFDLv1.2 include any modifications made by the author.
#
# USC Stevens Institute for Innovation
# University of Southern California
# 1150 S. Olive Street, Suite 2300
# Los Angeles, CA 90115, USA
# ATTN: Accounting
# 
# DISCLAIMER. USC MAKES NO EXPRESS OR IMPLIED WARRANTIES, EITHER IN FACT OR 
# BY OPERATION OF LAW, BY STATUTE OR OTHERWISE, AND USC SPECIFICALLY AND 
# EXPRESSLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTY OF MERCHANTABILITY OR 
# FITNESS FOR A PARTICULAR PURPOSE, VALIDITY OF THE SOFTWARE OR ANY OTHER 
# INTELLECTUAL PROPERTY RIGHTS OR NON-INFRINGEMENT OF THE INTELLECTUAL 
# PROPERTY OR OTHER RIGHTS OF ANY THIRD PARTY. SOFTWARE IS MADE AVAILABLE 
# AS-IS. LIMITATION OF LIABILITY.  TO THE MAXIMUM EXTENT PERMITTED BY LAW, 
# IN NO EVENT WILL USC BE LIABLE TO ANY USER OF THIS CODE FOR ANY INCIDENTAL, 
# CONSEQUENTIAL, EXEMPLARY OR PUNITIVE DAMAGES OF ANY KIND, LOST GOODWILL, 
# LOST PROFITS, LOST BUSINESS AND/OR ANY INDIRECT ECONOMIC DAMAGES WHATSOEVER, 
# REGARDLESS OF WHETHER SUCH DAMAGES ARISE FROM CLAIMS BASED UPON CONTRACT, 
# NEGLIGENCE, TORT (INCLUDING STRICT LIABILITY OR OTHER LEGAL THEORY), A 
# BREACH OF ANY WARRANTY OR TERM OF THIS AGREEMENT, AND REGARDLESS OF 
# WHETHER USC WAS ADVISED OR HAD REASON TO KNOW OF THE POSSIBILITY OF 
# INCURRING SUCH DAMAGES IN ADVANCE.
# 
# For additional licensing information, please contact:
# Rakesh Pandit
# USC Stevens Institute for Innovation
# University of Southern California
# 1150 S. Olive Street, Suite 2300
# Los Angeles, CA 90115, USA
# Tel: +1 213-821-3552
# Fax: +1 213-821-5001
# Email: rakeshvp@usc.edu and CC to: accounting@stevens.usc.edu
# 


# Modified Version
# Author: Sam Skalicky, skalicky@isi.edu
# Institution: Information Sciences Institute, University of Southern California
# 


import numpy as np
from scipy.ndimage.filters import convolve, gaussian_filter

# Change #1
#this causes an error in Python 3.6, using cv2 imread/imshow instead
#from scipy.misc import imread, imshow
import cv2

def CannyEdgeDetector(im, blur = 1, highThreshold = 91, lowThreshold = 31):
    im = np.array(im, dtype=float) #Convert to float to prevent clipping values

    #Gaussian blur to reduce noise
    im2 = gaussian_filter(im, blur)

    #Use sobel filters to get horizontal and vertical gradients
    im3h = convolve(im2,[[-1,0,1],[-2,0,2],[-1,0,1]])
    im3v = convolve(im2,[[1,2,1],[0,0,0],[-1,-2,-1]])

    #Get gradient and direction
    grad = np.power(np.power(im3h, 2.0) + np.power(im3v, 2.0), 0.5)
    theta = np.arctan2(im3v, im3h)
    thetaQ = (np.round(theta * (5.0 / np.pi)) + 5) % 5 #Quantize direction

    #Non-maximum suppression
    gradSup = grad.copy()
    for r in range(im.shape[0]):
        for c in range(im.shape[1]):
            #Suppress pixels at the image edge
            if r == 0 or r == im.shape[0]-1 or c == 0 or c == im.shape[1] - 1:
                gradSup[r, c] = 0
                continue
            tq = thetaQ[r, c] % 4

            if tq == 0: #0 is E-W (horizontal)
                if grad[r, c] <= grad[r, c-1] or grad[r, c] <= grad[r, c+1]:
                    gradSup[r, c] = 0
            if tq == 1: #1 is NE-SW
                if grad[r, c] <= grad[r-1, c+1] or grad[r, c] <= grad[r+1, c-1]:
                    gradSup[r, c] = 0
            if tq == 2: #2 is N-S (vertical)
                if grad[r, c] <= grad[r-1, c] or grad[r, c] <= grad[r+1, c]:
                    gradSup[r, c] = 0
            if tq == 3: #3 is NW-SE
                if grad[r, c] <= grad[r-1, c-1] or grad[r, c] <= grad[r+1, c+1]:
                    gradSup[r, c] = 0
            
    #Double threshold
    strongEdges = (gradSup > highThreshold)

    #Strong has value 2, weak has value 1
    thresholdedEdges = np.array(strongEdges, dtype=np.uint8) + (gradSup > lowThreshold)
    
    #Tracing edges with hysteresis
    #Find weak edge pixels near strong edge pixels
    finalEdges = strongEdges.copy()
    currentPixels = []

    for r in range(1, im.shape[0]-1):
        for c in range(1, im.shape[1]-1):
            if thresholdedEdges[r, c] != 1:
                continue #Not a weak pixel

            #Get 3x3 patch
            localPatch = thresholdedEdges[r-1:r+2,c-1:c+2]
            patchMax = localPatch.max()
            if patchMax == 2:
                currentPixels.append((r, c))
                finalEdges[r, c] = 1

    #Extend strong edges based on current pixels
    while len(currentPixels) > 0:
        newPix = []
        for r, c in currentPixels:
            for dr in range(-1, 2):
                for dc in range(-1, 2):
                    if dr == 0 and dc == 0: continue
                    r2 = r+dr
                    c2 = c+dc
                    if thresholdedEdges[r2, c2] == 1 and finalEdges[r2, c2] == 0:
                        #Copy this weak pixel to final result
                        newPix.append((r2, c2))
                        finalEdges[r2, c2] = 1
        currentPixels = newPix

    return finalEdges

if __name__=="__main__":
    # Change #2
    #changing to cv2 imread
    #im = imread("test.jpg", mode="L") #Open image, convert to greyscale
    im = cv2.imread("sample.png", 0) #Open image, convert to greyscale
    cv2.imshow('img',im)

    finalEdges = CannyEdgeDetector(im)

    # Change #3
    #scale data from [0-1] to [0-255]
    finalEdges = finalEdges * 255
    #convert to uin8 data type (was float)
    finalEdges = np.array(finalEdges, dtype='uint8')

    # Change #4
    #changing to cv2 imshow
    #imshow(finalEdges)
    cv2.imshow('result',finalEdges)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
