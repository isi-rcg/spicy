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
import time
#A
def div9(n:'uint8') -> 'uint8':
    q = (n - (n >> 3))
    q = q + (q >> 6)
    q = q >> 3
    return q

def div9b(n:'uint16') -> 'uint16':
    q = (n - (n >> 3))
    q = q + (q >> 6)
    q = q + (q >> 12)
    q = q >> 3
    return q

def convolve1(im:np.ndarray((240,320),'uint8'),out:np.ndarray((240,320),'uint16')):
    klen:'uint8' = 3
    khalf:'uint8' = 1
    imr = im.shape[0]
    imc = im.shape[1]
    for m in range(imr):
        for n in range(imc):
            pixel :'uint16' = 0
            for j in range(-khalf,khalf+1):
                for i in range(-khalf,khalf+1):
                    py = m+i
                    px = n+j
                    ky = klen-(i+khalf)-1
                    kx = klen-(j+khalf)-1
                    pix : 'uint16'
                    if px < 0 or py < 0 or py >= imr or px >= imc:
                        pix = 0
                    else:
                        pix = im[py][px]
                    pixel += pix
            out[m][n]=div9b(pixel)

def convolve2(im:np.ndarray((240,320),'uint16'),kernel:np.ndarray(9,'int8'),out:np.ndarray((240,320),'int16')):
    klen:'uint8' = 3
    khalf:'uint8' = 1
    imr = im.shape[0]
    imc = im.shape[1]
    for m in range(imr):
        for n in range(imc):
            pixel :'int16' = 0
            for j in range(-khalf,khalf+1):
                for i in range(-khalf,khalf+1):
                    py = m+i
                    px = n+j
                    ky = klen-(i+khalf)-1
                    kx = klen-(j+khalf)-1
                    pix : 'int16'
                    if px < 0 or py < 0 or py >= imr or px >= imc:
                        pix = 0
                    else:
                        pix = im[py][px]
                    pixel += pix * kernel[ky*3 + kx]

            out[m][n]=pixel

def filter(im:np.ndarray((240,320),'uint8'),thresholdedEdges:np.ndarray((240,320),'uint8')):
    im_buf = np.ndarray((5,320),'uint8')
    im2 = np.ndarray((5,320),'uint16')
    #Use sobel filters to get horizontal and vertical gradients
    sobelh : np.ndarray(9,'int8') = [-1,0,1,-2,0,2,-1,0,1]
    im3h = np.ndarray((5,320),'int16')
    sobelv : np.ndarray(9,'int8') = [1,2,1,0,0,0,-1,-2,-1]
    im3v = np.ndarray((5,320),'int16')
    #Get gradient and direction
    grad = np.ndarray((5,320),'int16')

    klen:'uint8' = 3
    khalf:'uint8' = 1
    imr = im.shape[0]
    imc = im.shape[1]
    py : 'uint16'
    px : 'uint16'
    ky : 'uint16'
    kx : 'uint16'
    for m in range(imr+10):
        for n in range(imc):
            if m > 0 and m < imr: #0,1,2
                im_buf[m%5][n] = im[m][n]
            if m > 2 and m < imr+3: #3,4,5
                pixel :'uint16' = 0
                for j in range(-khalf,khalf+1):
                    for i in range(-khalf,khalf+1):
                        py = m+i-3
                        px = n+j
                        ky = klen-(i+khalf)-1
                        kx = klen-(j+khalf)-1
                        pix : 'uint16'
                        if px < 0 or py < 0 or py >= imr or px >= imc:
                            pix = 0
                        else:
                            pix = im_buf[py%5][px]
                        pixel += pix
                im2[(m-3)%5][n]=div9b(pixel)
            if m > 5 and m < (imr+6): #6
                pixelh :'int16' = 0
                pixelv :'int16' = 0
                for j in range(-khalf,khalf+1):
                    for i in range(-khalf,khalf+1):
                        py = (m+i-6)
                        px = n+j
                        ky = klen-(i+khalf)-1
                        kx = klen-(j+khalf)-1
                        pix : 'int16'
                        if px < 0 or py < 0 or py >= imr or px >= imc:
                            pix = 0
                        else:
                            pix = im2[py%5][px]
                            pixelh += pix * sobelh[ky*3 + kx]
                            pixelv += pix * sobelv[ky*3 + kx]
                im3h[(m-6)%5][n]=pixelh
                im3v[(m-6)%5][n]=pixelv

            if m > 6 and m < (imr+7): #7,8,9
                #np.float32()  np.sqrt()
                h:'int32' = im3h[(m-7)%5][n]
                v:'int32' = im3v[(m-7)%5][n]
                grad[(m-7)%5][n] = np.sqrtf(h*h + v*v)

        if m > 9:
            for n in range(imc):
                gradSup : 'int16' = grad[(m-10)%5][n]
                #Suppress pixels at the image edge
                if m-10 == 0 or m-10 == im.shape[0]-1 or n == 0 or n == im.shape[1] - 1:
                    continue
                #5/pi = 1.591549
                theta = (np.round(np.arctan2f(im3v[(m-10)%5][n], im3h[(m-10)%5][n]) * 1.59) + 5)
                tq = (theta % 5) % 4
 
                if tq == 0: #0 is E-W (horizontal)
                    if grad[(m-10)%5][n] <= grad[(m-10)%5][n-1] or grad[(m-10)%5][n] <= grad[(m-10)%5][n+1]:
                        gradSup = 0
                if tq == 1: #1 is NE-SW
                    if grad[(m-10)%5][n] <= grad[(m-1-10)%5][n+1] or grad[(m-10)%5][n] <= grad[(m+1-10)%5][n-1]:
                        gradSup = 0
                if tq == 2: #2 is N-S (vertical)
                    if grad[(m-10)%5][n] <= grad[(m-1-10)%5][n] or grad[(m-10)%5][n] <= grad[(m+1-10)%5][n]:
                        gradSup = 0
                if tq == 3: #3 is NW-SE
                    if grad[(m-10)%5][n] <= grad[(m-1-10)%5][n-1] or grad[(m-10)%5][n] <= grad[(m+1-10)%5][n+1]:
                        gradSup = 0

                #Double threshold
                #Strong has value 2, weak has value 1
                if gradSup > 91:
                    thresholdedEdges[m-10][n] = 2
                elif gradSup > 31:
                    thresholdedEdges[m-10][n] = 1
                else:
                    thresholdedEdges[m-10][n] = 0

 
