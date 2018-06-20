import cv2
import cv2.cv as cv
import numpy as np
import serial
import time

kernel = np.ones((5,5),np.uint8)

# Take input from webcam
cap = cv2.VideoCapture(-1)

# Reduce the size of video to 320x240 so rpi can process faster
cap.set(3,320)
cap.set(4,240)

def nothing(x):
    pass
# Creating a windows for later use
cv2.namedWindow('HueComp')
cv2.namedWindow('SatComp')
cv2.namedWindow('ValComp')
cv2.namedWindow('closing')
cv2.namedWindow('tracking')


# Creating track bar for min and max for hue, saturation and value
# You can adjust the defaults as you like
cv2.createTrackbar('hmin', 'HueComp',0,300,nothing)
cv2.createTrackbar('hmax', 'HueComp',9,500,nothing)

cv2.createTrackbar('threshold', 'HueComp',75,100,nothing)
cv2.createTrackbar('xthr', 'HueComp',85,1000,nothing)
cv2.createTrackbar('far_thr', 'ValComp',24,100,nothing)


cv2.createTrackbar('smin', 'SatComp',182,255,nothing)
cv2.createTrackbar('smax', 'SatComp',255,255,nothing)

cv2.createTrackbar('vmin', 'ValComp',79,300,nothing)
cv2.createTrackbar('vmax', 'ValComp',281,300,nothing)

# My experimental values
# hmn = 12
# hmx = 37
# smn = 145
# smx = 255
# vmn = 186
# vmx = 255

port = serial.Serial("/dev/ttyAMA0", baudrate=115200) #pins 14,15 HW UART


while(1):
    

    serial_out = 0
    _, frame = cap.read()

    #converting to HSV
    hsv = cv2.cvtColor(frame,cv2.COLOR_BGR2HSV)
    hue,sat,val = cv2.split(hsv)

    # get info from track bar and appy to result
    hmn = cv2.getTrackbarPos('hmin','HueComp')
    hmx = cv2.getTrackbarPos('hmax','HueComp')

    thr = cv2.getTrackbarPos('threshold', 'HueComp')
    xpos = cv2.getTrackbarPos('xthr', 'HueComp')
    farthr = cv2.getTrackbarPos('far_thr', 'ValComp')



    smn = cv2.getTrackbarPos('smin','SatComp')
    smx = cv2.getTrackbarPos('smax','SatComp')


    vmn = cv2.getTrackbarPos('vmin','ValComp')
    vmx = cv2.getTrackbarPos('vmax','ValComp')

    # Apply thresholding
    hthresh = cv2.inRange(np.array(hue),np.array(hmn),np.array(hmx))
    sthresh = cv2.inRange(np.array(sat),np.array(smn),np.array(smx))
    vthresh = cv2.inRange(np.array(val),np.array(vmn),np.array(vmx))

    # AND h s and v
    tracking = cv2.bitwise_and(hthresh,cv2.bitwise_and(sthresh,vthresh))

    # Some morpholigical filtering
    dilation = cv2.dilate(tracking,kernel,iterations = 1)
    closing = cv2.morphologyEx(dilation, cv2.MORPH_CLOSE, kernel)
    closing = cv2.GaussianBlur(closing,(5,5),0)

    # Detect circles using HoughCircles
    circles = cv2.HoughCircles(closing,cv.CV_HOUGH_GRADIENT,2,120,param1=120,param2=50,minRadius=1,maxRadius=0)
    # circles = np.uint16(np.around(circles))
    #Draw Circles
    if circles is None:
           port.write("0") #if no ball is detected turn left
           port.write("\n")
           print "searching ball"
    if circles is not None:
            lower = 0;
            for i in circles[0,:]:
 
                if int(round(i[0])) < 0+xpos:
                    serial_out = 2
                elif int(round(i[0])) > 320-xpos:
                    serial_out = 3
                elif int(round(i[2])) < farthr:
                    cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),int(round(i[2])),(255,0,0),5)
                    cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),2,(255,0,0),10)
                    serial_out = 5
    # If the ball is far, draw it in RED
                elif int(round(i[2])) < thr:
                    cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),int(round(i[2])),(0,0,255),5)
                    cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),2,(0,0,255),10)
                    serial_out = 1
    # else draw it in GREEN
                elif int(round(i[2])) > thr:
                    cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),int(round(i[2])),(0,255,0),5)
                    cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),2,(0,255,0),10)
                    serial_out = 0
                    lower = 1;
                    
                

                port.write(str(serial_out))
                port.write("\n")
                print str(serial_out)
                if lower ==1:
                    time.sleep(1)
                    port.write(str(4))
                    port.write("\n")
                    print "actuating magnet"
                    

    #you can use the 'buzz' variable as a trigger to switch some GPIO lines on Rpi :)
    # print buzz                    
    #if buzz:
    #    port.write(round(i[2])"1")
     #   port.write("\n")
    #    print "FORWARD"
    #else:
     #   port.write("0")
      #  port.write("\n")
      #  print "STOP"
        # put your GPIO line here

    
    #Show the result in frames
    cv2.imshow('HueComp',hthresh)
    cv2.imshow('SatComp',sthresh)
    cv2.imshow('ValComp',vthresh)
    cv2.imshow('closing',closing)
    cv2.imshow('tracking',frame)

    k = cv2.waitKey(5) & 0xFF
    if k == 27:
        break

cap.release()

cv2.destroyAllWindows()
