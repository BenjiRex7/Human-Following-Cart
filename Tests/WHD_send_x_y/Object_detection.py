import cv2
import urllib.request
import numpy as np
import requests  # Import requests library to send HTTP requests

# IP address of the ESP32 server
esp32_ip = '192.168.100.202'
# IP address of the second ESP32 server
esp32_ip_2 = '192.168.100.66'

url = 'http://192.168.100.202/cam-hi.jpg'
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

while True:
    imgResponse = urllib.request.urlopen(url)
    imgNp = np.array(bytearray(imgResponse.read()), dtype=np.uint8)
    img = cv2.imdecode(imgNp, -1)

    classIds, confs, bbox = net.detect(img, confThreshold=0.5)
    print(classIds, bbox)

    if len(classIds) != 0:
        for classId, confidence, box in zip(classIds.flatten(), confs.flatten(), bbox):
            class_index = classId - 1
            if class_index >= 0 and class_index < len(classNames):
                x_min, y_min, width, height = box
                cv2.rectangle(img, (x_min, y_min), (x_min + width, y_min + height), color=(0, 255, 0), thickness=3)
                cv2.putText(img, classNames[class_index], (x_min + 10, y_min + 30),
                            cv2.FONT_HERSHEY_COMPLEX, 1, (0, 255, 0), 2)
                
                # Send x_min and y_min to the second ESP32
                payload = {'x_min': x_min, 'y_min': y_min}
                requests.post(f'http://{esp32_ip_2}/object-coordinates', data=payload)

    cv2.imshow(winName, img)

    # Wait for ESC to be pressed to end the program
    key = cv2.waitKey(5)
    if key == 27:
        break

cv2.destroyAllWindows()
