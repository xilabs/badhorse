BOOST_ROOT=/usr/local/boost_1_52_0
EXTRA_ROOT=/usr/local


all:
	 g++ -I$(BOOST_ROOT)  -I$(EXTRA_ROOT)/include -L$(BOOST_ROOT)/stage/lib -L$(EXTRA_ROOT)/lib  -Wall  badhorse.cpp  -o badhorse -lopencv_core -lopencv_highgui -lopencv_imgproc -ljson_linux-gcc-4.7_libmt
	 cp badhorse /var/www/badhorse/
	 chmod a+x /var/www/badhorse
	 chown www-data.www-data /var/www/badhorse
clean:
	rm badhorse