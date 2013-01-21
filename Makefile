BOOST_ROOT=/usr/local/boost_1_52_0
OPENCV_ROOT=/usr/local

all:
	 g++ -I$(BOOST_ROOT) -I$(OPENCV_ROOT)/include -L$(BOOST_ROOT)/stage/lib -L$(OPENCV_ROOT)/lib  -Wall  badhorse.cpp  -o badhorse -lopencv_core -lopencv_highgui -lopencv_imgproc
	 
clean:
	rm badhorse