## Graphics and miscellaneous helpers
from __future__ import division

import math
import numpy
from pyglet.gl import *


# Signed square root
def ssqrt(x):
    if x == 0:
        return 0
    
    return (x / abs(x)) * math.sqrt(abs(x))

def triVerts(x, y, z, sz, rot):
    return (x + sz/2 * math.cos(rot),                 y + sz/2 * math.sin(rot),                 z,
            x + sz/2 * math.cos(rot + 2.1*math.pi/3), y + sz/2 * math.sin(rot + 2.1*math.pi/3), z,
            x + sz/2 * math.cos(rot - 2.1*math.pi/3), y + sz/2 * math.sin(rot - 2.1*math.pi/3), z)

    
def gridLines(origin, size, step, color):
    verts = []
    colors = []
    
    x = origin[0]
    while x <= origin[0] + size[0]:
        v = [x,         origin[1], origin[2],\
             x, origin[1]+size[1], origin[2]]
        #verts += [a+b for a,b in zip(v, origin*2)]
        verts += v
        colors += color*2
        
        x += step[0]

    y = origin[1]
    while y <= origin[1] + size[1]:
        v = [        origin[0], y, origin[2],\
             origin[1]+size[0], y, origin[2]]
        #verts += [a+b for a,b in zip(v, origin*2)]
        verts += v
        colors += color*2
        
        y += step[1]

    return (verts, colors)

def drawAxes(origin, size):
    verts = []
    colors = []
    
    verts += [0, 0, 0, size, 0, 0]
    colors += [0, 1, 0, 1]*2
    
    verts += [0, 0, 0, 0, size, 0]
    colors += [0, 0, 1, 1]*2
    
    verts += [0, 0, 0, 0, 0, size]
    colors += [1, 0, 0, 1]*2
    
    return (verts, colors)

def drawCameraCone(cam, cone_color, area_color):
    verts  = []
    colors = []
    
    center = numpy.array([1024/2, 768/2, 1])
    origin = numpy.array([0, 0, 0])
    normal = numpy.array([0, 0, 1])
    
    K = cam['K']
    r = cam['r']
    t = cam['t']

    K_i = numpy.linalg.inv(K)
    r_i = numpy.linalg.inv(r)
    
    pos = -r_i.dot(t)
    
    corner_verts  = []
    corner_colors = []
    for c in [numpy.array([[x], [y], [1]]) for x,y in [(0,0), (0,768), (1024,768), (1024,0)]]:
        rot = r_i.dot(K_i).dot(c)
        rot = rot / numpy.linalg.norm(rot)
        
        mag = numpy.dot(origin - pos, normal) / numpy.dot(rot.T, normal)
        
        c1 = [float(pos[0]),              float(pos[1]),              float(pos[2])]
        c2 = [float(pos[0] + mag*rot[0]), float(pos[1] + mag*rot[1]), float(pos[2] + mag*rot[2])]
    
        verts  += [c1[0], c1[1], c1[2], \
                   c2[0], c2[1], c2[2]]
        colors += cone_color*2
        
        corner_verts  += [c2[0], c2[1], c2[2], \
                          c2[0], c2[1], c2[2]]
        corner_colors += area_color*2
    
    corner_verts[0:0]  = corner_verts[-3:]
    corner_verts[-3:]  = []
    corner_colors[0:0] = corner_colors[-4:]
    corner_colors[-4:] = []
    
    verts  += corner_verts
    colors += corner_colors
        
    return (verts, colors)

def loadCameraModelview(cam):
    mvm = numpy.copy(cam['r'].transpose())
    mvm = numpy.append(mvm, [cam['t'].T], 0)
    mvm = numpy.append(mvm, [[0], [0], [0], [1]], 1)
    
    mvm[:,(1,2)] *= -1
    mvm = mvm.ravel().astype(GLfloat)
     
    return mvm

def loadCameraProjection(cam, near, far):
    fx = cam['K'][0,0]
    fy = cam['K'][1,1]
    px = cam['K'][0,2]
    py = cam['K'][1,2]
    w  = cam['size'][0]
    h  = cam['size'][1]
    
    prj = numpy.array([[2.0 * fx / w, 0.0, 0.0, 0.0],
                       [0.0, 2.0 * fy / h, 0.0, 0.0],
                       [2.0 * (px / w) - 1.0, 2.0 * (py / h) - 1.0, -1.0 * (far + near) / (far - near), -1.0],
                       [0.0, 0.0, -2.0 * far * near / (far - near), 0.0]])
    
    return prj.ravel().astype(GLfloat)

def calc_rot(forward, up):
    rot = numpy.zeros((3,3))
    
    forward = forward / numpy.linalg.norm(forward)
    up      = up / numpy.linalg.norm(up)
    
    rot[:,0] = forward[:,0]
    
    a = numpy.cross(forward.T, up.T)
    rot[:,1] = a / numpy.linalg.norm(a)
    
    b = numpy.cross(rot[:,1], rot[:,0])
    rot[:,2] = b / numpy.linalg.norm(b)
    
    return rot

