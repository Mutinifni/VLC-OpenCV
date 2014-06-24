// Increases/Decreases the volume of media played by VLC media player, based on Haar hand detection.

// Libraries used : OpenCV, LibVLC

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>
#include <vlc/vlc.h>

using namespace std;
using namespace cv;

// Create CC for HaarClassification
CascadeClassifier hand_cascade;

// Hand detection function, returns the average x-coordinate
float handDetect(Mat frame)
{
	std::vector<Rect> hands;
	Mat frame_gray;
	float avgPos = 0;

	// Convert to GRAY scale for cascade
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// Detect hand
	hand_cascade.detectMultiScale(frame_gray, hands, 1.1, 2, 0, Size(30,30));

	for(size_t i = 0; i < hands.size(); i++)
	{
		avgPos += (hands[i].x + hands[i].width/2);
	}
	avgPos = avgPos / hands.size();

	return avgPos;
}


int main(int argc, char** argv)
{
	// Variable declarations
	VideoCapture capture;
	Mat frame;
	int esc, volume; 
	float handPos, minFWidth, maxFWidth;

	// LibVLC requirements, plays video specified as a command line argument
	libvlc_instance_t *instance = libvlc_new(0, NULL);
	libvlc_media_t *media = libvlc_media_new_path(instance, argv[1]);	
	libvlc_media_player_t *mplayer = libvlc_media_player_new_from_media(media);
	libvlc_media_release(media);

	// Error checking
	if(!hand_cascade.load("XML/palm.xml"))
	{
		printf("Error loading XML/palm.xml\n");
		return -1; 
	}

	capture.open(-1);
	if(!capture.isOpened())
	{
		printf("Error opening video capture.\n");
		return -1;
	}

	libvlc_media_player_play(mplayer);
	capture.read(frame);
	maxFWidth = (0.65) * capture.get(CV_CAP_PROP_FRAME_WIDTH);
	minFWidth = (0.35) * capture.get(CV_CAP_PROP_FRAME_WIDTH);
	volume = libvlc_audio_get_volume(mplayer);

	// Processing
	while(capture.read(frame))
	{
		// More error checking
		if(frame.empty())
		{
			printf("Empty frame.\n");
			break;
		}

		// Function call that returns x-coordinate of detected hand
		handPos = handDetect(frame);

		// Increase or Decrease volume based on palm position
		if(handPos >= maxFWidth && volume >= 0)
		{
			volume -= 10;
			libvlc_audio_set_volume(mplayer, volume);
			//printf("%d\n", volume);
		}
		else if(handPos <= minFWidth && volume <= 100)
		{
			volume += 10;
			libvlc_audio_set_volume(mplayer, volume);
			//printf("%d\n", volume);
		} 

		// Premature break condition
		esc = waitKey(10);
		if((char)esc == 27)
		{
			printf("Escape pressed, breaking out.\n");
			break;
		}
	}	

	return 0;
}