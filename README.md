Badhorse
=========

Badhorse is a simple framework for performing automobile license plate recognition

Philosophy
=========

We can count on a couple of items that will make our job easier (assumes fixed camera):

1) The plate will always appear within a fairly defined zone (unless the driver is not in their lane!)
2) Any angle or perspective will be the same every time, hence correctable
3) The plate will always be the same size and shape

This means that we should always start with the following steps:

1) Cropping (user defined area - tall but narrow for centered plates)
	* User controls: crop area
2) Perspective and rotation correction
	* User controls: rotation and perspective

Next, we can actually perform the shape detection:

3) Edge detection (Canny filter) on Hue
4) Edge detection (Canny filter) on Value
5) Combine above to produce composite
6) Hough line filter, discarding non-vertical/horizontal lines


Notes
=========

*Before compiling test1, make sure you download the latest copy of Boost and set BOOST_ROOT in Makefile to the Boost folder*
*You will also need OpenCV*

Boost is found at http://www.boost.org