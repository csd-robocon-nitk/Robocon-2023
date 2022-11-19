#!/usr/bin/env python
import freenect
import cv2
import frame_convert2
import numpy as np

keep_running = True


def display_depth(dev, data, timestamp):
    kernel = np.ones((5,5),np.uint8)
    global keep_running
    img = frame_convert2.pretty_depth_cv(data)
    img_blur = cv2.GaussianBlur(img, (3,3), 0)
    edges = cv2.Canny(image=img_blur, threshold1=80, threshold2=200)
    edges = cv2.dilate(edges,kernel,iterations = 1)
    ret, thresh = cv2.threshold(edges, 100, 255, 0)
    contours, heirarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    cv2.drawContours(img, contours, -1, (0,255,0), 3)
    #img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
    cv2.imshow('Depth', img)
    if cv2.waitKey(10) == 27:
        keep_running = False


def display_rgb(dev, data, timestamp):
    global keep_running
    kernel = np.ones((5,5),np.uint8)
    img = frame_convert2.video_cv(data)
    img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
    img = cv2.GaussianBlur(img, (3,3), 0)
    imgray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    #imgray = img[0,0,:]
    edges = cv2.Canny(img, 100, 255)
    edges = cv2.dilate(edges,kernel,iterations = 1)
    ret, thresh = cv2.threshold(edges, 100, 255, 0)
    contours, heirarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    cv2.drawContours(img, contours, -1, (0,255,0), 3)
    img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR);
    cv2.imshow('iontour', img)
    cv2.imshow('Canny', edges)
    if cv2.waitKey(10) == 27:
        keep_running = False


def body(*args):
    if not keep_running:
        raise freenect.Kill


print('Press ESC in window to stop')
freenect.runloop(depth=display_depth,
                 video=display_rgb,
                 body=body)