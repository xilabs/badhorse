
// 	badhorse.c
//
// Recognize a license plate



#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <json/json.h>

using namespace cv;
using namespace std;

int blursize=5;			// Must be odd


void find_squares(Mat& image, vector<vector<Point> >& squares);
void find_plate(Mat& img, vector<vector<Point> >& squares);
double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 );
cv::Mat debugSquares( std::vector<std::vector<cv::Point> > squares, cv::Mat image );

int main () {

	// Get settings
	Json::Value root;
	Json::Reader reader;
	ifstream config;
	config.open("/var/www/badhorse/settings.json");
	
	if(!reader.parse(config,root)) {
	
		config.close();
		cout << "Error reading settings.json: " << reader.getFormatedErrorMessages() << std::endl;
		return -1;
	
	}
	config.close();
	double rotation=(double)root["input"].get("rotation",0).asInt();
	double left=(double)root["input"]["crop_h"][(Json::Value::UInt)0].asInt()/100.0f;
	double right=(double)root["input"]["crop_h"][(Json::Value::UInt)1].asInt()/100.0f;

	// Load image
	Mat image=imread("/var/www/badhorse/in.jpg", CV_LOAD_IMAGE_COLOR);
	if(!image.data) {
	
		cout << "Error reading image" << std::endl;
		return -1;
	}
	std::cout << "Loaded\n";
	
	// Crop source
	Mat cropped=image(Rect(image.cols*left, 0, ((right-left)*image.cols),image.rows));
	
	
	// Rotate source
	Point2f center(cropped.cols/2.0f,cropped.rows/2.0f);
	warpAffine(cropped, cropped, getRotationMatrix2D(center, rotation, 1.0), cropped.size());
	
	
	
	// Create a vector for the squares, and find the squares
	std::cout << "Finding squares...\n";
	vector<vector<Point> > squares;
	
	// Intensity edge detection
	Mat intensity(cropped);
	cvtColor(cropped, intensity, CV_RGB2GRAY);
	medianBlur(intensity,intensity,blursize);
	Canny(intensity, intensity, 100, 300, 3);

/*

	// Grab the Hue channel, blur it, and perform Canny:
	Mat hsv(cropped);
	cvtColor(cropped, hsv, CV_BGR2HSV);
	
	Mat hue(cropped.rows, cropped.cols, CV_8UC1);
	int from_to[]= { 0, 0 };
	mixChannels(&hsv, 1, &hue, 1, from_to, 1);
//	medianBlur(hue,hue,blursize*2+1);
//	Canny(hue, hue, 200, 300, 3);
	*/
		
	// Line detection
	vector<Vec4i> lines;
	HoughLinesP(intensity, lines, 1, 10*CV_PI/180, 10, 50, 5);		// Angle, thresh, min_len, max_gap
	
	// BGR output
	//cvtColor(hue, hue, CV_GRAY2BGR);
	
	
	for(size_t i=0; i<lines.size(); i++) {
	
		Vec4i l=lines[i];
		line(cropped, Point(l[0],l[1]), Point(l[2],l[3]), Scalar(0,255,0), 3, CV_AA);
	}


//	imwrite("/var/www/badhorse/out.jpg",hue);
	imwrite("/var/www/badhorse/out.jpg",cropped);

	
/*	find_squares(image, squares);
	
	// Draw on image
	Mat output=debugSquares( squares, image );
	
	std::cout << "Writing image...\n";
	imwrite("out.jpg",output);
*/	
	return 0;

}


void find_plate(Mat& img, vector<vector<Point> >& squares) {

	

}

// Thanks to http://stackoverflow.com/questions/8667818/opencv-c-obj-c-detecting-a-sheet-of-paper-square-detection
void find_squares(Mat& image, vector<vector<Point> >& squares) {

    // blur will enhance edge detection
    Mat blurred(image);
    medianBlur(image, blurred, 9);

    Mat gray0(blurred.size(), CV_8U), gray;
    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for (int c = 0; c < 3; c++)
    {
        int ch[] = {c, 0};
        mixChannels(&blurred, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        const int threshold_level = 2;
        for (int l = 0; l < threshold_level; l++)
        {
            // Use Canny instead of zero threshold level!
            // Canny helps to catch squares with gradient shading
            if (l == 0)
            {
                Canny(gray0, gray, 10, 20, 3); // 

                // Dilate helps to remove potential holes between edge segments
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                    gray = gray0 >= (l+1) * 255 / threshold_level;
            }

            // Find contours and store them in a list
            findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

            // Test contours
            vector<Point> approx;
            for (size_t i = 0; i < contours.size(); i++)
            {
                    // approximate contour with accuracy proportional
                    // to the contour perimeter
                    approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

                    // Note: absolute value of an area is used because
                    // area may be positive or negative - in accordance with the
                    // contour orientation
                    if (approx.size() == 4 &&
                            fabs(contourArea(Mat(approx))) > 1000 &&
                            isContourConvex(Mat(approx)))
                    {
                            double maxCosine = 0;

                            for (int j = 2; j < 5; j++)
                            {
                                    double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                                    maxCosine = MAX(maxCosine, cosine);
                            }

                            if (maxCosine < 0.3)
                                    squares.push_back(approx);
                    }
            }
        }
    }
}


double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

cv::Mat debugSquares( std::vector<std::vector<cv::Point> > squares, cv::Mat image ){
    for ( unsigned int i = 0; i< squares.size(); i++ ) {
        // draw contour
        cv::drawContours(image, squares, i, cv::Scalar(255,0,0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());

        // draw bounding rect
        cv::Rect rect = boundingRect(cv::Mat(squares[i]));
        cv::rectangle(image, rect.tl(), rect.br(), cv::Scalar(0,255,0), 2, 8, 0);

        // draw rotated rect
        cv::RotatedRect minRect = minAreaRect(cv::Mat(squares[i]));
        cv::Point2f rect_points[4];
        minRect.points( rect_points );
        for ( int j = 0; j < 4; j++ ) {
            cv::line( image, rect_points[j], rect_points[(j+1)%4], cv::Scalar(0,0,255), 1, 8 ); // blue
        }
    }

    return image;
}