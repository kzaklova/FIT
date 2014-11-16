#!/usr/bin/env python

##
# Liveness detection tool
# Author: Katerina Zaklova, 2014
# xzaklo00@fit.vutbr.cz
#

import sys
import numpy as np
import cv2
import os

##
# Function calculates temperature in specific points
# @param img image of finger
# @return array with temperature in C in points
#
def pointTemp(img):
	allpoints = []

	# apply GaussianBlur
	blur = cv2.GaussianBlur(img,(5,5),0)

	# apply Otsu threshold
	ret,otsu = cv2.threshold(blur, 0, 255, cv2.THRESH_BINARY+cv2.THRESH_OTSU)

	# find contours of finger
	contours,hierarchy = cv2.findContours(otsu, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

	for cnt in contours:
		# count center of the finger
		M = cv2.moments(cnt)
		if M['m00'] != 0:
			c_x = int(M['m10']/M['m00'])
			c_y = int(M['m01']/M['m00'])

	# count extreme points of the finger
	rightmost = tuple(cnt[cnt[:,:,0].argmax()][0])
	topmost = tuple(cnt[cnt[:,:,1].argmin()][0])
	bottommost = tuple(cnt[cnt[:,:,1].argmax()][0])

	# points on lengthwise axis
	p1_x = rightmost[0]-((bottommost[1]-topmost[1])/4)
	p2_x = rightmost[0]-((bottommost[1]-topmost[1])*3/4)
	p3_x = rightmost[0]-((bottommost[1]-topmost[1])*5/4)
	p4_x = rightmost[0]-((bottommost[1]-topmost[1])*7/4)
	p_y = topmost[1] + ((bottommost[1]-topmost[1])/2)

	# points on edges
	p5_x = rightmost[0]-((bottommost[1]-topmost[1])/2)
	p5_y = c_y - ((c_y - topmost[1])/3)
	p6_x = rightmost[0]-((bottommost[1]-topmost[1])/2)
	p6_y = c_y + ((bottommost[1] - c_y)/3)

	# convert values to C
	allpoints.append(img[p_y, p1_x]*8/float(255)+27)
	allpoints.append(img[p_y, p2_x]*8/float(255)+27)
	allpoints.append(img[p_y, p3_x]*8/float(255)+27)
	allpoints.append(img[p_y, p4_x]*8/float(255)+27)
	allpoints.append(img[p5_y, p5_x]*8/float(255)+27)
	allpoints.append(img[p6_y, p6_x]*8/float(255)+27)

	return allpoints

##
# Function calculates average temperature of finger
# @param img image of finger
# @return average temperature in C
#
def averageTemp(img):
	# get width and height
	width, height = img.shape[:2]
	val = 0
	values = []

	for i in range(0, height-1):
		for j in range(0, width-1):
			val = img[j,i]
			# ignore black background
			if val < 10:
				continue;
			values.append(val)

	# calculate average value and convert to C
	return (sum(values)/len(values))*8/float(255)+27

##
# Function calculates maximal temperature
# and its coverage against finger
# @param img image of finger
# @return array with temperature and area
#
def maxTemp(img):
	maxtp = []
	area = 0
	# first threshold will be 255
	t = 258
	# value isn't in picture, decrease the threshold -0.1 C
	while area == 0:
		t -= 3
		ret,thr = cv2.threshold(img, t, 255, cv2.THRESH_BINARY)
		area = cv2.countNonZero(thr)
	# convert value to C
	maxtp.append(t*8/float(255)+27)

	# for highest 0.5 C in picture
	for i in range (0, 4):
		t -= 3
	ret,thr = cv2.threshold(img, t, 255, cv2.THRESH_BINARY)
	area = cv2.countNonZero(thr)
	# warmest area against finger area
	maxtp.append(area/float(cv2.countNonZero(img)))

	return maxtp

##
# Function calculates minimal temperature
# and its coverage against finger
# @param img image of finger
# @return array with temperature and area
#
def minTemp(img):
	mintp = []
	area = 0
	# first threshold will be 5 to reduce black background
	t = 2
	# value isn't in picture, increase the threshold 0.1 C
	while area == 0:
		t += 3
		ret,thr = cv2.threshold(img, t, 255, cv2.THRESH_BINARY_INV)
		area = cv2.countNonZero(thr) - (480*530-cv2.countNonZero(img))
		# convert value to C
		mintp.append(t*8/float(255)+27)

	# for lowest 0.5 C in picture
	for i in range (0, 4):
		t += 3
	ret,thr = cv2.threshold(img, t, 255, cv2.THRESH_BINARY_INV)
	area = cv2.countNonZero(thr) - (480*530-cv2.countNonZero(img))
	# coldest area against finger area
	mintp.append(area/float(cv2.countNonZero(img)))

	return mintp

##
# Function evaluates points for liveness detection (values used in this
# function are based on results of experiments)
# @param avr average temperature
# @param pt temperature in points
# @param mat maximal temperature and coverage
# @param mit minimal temperature and coverage
# @return sum of points
#
def evaluate(average, points, maximal, minimal):
	# average temperature
	if average <= 30.0:
		average = 0
	elif average > 32.0:
		average = 30
	else:
		average = round((average-30)*30/2)

	#points
	p = 0
	# difference between p1 and p2
	p1 = points[1]-points[0]
	if p1 <= 0.1 or p1 > 1.1:
		p = 0
	elif p1 > 0.1 and p1 <=0.8:
		p = round((p1-0.1)*150/7)
	else:
		p = round(15-((p1-0.8)*150/3))
	# difference between p2 and p3
	p2 = abs(points[2]-points[1])
	if p2 > 0.4:
		p += 0
	else:
		p += round(15-(p2*150/4))
	# average temperature from points
	pa = sum(points)/len(points)
	if pa < 31.4:
		p += 0
	elif pa > 33.4:
		p += 10
	else:
		p += (pa-31.4)*10/2

	# maximal temperature
	if maximal[0] <= 31.00:
		maxt = 0
	else:
		maxt = round((maximal[0]-31)*15/4)

	# minimal coverage
	if minimal[1] <= 0.06 or minimal[1] >= 0.2:
		mint = 0
	else:
		mint = round(15-((minimal[1]-0.06)*1500/14))

	# total points
	return int(average+p+maxt+mint)

##
# Main function
#
def main():
	# helpmsg
	if len(sys.argv) == 2 and sys.argv[1] == "-h":
		print ("Liveness detection tool \n"
		"(c) Katerina Zaklova, xzaklo00, 2014\n"
		"Usage: \n"
		"./detect.py [directory_with_samples]\n"
		"Script detects liveness for given set of images of fingers.\n"
		"Directory is optional, default directory is set to 'samples/'\n")
		exit(0)

	#set directory with images
	if len(sys.argv) == 1:
		folder="samples/"
	elif len(sys.argv) == 2:
		folder=sys.argv[1]
	else:
		print >> sys.stderr, 'Wrong arguments, rerun with -h'
		exit(1)

	# fix directory name
	if "/" not in folder:
		folder = folder + "/"

	rejected = 0
	accepted = 0
	score = 0

	try:
		#list directory
		files = os.listdir(folder)
	except:
		print >> sys.stderr, 'Non-existing directory!'
		exit(1)

	for name in files:
		# read image in grayscale
		img = cv2.imread(folder+name, 0)

		# area of finger is too small for detection
		if cv2.countNonZero(img) < 10000:
			rejected += 1
			print name,": REJECTED"
			continue;

		# temperature in points
		points = pointTemp(img)
		# average temperature
		average = averageTemp(img)
		# maximal temperature and coverage
		maximal = maxTemp(img)
		# minimal temperature and coverage
		minimal = minTemp(img)

		score = evaluate(average, points, maximal, minimal)

		if score > 40:
			accepted += 1
			print name,": ACCEPTED"
		else:
			rejected += 1
			print name,": REJECTED"

	print "Accepted fingers:", accepted
	print "Rejected fingers:", rejected


if __name__ == "__main__":
	main()
