import numpy as np
import socket


class Message:
    CAMERAS = 1
    MARKERS = 2


def recvall(sock, size):
    data = []
    total = 0
    
    while total < size:
        r = sock.recv(size - total)
        if not r:
            return
        data.append(r)
        total += len(r)
    
    return np.fromstring(''.join(data), dtype=np.uint8)
        
def readbytes(s, n):
    return (s[:n], s[n:])


def getMessage(sock):
    size = recvall(sock, 4)
    size = np.frombuffer(size, dtype=np.uint32, count=1)
    
    data = recvall(sock, size)
    header, data = readbytes(data, 1)
    header = int(header)
    
    ret = dict()
    if header == Message.CAMERAS:
        ret["cameras"] = readCameraData(data)
    elif header == Message.MARKERS:
        ret["markers"] = readMarkerData(data)
    else:
        print "Unrecognized header", header
    
    return ret


def readCameraData(data):
    numCams, data = readbytes(data, 1)
    numCams = int(numCams)

    cams = []
    for c in range(numCams):
        cam = dict()
        
        d, data = readbytes(data, 2*8)
        cam["size"] = np.frombuffer(d, dtype=np.double, count=2)
                
        cam["K"] = np.zeros((3,3), dtype=np.double)
        for i in range(3):
            d, data = readbytes(data, 3*8)
            cam["K"][i] = np.frombuffer(d, dtype=np.double, count=3)
            
        cam["t"] = np.zeros((1,3), dtype=np.double)
        d, data = readbytes(data, 3*8)
        cam["t"] = np.frombuffer(d, dtype=np.double, count=3)
            
        cam["r"] = np.zeros((3,3), dtype=np.double)
        for i in range(3):
            d, data = readbytes(data, 3*8)
            cam["r"][i] = np.frombuffer(d, dtype=np.double, count=3)
        
        cams.append(cam)
    
    return cams
        

def readMarkerData(data):
    numMarkers, data = readbytes(data, 1)
    numMarkers = int(numMarkers)

    markers = []
    for m in range(numMarkers):
        mark = dict()
        
        d, data = readbytes(data, 3*8)
        mark["mark"] = np.frombuffer(d, dtype=np.double, count=3)
        
        numRays, data = readbytes(data, 1)
        numRays = int(numRays)
        
        rays = []
        for i in range(numRays):
            d, data = readbytes(data, 3*8)
            o = np.frombuffer(d, dtype=np.double, count=3)
            
            d, data = readbytes(data, 3*8)
            r = np.frombuffer(d, dtype=np.double, count=3)
            
            rays.append((o,r))
        
        mark["rays"] = rays
        
        markers.append(mark)
    
    return markers
