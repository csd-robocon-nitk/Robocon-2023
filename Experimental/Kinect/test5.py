import freenect
import cv2
import frame_convert2
import numpy as np

cv2.namedWindow('Depth')
cv2.namedWindow('Video')
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
        frame = get_video(ind)
    except TypeError:
        ind = 0
        continue
    ind += 1

    b = frame[:, :, :1]

    g = frame[:, :, 1:2]

    r = frame[:, :, 2:]
 

    # computing the mean

    b_mean = np.mean(b)

    g_mean = np.mean(g)

    r_mean = np.mean(r)
 

    # displaying the most prominent color

    if ((b_mean < 40) and (g_mean > 240) and (r_mean > 240)) :
        print("Yellow")
    else:
        print("Not Yellow")

    # video = cv2.cvtColor(video, cv2.COLOR_BGR2HSV)
    # frame_threshold = cv2.inRange(video, (55, 0, 0), (65, 255, 255))

    cv2.imshow('Depth', depth)
    cv2.imshow('Video', frame)
    if cv2.waitKey(10) == 27:
        break