from __future__ import division

from gfx import *

import numpy as np

from pyglet.gl import *

from transformations import *


class Camera:
    FREE   = 0
    FOLLOW = 1
    XY     = 2
    FIXED  = 3
    
    def __init__(self, fov, position):
        self.mode        = Camera.FREE
        self.fov         = fov
        
        self.position    = np.array(position)
        
        self.up          = np.array([[0],[0],[1]])
        self.forward     = np.array([[1],[0],[-1]])
        self.up          = self.up / np.linalg.norm(self.up)
        self.forward     = self.forward / np.linalg.norm(self.forward)
        
        
        # Fixed mode variables
        self.cur_cam     = -1
        self.views       = []
        self.modelview   = []
        self.projection  = []
        
    
    def set_mode(self, mode):
        if mode == Camera.FIXED and \
           len(self.views) > 0:
               self.mode = Camera.FIXED
               
               if self.cur_view >= len(self.views):
                   self.cur_view = 0
                   
               self.load_view()
        else:
            self.mode = mode
    
    # Free mode
    def rotate(self, dy, dr):
        if self.mode == Camera.FREE:
            r1 = rotation_matrix(dy, self.up)
            r1 = np.delete(r1, 3, 0)
            r1 = np.delete(r1, 3, 1)
            self.forward = np.dot(r1, self.forward)
            
            r2 = rotation_matrix(dr, calc_rot(self.forward, self.up)[:,1])
            r2 = np.delete(r2, 3, 0)
            r2 = np.delete(r2, 3, 1)
            self.forward = np.dot(r2, self.forward)
    
    # Free or follow mode
    def translate(self, t):
        if self.mode == Camera.FREE:
            self.position += np.dot(calc_rot(self.forward, self.up), t)
            
        elif self.mode == Camera.FOLLOW:
            self.forward = self.target - self.position
            self.forward = self.forward / np.linalg.norm(self.forward)
            self.position += np.dot(calc_rot(self.forward, self.up), t)
        
        elif self.mode == Camera.XY:
            self.position[0] += t[1]
            self.position[1] += t[0]
            self.position[2] += t[2]
    
    # Follow mode
    def look_at(self, target):
        self.target = target
    
    # Fixed mode
    def update_views(self, views):
        self.views = views
        
        if len(self.views) > 0:
            self.cur_view = 0
        else:
            self.mode = self.FREE
            self.modelview = []
            
    def load_view(self):
            self.modelview  = loadCameraModelview(self.views[self.cur_view])
            self.projection = loadCameraProjection(self.views[self.cur_view], 0.1, 200000.0)
            
    def next_view(self):
        if len(self.views) > 0:
            self.cur_view = (self.cur_view + 1) % len(self.views)
                
        self.load_view()
        
    
    def prerender(self, width, height):
        if self.mode == Camera.FREE:
            cen = self.position + self.forward
            
            glMatrixMode(GL_MODELVIEW)
            glLoadIdentity()
            gluLookAt(self.position[0], self.position[1], self.position[2], cen[0], cen[1], cen[2], self.up[0], self.up[1], self.up[2])
            
        elif self.mode == Camera.FOLLOW:
            glMatrixMode(GL_MODELVIEW)
            glLoadIdentity()
            gluLookAt(self.position[0], self.position[1], self.position[2], self.target[0], self.target[1], self.target[2], self.up[0], self.up[1], self.up[2])
            
        elif self.mode == Camera.XY:
            glMatrixMode(GL_PROJECTION)
            glPushMatrix()
            glLoadIdentity()
            
            zoom = self.position[2] / 800
            glOrtho(-width/2 * zoom, width/2 * zoom, -height/2 * zoom, height/2 * zoom, -20000, 40000)
            
            glMatrixMode(GL_MODELVIEW)
            glLoadIdentity()
            glTranslatef(-self.position[0], -self.position[1], 0)
            
        elif self.mode == Camera.FIXED:
            c_float_p = POINTER(GLfloat)
            
            if len(self.projection) == 16:
                prj_p = self.projection.ctypes.data_as(c_float_p)
                
                glMatrixMode(GL_PROJECTION)
                glPushMatrix()
                glLoadIdentity()
                glLoadMatrixf(prj_p)
                
            if len(self.modelview) == 16:
                mvm_p = self.modelview.ctypes.data_as(c_float_p)
                
                glMatrixMode(GL_MODELVIEW)
                glLoadIdentity()
                glLoadMatrixf(mvm_p)
                
    def postrender(self):
        if self.mode == Camera.FIXED or self.mode == Camera.XY:
            glMatrixMode(GL_PROJECTION)
            glPopMatrix()
                
