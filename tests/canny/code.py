#!/Bin/python
import numpy as np
from scipy.ndimage.filters import convolve, gaussian_filter
import cv2
from matplotlib.pyplot import imshow
import time
import socket

def supress(im3h:np.ndarray((720,1280),dtype='float32'), im3v:np.ndarray((720,1280),dtype='float32'), finalEdges:np.ndarray((720,1280),dtype='uint8')):
    highThreshold = 91
    lowThreshold = 31

    grad = np.ndarray(shape=(720,1280), dtype='uint8')
    tq = np.ndarray(shape=(720,1280), dtype='uint8')
    #get gradient and direction
    for r in range(grad.shape[0]):
        for c in range(grad.shape[1]):
            t1:'float32' = im3h[r][c]
            t2:'float32' = im3v[r][c]
            val = np.power(np.power(t1,2.0) + np.power(t2,2.0), 0.5)
            grad[r][c] = np.round(val)

            theta = np.arctan2(t2,t1)
            tmp:'uint8' = (np.round(theta * (5.0 / np.pi)) + 5)
            thetaQ = tmp % 5
            tq[r][c] = thetaQ % 4


    strongEdges = np.ndarray(shape=(3,1280), dtype='uint8')
    thresholdedEdges = np.ndarray(shape=(3,1280), dtype='uint8')
    grad_buf = np.ndarray(shape=(3,1280), dtype='uint8')
    for r in range(grad.shape[0]+5):
        if r < 3:
            for c in range(grad.shape[1]):
                grad_buf[r][c] = grad[r][c]
        if r > 2:
            if r < grad.shape[0]:
                for c in range(grad.shape[1]):
                    grad_buf[r%3][c] = grad[r][c]
            for c in range(grad.shape[1]):
                if r < grad.shape[0]:
                    #Suppress pixels at the image edge
                    gradSup = 0
                    if r == 0 or r == grad.shape[0]-1 or c == 0 or c == grad.shape[1] - 1:
                        strongEdges[r%3][c]=0
                        thresholdedEdges[r%3][c] = 0
                    else:    
                        tq_buf = tq[r][c]
                        if tq_buf == 0: #0 is E-W (horizontal)
                            if grad_buf[r%3][c] < grad_buf[r%3][c-1] or grad_buf[r%3][c] < grad_buf[r%3][c+1]:
                                gradSup = 0
                            else:
                                gradSup = grad_buf[r%3][c]
                        elif tq_buf == 1: #1 is NE-SW
                            if grad_buf[r%3][c] < grad_buf[(r-1)%3][c+1] or grad_buf[r%3][c] < grad_buf[(r+1)%3][c-1]:
                                gradSup = 0
                            else:
                                gradSup = grad_buf[r%3][c]
                        elif tq_buf == 2: #2 is N-S (vertical)
                            if grad_buf[r%3][c] < grad_buf[(r-1)%3][c] or grad_buf[r%3][c] < grad_buf[(r+1)%3][c]:
                                gradSup = 0
                            else:
                                gradSup = grad_buf[r%3][c]
                        elif tq_buf == 3: #3 is NW-SE
                            if grad_buf[r%3][c] < grad_buf[(r-1)%3][c-1] or grad_buf[r%3][c] < grad_buf[(r+1)%3][c+1]:
                                gradSup = 0
                            else:
                                gradSup = grad_buf[r%3][c]
                        else:
                            gradSup = grad_buf[r%3][c]
                    
                        if gradSup > highThreshold:
                            strongEdges[r%3][c]=1
                        else:
                            strongEdges[r%3][c]=0
                        thresholdedEdges[r%3][c] = strongEdges[r%3][c] + (gradSup > lowThreshold)

        if r > 4:
            #Tracing edges with hysteresis
            #Find Weak edge pixels near strong edge pixels
            for c in range(grad.shape[1]):
                final_buf = strongEdges[(r-5)%3][c]
                if r-5 > 0 and r-5 < grad.shape[0]-1 and c > 0 and c < grad.shape[1]-1 and thresholdedEdges[(r-5)%3][c] == 1:
                    #Get 3x3 patch
                    patchMax = -1
                    for rp in range(-1,2):
                        for cp in range(-1,2):
                            if patchMax < thresholdedEdges[(r-5+rp)%3][c+cp]:
                                patchMax = thresholdedEdges[(r-5+rp)%3][c+cp]
                    if patchMax == 2:
                        final_buf = 1
                finalEdges[(r-5)][c] = final_buf


