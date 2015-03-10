#!/usr/bin/python
from __future__ import division

## Project modules
from comms import *
from gfx import *
from Camera import Camera

import socket, select

import numpy

import pyglet
from pyglet.window import key
from pyglet.gl import *

## General modules
import sys
import time
import math
from pprint import pprint as pp



## Override pyglet's default idle method to avoid drawing on every event
def idle(self):
    pyglet.clock.tick(poll = True)
    return pyglet.clock.get_sleep_time(sleep_idle = True)
pyglet.app.EventLoop.idle = idle


## Main LivePlot class --------------------------------------------------------
class LivePlot(pyglet.window.Window):
    ## Setup Functions --------------------------------------------------------
    def __init__(self, server, *args, **kwargs):
        super(LivePlot, self).__init__(*args, **kwargs)
        
        self.keyboard = key.KeyStateHandler()
        self.push_handlers(self.keyboard)
        
        self.setup_context()
        self.setup_scene()

        self.frequency = 60.5
        pyglet.clock.schedule_interval(self.step, 1/self.frequency)
        
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect(server)

    def setup_context(self):
        glClearColor(0, 0, 0, 1)

        glEnable(GL_DEPTH_TEST)
        glDepthMask(GL_TRUE)
        
        glDisable(GL_CULL_FACE)

        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

        #glEnable(GL_MULTISAMPLE_ARB)
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
        
        glEnable(GL_POLYGON_SMOOTH)
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST)
        glEnable(GL_POINT_SMOOTH)
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST)
        
        #glEnable(GL_LINE_SMOOTH)
        #glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)

    def setup_scene(self):
        self.fps = []
        self.fps_display = pyglet.text.Label(
                '',
                font_name = 'Courier', font_size = 16, bold = True,
                color = (255, 255, 255, 100),
                x = 5, y = 0,
                anchor_x = 'left', anchor_y = 'bottom'
        )
        
        self.camera_mode_display = pyglet.text.Label(
                '',
                font_name = 'Courier', font_size = 16, bold = True,
                color = (255, 255, 255, 200),
                x = 0, y = 0,
                anchor_x = 'right', anchor_y = 'bottom'
        )

        self.camera = Camera(50, [[1],[1],[3000]])
        self.set_exclusive_mouse(True)
        self.camera.look_at(np.array([[0],[0],[0]]))

        self.cameras = []
        self.markers = []
        
        self.cur_view = -1

        self.batch        = pyglet.graphics.Batch()
        self.background   = pyglet.graphics.OrderedGroup(0)
        self.foreground   = pyglet.graphics.OrderedGroup(1)
        
        self.floor_grid   = self.batch.add(1, pyglet.gl.GL_LINES,  self.background, 'v3f', 'c4f')
        self.axes         = self.batch.add(1, pyglet.gl.GL_LINES,  self.foreground, 'v3f', 'c4f')
        self.camera_cones = self.batch.add(1, pyglet.gl.GL_LINES,  self.foreground, 'v3f', 'c4f')
        self.markers      = self.batch.add(1, pyglet.gl.GL_POINTS, self.foreground, 'v3f', 'c4f')

        floor_grid = gridLines((-80000, -80000, 0), (160000, 160000, 0), (1000, 1000), [0.15, 0.15, 0.15, 1])
        self.floor_grid.resize(len(floor_grid[0]) // 3)
        self.floor_grid.vertices = floor_grid[0]
        self.floor_grid.colors   = floor_grid[1]
        
        axes = drawAxes((0, 0, 0), 1000)
        self.axes.resize(len(axes[0]) // 3)
        self.axes.vertices = axes[0]
        self.axes.colors   = axes[1]


    ## Step function, called every tick ---------------------------------------
    def step(self, dt):
        self.fps.append(1/dt)
        self.fps = self.fps[int(-self.frequency):]
        self.fps_display.text = '{:.2f}'.format(sum(self.fps)/len(self.fps))
        
        if self.camera.mode == Camera.FREE:
            self.camera_mode_display.text = "FREE"
        elif self.camera.mode == Camera.FOLLOW:
            self.camera_mode_display.text = "FOLLOW"
        elif self.camera.mode == Camera.XY:
            self.camera_mode_display.text = "XY"
        elif self.camera.mode == Camera.FIXED:
            self.camera_mode_display.text = "FIXED"
        
        ready,_,_ = select.select([self.sock], [], [], 0)
        #ready = False
        if ready:
            msg = getMessage(ready[0])

            for t,d in msg.iteritems():
                if t == "cameras":
                    self.cameras = d
                    self.camera.update_views(self.cameras)
                    
                    cone_color_cycle = [[1.0, 0.3, 0.3, 0.3], \
                                        [0.3, 1.0, 0.3, 0.3], \
                                        [0.3, 0.3, 1.0, 0.3]]
                    area_color_cycle = [[1.0, 0.3, 0.3, 0.5], \
                                        [0.3, 1.0, 0.3, 0.5], \
                                        [0.3, 0.3, 1.0, 0.5]]
                    
                    verts  = []
                    colors = []
                    for cam in d:
                        v,c = drawCameraCone(cam, cone_color_cycle[0], area_color_cycle[0])
                        verts  += v
                        colors += c
                        
                        cone_color_cycle = cone_color_cycle[1:] + cone_color_cycle[:1]
                        area_color_cycle = area_color_cycle[1:] + area_color_cycle[:1]
                    
                    self.camera_cones.resize(len(verts) // 3)
                    self.camera_cones.vertices = verts
                    self.camera_cones.colors   = colors
                        
                elif t == "markers":
                    markers = []
                    for m in d:
                        markers += list(m['mark'])
                        
                    self.markers.resize(len(d))
                    self.markers.vertices = markers
                    self.markers.colors   = [0, 0, 1, 1] * len(d)
        
        
        vc = np.array([[6000 * dt], [6000 * dt], [6000 * dt]])
        vc *= np.array([[self.keyboard[key.W]      - self.keyboard[key.S]], \
                        [self.keyboard[key.D]      - self.keyboard[key.A]], \
                        [self.keyboard[key.LSHIFT] - self.keyboard[key.LCTRL]]])
        
        self.camera.translate(vc)

            
        # Hack for pyglet bug
        try:
            self.switch_to()
            self.dispatch_event('on_draw')
            self.flip()
        except AttributeError:
            pass


    ## Rendering --------------------------------------------------------------
    def render(self):
        w,h = self.get_size()
        self.camera.prerender(w, h)
        
        rot = time.time() * 0.5
        
        glLineWidth(1)
        glPointSize(10)
        glDisable(GL_DEPTH_TEST)
        self.batch.draw()
        
        self.camera.postrender()
        
    def render_2d(self):
        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        glLoadIdentity()
        
        w,h = self.get_size()
        glOrtho(0, w, 0, h, -1.0, 2.0)
        
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        glLoadIdentity()
        
        ## Draw Text
        glDisable(GL_DEPTH_TEST)
        self.fps_display.draw()
        self.camera_mode_display.draw()
        glEnable(GL_DEPTH_TEST)
        
        glPopMatrix()
        
        glMatrixMode(GL_PROJECTION)
        glPopMatrix()


    ## Events -----------------------------------------------------------------
    ## Input events
    def on_key_press(self, symbol, modifiers):
        pass
    
    def on_key_release(self, symbol, modifiers):
        if symbol == key.ESCAPE:
            self.close()
        elif symbol == key.F:
            if not self.fullscreen:
                pos = self.get_location()
                screens = pyglet.window.get_platform().get_default_display().get_screens()
                
                for s in screens:
                    if pos[0] >= s.x and \
                       pos[0] < s.x + s.width and \
                       pos[1] >= s.y and \
                       pos[1] < s.y + s.height:
                           screen = s
                           break
                    
                self.set_fullscreen(True, screen)
            else:
                self.set_fullscreen(False)
        elif symbol == key._1:
            self.camera.set_mode(Camera.FREE)
            self.set_exclusive_mouse(True)
        elif symbol == key._2:
            self.camera.set_mode(Camera.FOLLOW)
            self.set_exclusive_mouse(False)
        elif symbol == key._3:
            self.camera.set_mode(Camera.XY)
            self.set_exclusive_mouse(False)
        elif symbol == key._4:
            self.camera.set_mode(Camera.FIXED)
            self.set_exclusive_mouse(False)
        elif symbol == key.SPACE:
            self.camera.next_view()
    
    def on_mouse_motion(self, x, y, dx, dy):
        self.camera.rotate(-dx/700, dy/700)
    
    
    ## Window events
    def on_resize(self, width, height):
        glViewport(0, 0, width, height)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        
        fov = 50
        aspect = width / height

        if width < height:
            fov = 360/math.pi * math.atan(math.tan(fov * math.pi/360) / aspect)
                    
        gluPerspective(fov, aspect, 0.1, 200000)
        
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        
        # Update text labels
        self.camera_mode_display.x = width - 5

        return pyglet.event.EVENT_HANDLED

    def on_draw(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        
        self.render()
        self.render_2d()


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "LivePlot.py <server ip> <server port>"
        sys.exit(1)
    
    server = (sys.argv[1], int(sys.argv[2]))
    

    screen = pyglet.window.get_platform().get_default_display().get_default_screen()
    template = pyglet.gl.Config(depth_size=24, double_buffer=True, sample_buffers=1, samples=4)
    
    try:
        config = screen.get_best_config(template)
    except pyglet.window.NoSuchConfigException:
        template = pyglet.gl.Config()
        config = screen.get_best_config(template)
        
    window = LivePlot(server, width=1024, height=768, resizable=True, config=config, vsync=False)
    
    pyglet.app.run()
