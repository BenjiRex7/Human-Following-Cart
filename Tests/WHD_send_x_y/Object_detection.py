import cv2
import urllib.request
import numpy as np
import requests

# esp32_ip = '192.168.110.0'
# esp32_ip_2 = '192.168.110.10'

# esp32_ip_2 = '192.168.100.22'
esp32_ip_2 = '192.168.232.55'

url = 'http://192.168.232.27/cam-hi.jpg'
winName = 'ESP32 CAMERA'
cv2.namedWindow(winName, cv2.WINDOW_AUTOSIZE)

classNames = []
classFile = 'Things.names'
with open(classFile, 'rt') as f:
    classNames = f.read().rstrip('\n').split('\n')

configPath = 'ssd_mobilenet_v3_large_coco_2020_01_14.pbtxt'
weightsPath = 'frozen_inference_graph.pb'

net = cv2.dnn_DetectionModel(weightsPath, configPath)
net.setInputSize(320, 320)
net.setInputScale(1.0 / 127.5)
net.setInputMean((127.5, 127.5, 127.5))
net.setInputSwapRB(True)

largest_area = 0
largest_index = -1

while True:
    imgResponse = urllib.request.urlopen(url)
    imgNp = np.array(bytearray(imgResponse.read()), dtype=np.uint8)
    img = cv2.imdecode(imgNp, -1)

    classIds, confs, bbox = net.detect(img, confThreshold=0.5)

    if len(classIds) != 0:
        largest_area = 0
        largest_index = -1
        for i, (classId, confidence, box) in enumerate(zip(classIds.flatten(), confs.flatten(), bbox)):
            class_index = classId - 1
            if class_index >= 0 and class_index < len(classNames):
                x, y, w, h = box
                area = w * h
                if area > largest_area:
                    largest_area = area
                    largest_index = i

        if largest_index != -1:
            classId, confidence, box = classIds[largest_index], confs[largest_index], bbox[largest_index]
            class_index = classId - 1
            x, y, w, h = box
            cv2.rectangle(img, (x, y), (x + w, y + h), color=(0, 255, 0), thickness=3)
            cv2.putText(img, classNames[class_index], (x + 10, y + 30),
                        cv2.FONT_HERSHEY_COMPLEX, 1, (0, 255, 0), 2)
            
            # Send x, y, width, and height of the largest object to the second ESP32
            payload = {'x': x, 'y': y, 'width': w, 'height': h}
            requests.post(f'http://{esp32_ip_2}/object-coordinates', data=payload)

    cv2.imshow(winName, img)

    key = cv2.waitKey(5)
    if key == 27:
        break

cv2.destroyAllWindows()
