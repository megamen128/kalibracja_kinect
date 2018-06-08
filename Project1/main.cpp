
#include "OpenNI.h"
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <vector>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/core/core.hpp>
#include <string>>



using namespace cv;
using namespace std;
#define C_DEPTH_STREAM 0
#define C_COLOR_STREAM 1
#define C_NUM_STREAMS 2
#define C_MODE_COLOR 0x01
#define C_MODE_DEPTH 0x02
#define C_MODE_ALIGNED 0x04
#define C_STREAM_TIMEOUT 2000
int mode=0;
bool m_alignedStreamStatus, m_colorStreamStatus, m_depthStreamStatus;
openni::Status m_status;

const float calibrationSquareDimension = 0.025f; //meters


int main()
{
	
	while (mode == 0) {
		int wybor=0;
		std::cout << "Wpisz odpowiedni numer" << std::endl;
		std::cout << "1 - Zapisz zdjêcia z kamery kinecta oraz zwyklej kamery" << std::endl;
		std::cout << "2 - Kalibracja zapisanych zdjêæ" << std::endl;
		std::cin >> wybor;
		cin.clear(); cin.ignore(INT_MAX, '\n');
		switch (wybor) {
		case 1:   mode = 1; break;
		case 2:  mode = 2; break;
		case 3:  mode = 3; break;
		default: ;
		}
	}
	while (mode == 1)
	{
		cv::vector<cv::Mat> dataStream;
		m_colorStreamStatus = true;
		m_depthStreamStatus = true;
		openni::Device m_device;
		openni::VideoStream m_depth, m_color, **m_streams;
		openni::VideoFrameRef m_colorFrame, m_depthFrame;
		uint64_t m_colorTimeStamp, m_depthTimeStamp;
		int m_currentStream;
		cv::Mat m_colorImage, m_depthImage;

		VideoCapture cap(0); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return -1;


		m_status = openni::STATUS_OK;

		m_status = openni::OpenNI::initialize();
		std::cout << "After initialization: " << std::endl;
		std::cout << openni::OpenNI::getExtendedError() << std::endl;

		m_status = m_device.open(openni::ANY_DEVICE);
		if (m_status != openni::STATUS_OK)
		{
			std::cout << "OpenCVKinect: Device open failseed: " << std::endl;
			std::cout << openni::OpenNI::getExtendedError() << std::endl;
			openni::OpenNI::shutdown();
			return false;
		}
		m_status = m_color.create(m_device, openni::SENSOR_COLOR);
		if (m_status == openni::STATUS_OK)
		{
			m_status = m_color.start();
			if (m_status != openni::STATUS_OK)
			{

				std::cout << "OpenCVKinect: Couldn't start color stream: " << std::endl;
				std::cout << openni::OpenNI::getExtendedError() << std::endl;
				m_color.destroy();
				return false;
			}
		}
		else
		{
			std::cout << "OpenCVKinect: Couldn't find color stream: " << std::endl;
			std::cout << openni::OpenNI::getExtendedError() << std::endl;
			return false;
		}

		m_status = m_depth.create(m_device, openni::SENSOR_DEPTH);
		if (m_status == openni::STATUS_OK)
		{
			m_status = m_depth.start();
			if (m_status != openni::STATUS_OK)
			{
				std::cout << "OpenCVKinect: Couldn't start depth stream: " << std::endl;
				std::cout << openni::OpenNI::getExtendedError() << std::endl;
				m_depth.destroy();
				return false;
			}
		}
		else
		{
			std::cout << "OpenCVKinect: Couldn't find depth stream: " << std::endl;
			std::cout << openni::OpenNI::getExtendedError() << std::endl;
			return false;
		}
		int i = 1;
		m_streams = new openni::VideoStream*[C_NUM_STREAMS];
		m_streams[C_COLOR_STREAM] = &m_color;
		m_streams[C_DEPTH_STREAM] = &m_depth;

		char c = ' ';
		while (c != 27) {
			cv::Mat bufferImage;
			cv::vector<cv::Mat> Vec;

			Mat frame;
			cap >> frame; // get a new frame from camera
			cv::imshow("Color2", frame);

			bool colorCaptured = false;
			bool depthCaptured = false;
			if (m_colorStreamStatus & m_depthStreamStatus)
			{
				while (!colorCaptured || ! depthCaptured || m_depthTimeStamp != m_colorTimeStamp)
				{
					m_status = openni::OpenNI::waitForAnyStream(m_streams, C_NUM_STREAMS, &m_currentStream, C_STREAM_TIMEOUT);
					if (m_status != openni::STATUS_OK)
					{
						std::cout << "OpenCVKinect: Unable to wait for streams. Exiting" << std::endl;
						exit(EXIT_FAILURE);
					}
					switch (m_currentStream)
					{
					case C_COLOR_STREAM:
						m_color.readFrame(&m_colorFrame);
						m_colorImage.create(m_colorFrame.getHeight(), m_colorFrame.getWidth(), CV_8UC3);
						bufferImage.create(m_colorFrame.getHeight(), m_colorFrame.getWidth(), CV_8UC3);
						bufferImage.data = (uchar*)m_colorFrame.getData();
						m_colorTimeStamp = m_colorFrame.getTimestamp() >> 16;
						colorCaptured = true;
						cv::cvtColor(bufferImage, m_colorImage, CV_BGR2RGB);
						break;
					case C_DEPTH_STREAM:
						m_depth.readFrame(&m_depthFrame);
						m_depthImage.create(m_depthFrame.getHeight(), m_depthFrame.getWidth(), CV_16UC1);
						m_depthImage.data = (uchar*)m_depthFrame.getData();
						m_depthTimeStamp = m_depthFrame.getTimestamp() >> 16;
						depthCaptured = true;
					default:
						break;
					}
				}


			}
			else
			{
				m_colorImage = cv::Mat::zeros(10, 10, CV_8UC1);
				m_depthImage = cv::Mat::zeros(10, 10, CV_8UC1);
			
			}
			dataStream.push_back(m_depthImage);
			dataStream.push_back(m_colorImage);

			bufferImage.release();
			//std::cout << dataStream.size() << std::endl;
			cv::imshow("Color", dataStream[C_COLOR_STREAM]);
			cv::imshow("Depth", dataStream[C_DEPTH_STREAM]); 

			string nazwa_kin = "Zdjecia/Kinect/img";
			string nazwa_dep = "Zdjecia/Depth/img";
			string nazwa_kol = "Zdjecia/Kamera/img";

		
			c = cv::waitKey(10);
			if(i<=30 && c==83)
			{
				
					string temp_kin = nazwa_kin + std::to_string(i) + ".jpg";
					string temp_dep = nazwa_dep + std::to_string(i) + ".jpg";
					string temp_kol = nazwa_kol + std::to_string(i) + ".jpg";

					printf("Zapisano nr %d\n",i);


					cv::imshow("Img_Kinect", dataStream[C_COLOR_STREAM]);
					cv::imshow("Img_Kamera", frame);
					cv::imshow("Img_Depth", dataStream[C_DEPTH_STREAM]);

					cv::imwrite(temp_kin, dataStream[C_COLOR_STREAM]);
					cv::imwrite(temp_dep, dataStream[C_DEPTH_STREAM]);
					cv::imwrite(temp_kol, frame);
					i++;

			}

		}
		
		m_colorFrame.release();
		m_depthFrame.release();
		
		m_depth.stop();
		m_color.stop();
		openni::OpenNI::shutdown();
		m_device.close();
		
		mode = 0; 
		
	}
	if (mode == 2)
	{
		Mat K, D;
		vector < Mat > rvecs, tvecs;
		int numBoards = 8;
		int punktyPionowo = 6;
		int punktyPoziomo = 8;
		int iloscKwadratow = punktyPionowo * punktyPoziomo;
		Size wielkosc_tablicy = Size(punktyPionowo, punktyPoziomo);
		vector<vector<Point3f>> Punkty_obiektu;
		vector<vector<Point2f>> Punkty_obrazu;
		vector<Point2f> katy;
		vector<Point2f> katy_d;
		int successes = 0;

		vector<Point3f> obj;
		for (int j = 0; j<iloscKwadratow; j++)
			obj.push_back(Point3f(j / punktyPionowo, j%punktyPionowo, 0.0f));


		string nazwa = "Zdjecia/Kinect/img";
		string nazwa2 = "Zdjecia/Depth/img";

		char c;
	//	cv::namedWindow("Zdjecia", CV_WINDOW_AUTOSIZE);
		Mat image[30], image_depth[30];
		for (int i = 1; i <=30; i++)
		{
			string s = nazwa + std::to_string(i) + ".jpg";
			image[i-1] = imread(s);
			string s_depth = nazwa2 + std::to_string(i) + ".jpg";
			image_depth[i - 1] = imread(s_depth);
			
		}
		cv::imshow("win1", image[0]); 
		cv::waitKey(); 

		for (int i =0; i<=30; i++ )
		{
			Mat gray_image, gray_imaged;
			cvtColor(image[i], gray_image, CV_BGR2GRAY);
			cvtColor(image_depth[i], gray_imaged, CV_BGR2GRAY);

			bool found = findChessboardCorners(image[i], wielkosc_tablicy, katy, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

			if (found)
			{
				int xs = 640, ys = 480, xs2 = 320, ys2 = 240;
				cornerSubPix(gray_image, katy, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
				for (std::vector<Point2f>::iterator it = katy.begin(); it!= katy.end(); ++it)
				{
	
					cv::Point2f a;
					a.x = ((( (int)it->x + 10 - xs2) * 241) >> 8) + xs2;
					a.y = (((  (int)it->y + 30 - ys2) * 240) >> 8) + ys2;
					katy_d.push_back(a);

				}
				drawChessboardCorners(gray_imaged, wielkosc_tablicy, katy_d, found);
				drawChessboardCorners(gray_image, wielkosc_tablicy, katy, found);

				katy_d.clear();
			}
			cv::imshow("win1", image[i]);
			cv::imshow("win2", gray_imaged);
			cv::imshow("win3", gray_image);

			string sciezka = "Zdj_szare_z_punktami/img" + std::to_string(i+1) + ".jpg";
			imwrite(sciezka, gray_image);
			int key = waitKey(0);

			if (key == 27)
				return 0;
			else
	
			if ( found != 0  )
			{
 				Punkty_obrazu.push_back(katy);
				Punkty_obiektu.push_back(obj);

				cout<<("Snap stored!");

				successes++;

				if (successes >= 30|| i==29)
					break;
			}
			
		}
		
		mode = 0;
		cv::destroyAllWindows();
		cv::calibrateCamera(Punkty_obiektu, Punkty_obrazu, image[1].size(), K, D, rvecs, tvecs, 0);
		FileStorage fs("kalibracja_kinect", FileStorage::WRITE);
		fs << "K" << K;
		fs << "D" << D;
		fs << "punktyPionowo" << punktyPionowo;
		fs << "punktyPoziomo" << punktyPoziomo;
		cout << ("Kalibracja Skoñczona");
		
		cv::waitKey();
		return 0;
	}
	while (mode == 3)
	{
		Mat image_depth[30], image_kinect[30];

		string nazwa = "Zdjecia/Depth/img";
		string nazwa2 = "Zdj_szare_z_punktami/img";


		for (int i = 1; i <= 30; i++)
		{
			string s_depth = nazwa + std::to_string(i) + ".jpg";
			image_depth[i - 1] = imread(s_depth);
			string s_kinect = nazwa2 + std::to_string(i) + ".jpg";
			
			image_kinect[i - 1] = imread(s_kinect);
			int key = cv::waitKey();
			if (key == 27)
				return 0;

		}
		cv::destroyAllWindows();
		int key = cv::waitKey();
		mode = 0;
		if (key == 27)
			return 0;

	}
}
