import cv2 as cv
import numpy as np
import frame_convert2
import freenect

cv.namedWindow('Minimum')
cv.namedWindow('Maximum')

kernel = np.ones((5,5),np.uint8)

def nothing(x):
	pass

ind = 0
print('%s\nPress ESC to stop' % __doc__)


def get_depth(ind):
    return frame_convert2.pretty_depth_cv(freenect.sync_get_depth(ind)[0])


def get_video(ind):
    return frame_convert2.video_cv(freenect.sync_get_video(ind)[0])

cv.createTrackbar('H','Minimum', 0, 255, nothing)
cv.createTrackbar('S','Minimum', 0, 255, nothing)
cv.createTrackbar('V','Minimum', 0, 255, nothing)

cv.createTrackbar('H','Maximum', 0, 255, nothing)
cv.createTrackbar('S','Maximum', 0, 255, nothing)
cv.createTrackbar('V','Maximum', 0, 255, nothing)

hmin = 20
hmax = 38

smin = 42
smax = 190

vmin = 107
vmax = 255

red_lower = np.array([156, 97, 18])
red_upper = np.array([181, 255, 255])

while True:
    print(ind)
    try:
        depth = get_depth(ind)
        frame = get_video(ind)
    except TypeError:
        ind = 0
        continue
    ind += 1

    frame = np.array(frame)
    zero_copy = frame.copy()
    zero_copy[:, :, :] = 0
    ring_zero_copy = zero_copy.copy()
    frame_hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
    
    # hmin = cv.getTrackbarPos('H', 'Minimum')
    # smin = cv.getTrackbarPos('S', 'Minimum')
    # vmin = cv.getTrackbarPos('V', 'Minimum')

    # hmax = cv.getTrackbarPos('H', 'Maximum')
    # smax = cv.getTrackbarPos('S', 'Maximum')
    # vmax = cv.getTrackbarPos('V', 'Maximum')

    mask = cv.inRange(frame_hsv, np.array([hmin, smin, vmin]), np.array([hmax, smax, vmax]))
    mask = cv.morphologyEx(mask, cv.MORPH_OPEN, kernel)

    frame_hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
    red_mask = cv.inRange(frame_hsv, red_lower, red_upper)
    # red_mask = cv.dilate(red_mask, kernel, iterations = 1)

    frame_thresh = cv.bitwise_and(frame, frame, mask = mask)
    ring_thresh = cv.bitwise_and(frame, frame, mask = red_mask)

    contours , _ = cv.findContours(mask.copy().astype(np.uint8), cv.RETR_TREE, cv.CHAIN_APPROX_NONE)
    ring_contours , _ = cv.findContours(red_mask.copy().astype(np.uint8), cv.RETR_TREE, cv.CHAIN_APPROX_NONE)

    for c in contours:
        M = cv.moments(c)
        if(M["m00"] < 1000):
            continue
        cv.drawContours(zero_copy, [c], -1, (0, 255, 0), 2)

    for c in ring_contours:
        M = cv.moments(c)
        if(M["m00"] < 200):
            continue
        cv.drawContours(ring_zero_copy, [c], -1, (0, 255, 0), 2)

    zero_copy = cv.cvtColor(zero_copy, cv.COLOR_BGR2GRAY)
    ring_zero_copy = cv.cvtColor(ring_zero_copy, cv.COLOR_BGR2GRAY)
    # zero_copy = cv.morphologyEx(zero_copy, cv.MORPH_OPEN, kernel)
    # zero_copy = cv.dilate(zero_copy, kernel, iterations = 1)
    ring_zero_copy = cv.dilate(ring_zero_copy, kernel, iterations = 1)
    contours , _ = cv.findContours(zero_copy.copy().astype(np.uint8), cv.RETR_TREE, cv.CHAIN_APPROX_NONE)
    ring_contours , _ = cv.findContours(ring_zero_copy.copy().astype(np.uint8), cv.RETR_TREE, cv.CHAIN_APPROX_NONE)

    for c in contours:
        M = cv.moments(c)
        if(M["m00"] < 5000):
            continue
        cX = int(M["m10"] / M["m00"])
        cY = int(M["m01"] / M["m00"])
        rect = cv.minAreaRect(c)
        if(rect[2] > 6 or rect[2] < -6):
            continue
        if(rect[1][0] / rect[1][1] > 0.4):
            continue
        box = cv.boxPoints(rect)
        box = np.int0(box)
        cv.drawContours(frame, [box], 0, (255, 255, 255), 2)
        cv.circle(frame, (cX, cY), 7, (255, 255, 255), -1)
        cv.putText(frame, "Pole at Depth:" + str(depth[cY, cX]), (cX - 20, cY - 20), cv.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)

    for c in ring_contours:
        M = cv.moments(c)
        if(M["m00"] < 500):
            continue
        cX = int(M["m10"] / M["m00"])
        cY = int(M["m01"] / M["m00"])
        rect = cv.minAreaRect(c)
        if(rect[2] > 6 or rect[2] < -6):
            continue
        if(rect[1][0] / rect[1][1] < 0.6):
            continue
        box = cv.boxPoints(rect)
        box = np.int0(box)
        cv.drawContours(frame, [box], 0, (255, 255, 255), 2)
        cv.circle(frame, (cX, cY), 7, (255, 255, 255), -1)
        cv.putText(frame, "Ring at Distance:" + str(depth[cY, cX]), (cX - 20, cY - 20), cv.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)

    cv.imshow('Pole',frame)
    #cv.imshow('Mask',mask)
    cv.imshow('Thresholded',frame_thresh)
    cv.imshow('zero_copy', zero_copy)
    cv.imshow('ring_thresh', ring_zero_copy)

    k = cv.waitKey(1) & 0xFF
    if k == 27:
        break