def CannyEdgeDetector(im:np.ndarray((240,320),'uint8'),finalEdges:np.ndarray((240,320),'uint8')):
    '''
    #pragma SDS data access_pattern(im:SEQUENTIAL, finalEdges:SEQUENTIAL)
    #pragma SDS data mem_attribute(im:PHYSICAL_CONTIGUOUS, finalEdges:PHYSICAL_CONTIGUOUS)
    #pragma SDS data data_mover(im:AXIDMA_SIMPLE, finalEdges:AXIDMA_SIMPLE)
    '''
    #Gaussian blur to reduce noise
    #gaussian_filter(im, blur)
    #im2 = cv2.blur(im,(3,3))
    #im2 = convolve(im,[[-1/9,-1/9,-1/9],[-1/9,-1/9,-1/9],[-1/9,-1/9,-1/9]]
    thresholdedEdges = np.ndarray((240,320),'uint8')
    finalEdges_buf = np.ndarray((240,320),'uint8')
    filter(im,thresholdedEdges)

    #Tracing edges with hysteresis
    #Find weak edge pixels near strong edge pixels
    currentPixels_r :List['uint8:1000'] = []
    currentPixels_c :List['uint8:1000'] = []
    for r in range(1, im.shape[0]-1):
        for c in range(1, im.shape[1]-1):
            if thresholdedEdges[r][c] == 2:
                finalEdges_buf[r][c] = 255
            else:
                finalEdges_buf[r][c] = 0
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

def run():
    capture = cv2.VideoCapture(0)
    prev = time.time()
    acc_time = 0
    acc_cnt = 0
    while True:
        ret, frame = capture.read()
        if frame is not None:   
            gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY,1)
            gray = cv2.resize(gray,(320,240))
            gray = np.array(gray,dtype='uint8')
            #cv2.imwrite('gray.png',gray)
            finalEdges = np.ndarray(gray.shape,'uint8')
            CannyEdgeDetector(gray,finalEdges)
            result = np.array(finalEdges, dtype='uint8')

            cv2.imshow('Result',result)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                cv2.destroyAllWindows()
                s.close()
                break
        cap = time.time()
        fps = 1.0/(cap-prev)
        acc_time += fps
        acc_cnt += 1
        print('elapsed: %.2f ms   rate: %.1f fps   avg: %.2f fps' % (((cap-prev)*1000),fps,acc_time/acc_cnt))

        prev = cap
 
def single():
    acc_time = 0
    img = cv2.imread('gray.png',0)
    gray = np.ndarray(img.shape,'uint8')
    np.copyto(gray,img)
    finalEdges = np.ndarray(gray.shape,'uint8')
    for i in range(8):
        prev = time.time()
        CannyEdgeDetector(gray,finalEdges)
        cap = time.time()
        fps = 1.0/(cap-prev)
        print('elapsed: %.2f ms   rate: %.1f fps   avg: %.2f fps' % (((cap-prev)*1000),fps,acc_time))

    #cv2.imshow('Result',finalEdges)
    #cv2.waitKey(0)
    #cv2.destroyAllWindows()

if __name__=="__main__":
    im = cv2.imread("sample.png", 0) #Open image, convert to greyscale
    cv2.imshow('img',im)

    finalEdges = np.ndarray(im.shape,'uint8')
    CannyEdgeDetector(im,finalEdges)

    cv2.imshow('result',finalEdges)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

