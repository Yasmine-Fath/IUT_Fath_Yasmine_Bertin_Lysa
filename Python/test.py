import cv2
print(cv2.__version__)

import numpy as np
from urllib.request import urlopen
req = urlopen("http://www.vgies.com/downloads/robocup.png")
arr = np.asarray(bytearray(req.read()), dtype=np.uint8)
img = cv2.imdecode(arr, -1)
cv2.imshow("RoboCup␣image", img)
cv2.waitKey(0)
##
##B, G, R = cv2.split(img)
##cv2.imshow("original", img)
##cv2.waitKey(0)
##cv2.imshow("blue", B)
##cv2.waitKey(0)
##cv2.imshow("Green", G)
##cv2.waitKey(0)
##cv2.imshow("Red", R)
##cv2.waitKey(0)

#converting the image to HSV color space using cvtColor function
imagehsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
H, S, V = cv2.split(imagehsv)
##cv2.imshow("Hue", H)
##cv2.waitKey(0)
##cv2.imshow("Saturation", S)
##cv2.waitKey(0)
##cv2.imshow("Value", V)
##cv2.waitKey(0)
##
##
##
###Definition des limites basses et hautes de la couleur jaune en HSV
###A noter que le jaune se situe vers les 25 degres dans la roue de couleur HSV en H
##lower_yellow = np.array([20, 100, 100])
##upper_yellow = np.array([30,255,255])
###Masquage de l’image HSV pour ne garder que les zones jaunes
##imagemaskyellow = cv2.inRange(imagehsv, lower_yellow, upper_yellow)
##cv2.imshow("Image␣Masque␣Jaune", imagemaskyellow)
##cv2.waitKey(0)


###Pour le vert
##lower_green = np.array([35, 20, 20])
##upper_green = np.array([85,255,255])
###Masquage de l’image HSV pour ne garder que les zones vertes
##imagemaskgreen = cv2.inRange(imagehsv, lower_green, upper_green)
##cv2.imshow("Image␣Masque␣Vert", imagemaskgreen)
##cv2.waitKey(0)

###Filtre Vert sur l'image resultante 
##image = np.zeros(img.shape, np.uint8)
##image[:] = (0,255,0)
##cv2.imshow('image',image)
###Pour le vert
##lower_green = np.array([35, 20, 20])
##upper_green = np.array([85,255,255])
###Masquage de l’image HSV pour ne garder que les zones vertes
##imagemaskgreen = cv2.inRange(imagehsv, lower_green, upper_green)
##res = cv2.bitwise_and(image,image, mask= imagemaskgreen)
##cv2.imshow("Image␣Masque␣Vert", imagemaskgreen)
##cv2.imshow('res',res)
##cv2.waitKey(0)

#Filtre Jaune Bleu et vert sur l'image resultante
imageJ = np.zeros(img.shape, np.uint8)
imageJ[:] = (0,255,255)
imageB = np.zeros(img.shape, np.uint8)
imageB[:] = (255,0,0)
imageV = np.zeros(img.shape, np.uint8)
imageV[:] = (0,255,0)

lower_yellow = np.array([20, 100, 100])
upper_yellow = np.array([30,255,255])
#Masquage de l’image HSV pour ne garder que les zones jaunes
imagemaskyellow = cv2.inRange(imagehsv, lower_yellow, upper_yellow)
cv2.imshow("Image␣Masque␣Jaune", imagemaskyellow)
resJ = cv2.bitwise_and(imageJ,imageJ, mask= imagemaskyellow)
cv2.imshow('resJ',resJ)
cv2.waitKey(0)

lower_noir = np.array([0, 0, 0])
upper_noir = np.array([255,255,50])    #pour le H et le S étants en bas du cône on a pas besoin de les limiter
#Masquage de l’image HSV pour ne garder que les zones noires
imagemasknoir = cv2.inRange(imagehsv, lower_noir, upper_noir)
cv2.imshow("Image␣Masque␣Noir", imagemasknoir)
resB = cv2.bitwise_and(imageB,imageB, mask= imagemasknoir)
cv2.imshow('resB',resB)
cv2.waitKey(0)

lower_green = np.array([35, 20, 20])
upper_green = np.array([85,255,255])
#Masquage de l’image HSV pour ne garder que les zones vertes
imagemaskgreen = cv2.inRange(imagehsv, lower_green, upper_green)
cv2.imshow("Image␣Masque␣Vert", imagemaskgreen)

resV = cv2.bitwise_and(imageV,imageV, mask= imagemaskgreen)
cv2.imshow('resV',resV)
cv2.waitKey(0)

restot1 = resB +resV + resJ
cv2.imshow('restot1',restot1)
cv2.waitKey(0)








