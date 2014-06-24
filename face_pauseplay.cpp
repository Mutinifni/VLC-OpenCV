// Plays/pauses a media played by VLC media player, based on Haar face detection.

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
CascadeClassifier face_cascade;

// Face detection function, returns whether present
int faceDetect(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	// Convert to GRAY scale for cascade
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// Detect face
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0, Size(30,30));

	// Return whether present
	if(faces.size() == 0)
		return 0;

	return 1;
}

int main(int argc, char** argv)
{
	// Variable declarations
	VideoCapture capture;
	Mat frame;
	int esc; 
	int facePresent, facePast = 1;

	// LibVLC requirements, plays video specified as a command line argument
	libvlc_instance_t *instance = libvlc_new(0, NULL);
	libvlc_media_t *media = libvlc_media_new_path(instance, argv[1]);	
	libvlc_media_player_t *mplayer = libvlc_media_player_new_from_media(media);
	libvlc_media_release(media);

	// Error checking
	if(!face_cascade.load("XML/face.xml"))
	{
		printf("Error loading XML/face.xml\n");
		return -1;
	}

	if(!face_cascade.load("XML/haarcascade_frontalface_alt_tree.xml"))
	{
		printf("Error loading XML/haarcascade_frontalface_alt_tree.xml\n");
		return -1; 
	}

	capture.open(-1);
	if(!capture.isOpened())
	{
		printf("Error opening video capture.\n");
		return -1;
	}

	libvlc_media_player_play(mplayer);

	// Processing
	while(capture.read(frame))
	{
		// More error checking
		if(frame.empty())
		{
			printf("Empty frame.\n");
			break;
		}

		// Function call, and pause/play depending on return value
		facePresent = faceDetect(frame);

		if(!facePresent && facePast)
			libvlc_media_player_pause(mplayer);

		if(facePresent && !facePast)
			libvlc_media_player_play(mplayer);

		facePast = facePresent;

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
