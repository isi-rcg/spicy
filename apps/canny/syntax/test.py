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
import cv2

def convolve(im:np.ndarray((240,320),'uint8'),kernel:np.ndarray((3,3),'int16'),out:np.ndarray((240,320),'int16')):
    '''
    #pragma HLS inline
    '''
    klen = 3
    khalf = 1
    imr = im.shape[0]
    imc = im.shape[1]
    for m in range(imr):
        for n in range(imc):
            pixel = 0
            for j in range(-khalf,khalf+1):
                for i in range(-khalf,khalf+1):
                    py = m+i
                    px = n+j
                    ky = klen-(i+khalf)-1
                    kx = klen-(j+khalf)-1
                    pix = 0
                    if px < 0 or py < 0 or py >= imr or px >= imc:
                        pix = 0
                    else:
                        pix = im[py][px]
                    pixel += pix * kernel[ky][kx]
            out[m][n]=pixel

def gaussian_filter(im:np.ndarray((240,320),'uint8'),out:np.ndarray((240,320),'uint8')):
    '''
    #pragma HLS inline
    '''
    kernel = np.array(
        [[1.0/273.0, 4.0/273.0, 7.0/273.0, 4.0/273.0,1.0/273.0],
         [4.0/273.0,16.0/273.0,26.0/273.0,16.0/273.0,4.0/273.0],
         [7.0/273.0,26.0/273.0,41.0/273.0,26.0/273.0,7.0/273.0],
         [4.0/273.0,16.0/273.0,26.0/273.0,16.0/273.0,4.0/273.0],
         [1.0/273.0, 4.0/273.0, 7.0/273.0, 4.0/273.0,1.0/273.0]],
        'float32')

    klen = 5
    khalf = 2
    imr = im.shape[0]
    imc = im.shape[1]
    for m in range(imr):
        for n in range(imc):
            pixel = 0
            for j in range(-khalf,khalf+1):
                for i in range(-khalf,khalf+1):
                    py = m+i
                    px = n+j
                    ky = klen-(i+khalf)-1
                    kx = klen-(j+khalf)-1
                    pix = 0
                    if px < 0 or py < 0 or py >= imr or px >= imc:
                        pix = 0
                    else:
                        pix = im[py][px]
                    pixel += pix * kernel[ky][kx]
            out[m][n]=pixel

