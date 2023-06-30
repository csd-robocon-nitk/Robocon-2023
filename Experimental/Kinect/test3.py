import freenect
import cv2
import frame_convert2
import numpy as np
import utils
import math

dist = 0
ind = 0
print('%s\nPress ESC to stop' % __doc__)


def get_depth(ind):
    cdist = freenect.sync_get_depth(ind)[0][240, 320]
    return frame_convert2.pretty_depth_cv(freenect.sync_get_depth(ind)[0]), cdist


def get_video(ind):
    return frame_convert2.video_cv(freenect.sync_get_video(ind)[0])

while 1:
    print(ind)
    try:
        depth, wdist = get_depth(ind)
        video = get_video(ind)
    except TypeError:
        ind = 0
        continue
    ind += 1

    img = depth
    img2 = video
    img2[235:245, 315:325] = [0, 0, 255]
    wdepth = img[240, 320]
    th1 = 255 * np.logical_and(depth >= wdepth - 5, depth <= wdepth + 5)
    th1 = th1.astype(np.uint8)
    cpy = th1
    th1 = cv2.cvtColor(th1, cv2.COLOR_GRAY2BGR)
    th1[:, :, 0] = 0
    th1[:, :, 1] = 0
    th1[400:480, :] = 0
    th1[:, 0:200] = 0
    th1[:, 440:640] = 0
    edges = cv2.Canny(image=th1, threshold1=150, threshold2=200)
    lines = cv2.HoughLines(edges, 1, np.pi/180, 90, 0, 0, 0)
    linesImage = np.zeros_like(th1)
    if lines is not None:
        for i in range(0, len(lines)):
            rho = lines[i][0][0]
            theta = lines[i][0][1]
            a = math.cos(theta)
            b = math.sin(theta)
            x0 = a * rho
            y0 = b * rho
            pt1 = (int(x0 + 1000*(-b)), int(y0 + 1000*(a)))
            pt2 = (int(x0 - 1000*(-b)), int(y0 - 1000*(a)))
            cv2.line(linesImage, pt1, pt2, (0,0,255), 3, cv2.LINE_AA)
    #ret,th1 = cv2.threshold(img, wdepth + 3 ,255,cv2.THRESH_BINARY)
    cv2.imshow('bruh', linesImage)
    cv2.imshow('bruh2', th1)
    cv2.imshow('Depth', img2)
    #cv2.imshow('Video', video)
    print("dist :",wdist)
    if cv2.waitKey(10) == 27:
        break