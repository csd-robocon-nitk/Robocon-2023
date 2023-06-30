import freenect
import cv2
import frame_convert2
import tensorflow_hub as hub
import numpy
import pandas as pd
import tensorflow as tf
import matplotlib.pyplot as plt

cv2.namedWindow('Depth')
cv2.namedWindow('Video')
ind = 0
print('%s\nPress ESC to stop' % __doc__)


def get_depth(ind):
    return frame_convert2.pretty_depth_cv(freenect.sync_get_depth(ind)[0])


def get_video(ind):
    return frame_convert2.video_cv(freenect.sync_get_video(ind)[0])

detector = hub.load("https://tfhub.dev/tensorflow/efficientdet/lite2/detection/1")
labels = pd.read_csv('labels.csv', sep=';', index_col='ID')
labels = labels['OBJECT (2017 REL.)']

while 1:
    print(ind)
    try:
        depth = get_depth(ind)
        video = get_video(ind)
    except TypeError:
        ind = 0
        continue
    ind += 1
    width = 1028
    height = 1028
    rgb = cv2.cvtColor(video, cv2.COLOR_BGR2RGB)
    rgb_tensor = tf.convert_to_tensor(rgb, dtype=tf.uint8)
    rgb_tensor = tf.expand_dims(rgb_tensor , 0)
    boxes, scores, classes, num_detections = detector(rgb_tensor)
    pred_labels = classes.numpy().astype('int')[0]
    
    pred_labels = [labels[i] for i in pred_labels]
    pred_boxes = boxes.numpy()[0].astype('int')
    pred_scores = scores.numpy()[0]

    for score, (ymin,xmin,ymax,xmax), label in zip(pred_scores, pred_boxes, pred_labels):
        if score < 0.5:
            continue
            
        score_txt = f'{100 * round(score,0)}'
        img_boxes = cv2.rectangle(rgb,(xmin, ymax),(xmax, ymin),(0,255,0),1)      
        font = cv2.FONT_HERSHEY_SIMPLEX
        cv2.putText(img_boxes,label,(xmin, ymax-10), font, 0.5, (255,0,0), 1, cv2.LINE_AA)
        cv2.putText(img_boxes,score_txt,(xmax, ymax-10), font, 0.5, (255,0,0), 1, cv2.LINE_AA)

    cv2.imshow('black and white',img_boxes)
    cv2.imshow('Depth', depth)
    cv2.imshow('Video', video)
    if cv2.waitKey(10) == 27:
        break