def CannyEdgeDetector(im, blur = 1, highThreshold = 91, lowThreshold = 31):
    im = np.array(im, dtype='float32') #Convert to float to prevent clipping values

    #Gaussian blur to reduce noise
    im2 = cv2.blur(im,(3,3))


    #Use sobel filters to get horizontal and vertical gradients
    im3h = convolve(im2,[[-1,0,1],[-2,0,2],[-1,0,1]])
    im3v = convolve(im2,[[1,2,1],[0,0,0],[-1,-2,-1]])
            
    finalEdges = np.ndarray((720,1280),dtype='uint8')
    start = time.time()
    supress(im3h,im3v,finalEdges)
    stop = time.time()
    elapsed = (stop -start) * 1000
    print('elapsed: %d ms' % elapsed)
    return 255-(finalEdges*255)

def CannyEdgeDetector2(im):
    highThreshold = 91
    lowThreshold = 31
    im = np.array(im, dtype=float) #Convert to float to prevent clipping values
 
    #Gaussian blur to reduce noise
    im2 = cv2.blur(im,(3,3))
    print('blur')
    #Use sobel filters to get horizontal and vertical gradients
    im3h = convolve(im2,[[-1,0,1],[-2,0,2],[-1,0,1]]) 
    im3v = convolve(im2,[[1,2,1],[0,0,0],[-1,-2,-1]])
    print('convolve')
    #Get gradient and direction
    grad = np.power(np.power(im3h, 2.0) + np.power(im3v, 2.0), 0.5)
    theta = np.arctan2(im3v, im3h)
    thetaQ = (np.round(theta * (5.0 / np.pi)) + 5) % 5 #Quantize direction
    print('grad')
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

    print('while')
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
 
    return 255-(finalEdges*255)

def recvall(sock, count):
    buf = b''
    while count:
        newbuf = sock.recv(count)
        if not newbuf: return None
        buf += newbuf
        count -= len(newbuf)
    return buf

def run():
    TCP_IP = '0.0.0.0'
    TCP_PORT = 11236

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((TCP_IP, TCP_PORT))
    s.listen(True)
    print('server ready')
    conn, addr = s.accept()
    while True:
        #receive image
        length = recvall(conn,16)
        if length is not None:
            stringData = recvall(conn, int(length))
            data = np.fromstring(stringData, dtype='uint8')
            decimg=cv2.imdecode(data,cv2.IMREAD_GRAYSCALE)

        #compute 
            finalEdges = CannyEdgeDetector2(decimg)
            result = np.array(finalEdges, dtype='uint8')
            encode_param=[int(cv2.IMWRITE_JPEG_QUALITY),90]
            result, imgencode = cv2.imencode('.jpg', result, encode_param)        
        #cv2.imshow('SERVER',result)
        #if cv2.waitKey(1) & 0xFF == ord('q'):
        #    cv2.destroyAllWindows()
        #    s.close()
        #    break

        #send result back
            data = np.array(imgencode)
            stringData = data.tostring()
            conn.send( str.encode(str(len(stringData)).ljust(16)));
            conn.send(stringData)


if __name__=="__main__":
    run()

def old():
    im = cv2.imread("valve.jpg", cv2.IMREAD_GRAYSCALE) #Open image, convert to greyscale
    cv2.imshow('input',im)

    finalEdges = CannyEdgeDetector(im)
    result = np.array(finalEdges, dtype='uint8') #Convert to float to prevent clipping values
    cv2.imshow('result',result)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
        
