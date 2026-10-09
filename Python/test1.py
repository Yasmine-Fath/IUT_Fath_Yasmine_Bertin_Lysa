import cv2
print(cv2.__version__)

import numpy as np
from urllib.request import urlopen
req = urlopen("http://www.vgies.com/downloads/robocup.png")
arr = np.asarray(bytearray(req.read()), dtype=np.uint8)
img = cv2.imdecode(arr, -1)
cv2.imshow("RoboCup␣image", img)
cv2.waitKey(0)

#TRANSFORMATIONS MANUELLES D’UNE IMAGE
height = img.shape[0]
width = img.shape[1]
channels = img.shape[2]
imgTransform = img
for x in range(0, (int)(width)):
    for y in range (0, (int)(height)):
        imgTransform[y,x][0] *= 0
        imgTransform[y,x][1] *= x/width
        imgTransform[y,x][2] *= 0
cv2.imshow("Transformation␣manuelle␣de␣l’image", imgTransform)
cv2.waitKey(0)
