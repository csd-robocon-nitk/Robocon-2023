import freenect
import cv2
import frame_convert2
import numpy as np

ind = 0
print('%s\nPress ESC to stop' % __doc__)


def get_depth(ind):
    return frame_convert2.pretty_depth_cv(freenect.sync_get_depth(ind)[0])


def get_video(ind):
    return frame_convert2.video_cv(freenect.sync_get_video(ind)[0])


while 1:
    print(ind)
    try:
        depth = get_depth(ind)
        video = get_video(ind)
    except TypeError:
        ind = 0
        continue
    ind += 1

    img = depth
    img2 = video
    img2 = cv2.cvtColor(img2, cv2.COLOR_RGB2BGR)
    kernel = np.ones((5,5),np.uint8)
    img_blur = cv2.GaussianBlur(img, (3,3), 0)
    edges = cv2.Canny(image=img_blur, threshold1=80, threshold2=200)
    edges = cv2.dilate(edges,kernel,iterations = 1)
    ret, thresh = cv2.threshold(edges, 100, 255, 0)
    contours, heirarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    img2 = cv2.cvtColor(img2, cv2.COLOR_RGB2BGR)
    cv2.drawContours(img2, contours, -1, (0,255,0), 3)
    cv2.imshow('Depth', img2)
    #cv2.imshow('Video', video)
    if cv2.waitKey(10) == 27:
        break