def CannyEdgeDetector(im:np.ndarray((240,320),'uint8'),finalEdges:np.ndarray((240,320),'uint8')):
    '''
    #pragma SDS data access_pattern(im:SEQUENTIAL, finalEdges:SEQUENTIAL)
    #pragma SDS data mem_attribute(im:PHYSICAL_CONTIGUOUS, finalEdges:PHYSICAL_CONTIGUOUS)
    #pragma SDS data data_mover(im:AXIDMA_SIMPLE, finalEdges:AXIDMA_SIMPLE)
    ''' 
    #Gaussian blur to reduce noise
    im2 = np.ndarray(im.shape,'uint8')
    gaussian_filter(im, im2)

    #Use sobel filters to get horizontal and vertical gradients
    horiz = np.array([[-1,0,1],[-2,0,2],[-1,0,1]],'int16')
    vert = np.array([[1,2,1],[0,0,0],[-1,-2,-1]],'int16')
    im3h = np.ndarray((240,320),'int16')
    im3v = np.ndarray((240,320),'int16')
    convolve(im2,horiz,im3h)
    convolve(im2,vert,im3v)

    grad = np.ndarray((240,320),dtype='uint16')
    theta = np.ndarray((240,320),dtype='float32')
    thetaQ = np.ndarray((240,320),dtype='uint8')
    for r in range(im.shape[0]):
        for c in range(im.shape[1]):
            # Get gradient and direction
            grad[r][c] = np.power(np.power(im3h[r][c], 2.0) + np.power(im3v[r][c], 2.0), 0.5)
            theta[r][c] = np.arctan2(im3v[r][c], im3h[r][c])
            # Quantize direction
            tmp = np.round(theta[r][c] * (5.0 / np.pi))
            thetaQ[r][c] = (tmp + 5) % 5

    # Non-maximum suppression
    gradSup = np.ndarray((240,320),dtype='uint16')
    strongEdges = np.ndarray((240,320),dtype='uint8')
    thresholdedEdges = np.ndarray((240,320),dtype='uint8')
    for r in range(im.shape[0]):
        for c in range(im.shape[1]):
            gradSup[r][c] = grad[r][c]
            #Suppress pixels at the image edge
            if r == 0 or r == im.shape[0]-1 or c == 0 or c == im.shape[1] - 1:
                gradSup[r][c] = 0
                continue
            tq = thetaQ[r][c] % 4

            if tq == 0: #0 is E-W (horizontal)
                if grad[r][c] <= grad[r][c-1] or grad[r][c] <= grad[r][c+1]:
                    gradSup[r][c] = 0
            if tq == 1: #1 is NE-SW
                if grad[r][c] <= grad[r-1][c+1] or grad[r][c] <= grad[r+1][c-1]:
                    gradSup[r][c] = 0
            if tq == 2: #2 is N-S (vertical)
                if grad[r][c] <= grad[r-1][c] or grad[r][c] <= grad[r+1][c]:
                    gradSup[r][c] = 0
            if tq == 3: #3 is NW-SE
                if grad[r][c] <= grad[r-1][c-1] or grad[r][c] <= grad[r+1][c+1]:
                    gradSup[r][c] = 0
            
            # Double threshold
            # Strong has value 2, weak has value 1
            if gradSup[r][c] > 91:
                strongEdges[r][c] = 1
                thresholdedEdges[r][c] = 2
            elif gradSup[r][c] > 31:
                strongEdges[r][c] = 0
                thresholdedEdges[r][c] = 1
            else:
                strongEdges[r][c] = 0
                thresholdedEdges[r][c] = 0

    #Tracing edges with hysteresis
    #Find weak edge pixels near strong edge pixels
    currentPixels_r :List['int32:1000'] = []
    currentPixels_c :List['int32:1000'] = []
    finalEdges_buf = np.ndarray((240,320),'uint8')
    for r in range(1, im.shape[0]-1):
        for c in range(1, im.shape[1]-1):
            finalEdges_buf[r][c] = strongEdges[r][c] * 255
            if thresholdedEdges[r][c] != 1:
                continue #Not a weak pixel

            #Get 3x3 patch
            patchMax = 0
            for i in range(-1,2):
                for j in range(-1,2):
                    if patchMax < thresholdedEdges[r+i][c+i]:
                        patchMax = thresholdedEdges[r+i][c+i]
            if patchMax == 2:
                currentPixels_r.append(r)
                currentPixels_c.append(c)
                finalEdges_buf[r][c] = 255

    #Extend strong edges based on current pixels
    while len(currentPixels_r) > 0:
        r = currentPixels_r.pop()
        c = currentPixels_c.pop()
        for dr in range(-1, 2):
            for dc in range(-1, 2):
                if dr == 0 and dc == 0: continue
                r2 = r+dr
                c2 = c+dc
                if thresholdedEdges[r2][c2] == 1 and finalEdges_buf[r2][c2] == 0:
                    #Copy this weak pixel to final result
                    currentPixels_r.append(r2)
                    currentPixels_c.append(c2)
                    finalEdges_buf[r2][c2] = 255

    for r in range(0, im.shape[0]):
        for c in range(0, im.shape[1]):
            finalEdges[r][c]=finalEdges_buf[r][c]
            
if __name__=="__main__":
    # Change #2
    #changing to cv2 imread
    #im = imread("test.jpg", mode="L") #Open image, convert to greyscale
    im = cv2.imread("sample.png", 0) #Open image, convert to greyscale
    cv2.imshow('img',im)

    finalEdges = np.ndarray(im.shape,'uint8')
    CannyEdgeDetector(im,finalEdges)

    # Change #4
    #changing to cv2 imshow
    #imshow(finalEdges)
    cv2.imshow('result',finalEdges)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
