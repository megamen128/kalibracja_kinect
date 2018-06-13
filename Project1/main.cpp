
#include "OpenNI.h"
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <vector>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/core/core.hpp>
#include <string>

#include <NuiApi.h>

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
		std::cout << "2 - Rektyfikacja zdjêæ z kamery oddzielnej do kamery kinecta" << std::endl;

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

		INuiCoordinateMapper * pMapper;
		//INuiSensor
		//NuiGetCoordinateMapper(&pMapper);





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
			int x, y;
			//openni::CoordinateConverter::convertDepthToColor(m_depth, m_color, 0, 0, 0, &x, &y);
		//	std::cout << x << y;
			string nazwa_kin = "Zdjecia/Kinect/img";
			string nazwa_dep = "Zdjecia/Depth/img";
			string nazwa_kol = "Zdjecia/Kamera/img";

		
			c = cv::waitKey(10);
			if(i<=30 && (c==83 || c==115) )
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
		Mat K, D,K3,D3;
		vector < Mat > rvecs, tvecs,rvecs2,tvecs2;
		int numBoards = 8;
		int punktyPionowo = 6;
		int punktyPoziomo = 8;
		int iloscKwadratow = punktyPionowo * punktyPoziomo;
		Size wielkosc_tablicy = Size(punktyPionowo, punktyPoziomo);
		vector<vector<Point3f>> Punkty_obiektu;
		vector<vector<Point2f>> Punkty_obrazu, Punkty_obrazu_kamera;
		vector<Point2f> katy, katy_kamera;
		vector<Point2f> katy_d;
		int successes = 0;

		vector<Point3f> obj;
		for (int j = 0; j<iloscKwadratow; j++)
			obj.push_back(Point3f(j / punktyPionowo, j%punktyPionowo, 0.0f));


		string nazwa = "Zdjecia/Kinect/img";
		string nazwa2 = "Zdjecia/Depth/img";
		string nazwa3 = "Zdjecia/Kamera/img";
		char c;
	//	cv::namedWindow("Zdjecia", CV_WINDOW_AUTOSIZE);
		Mat image[30], image_depth[30], image_kamera[30];
		for (int i = 1; i <=30; i++)
		{
			string s = nazwa + std::to_string(i) + ".jpg";
			image[i-1] = imread(s);
			string s_depth = nazwa2 + std::to_string(i) + ".jpg";
			image_depth[i - 1] = imread(s_depth);
			string s_kamera = nazwa3 + std::to_string(i) + ".jpg";
			image_kamera[i - 1] = imread(s_kamera);

			cv::waitKey();

			
		}
	
		
		for (int i =0; i<=30; i++ )
		{
			Mat gray_image, gray_imaged, gray_image_kamera;
			cvtColor(image[i], gray_image, CV_BGR2GRAY);
			cvtColor(image_depth[i], gray_imaged, CV_BGR2GRAY);
			cvtColor(image_kamera[i], gray_image_kamera, CV_BGR2GRAY);


			bool found = findChessboardCorners(image[i], wielkosc_tablicy, katy, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
			bool found2 = findChessboardCorners(image_kamera[i], wielkosc_tablicy, katy_kamera, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

			if (found)
			{
				int xs = 640, ys = 480, xs2 = 320, ys2 = 240;
				cornerSubPix(gray_image, katy, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
				for (std::vector<Point2f>::iterator it = katy.begin(); it!= katy.end(); ++it)
				{
					cv::Point2f a;
					a.x = (float)it->x-27.0;
					a.y = (float)it->y-32.0;
					katy_d.push_back(a);
				}
				drawChessboardCorners(gray_imaged, wielkosc_tablicy, katy_d, found);
				drawChessboardCorners(gray_image, wielkosc_tablicy, katy, found);

				katy_d.clear();
			}
			if (found2)
			{
				cornerSubPix(gray_image_kamera, katy_kamera, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
				drawChessboardCorners(gray_image_kamera, wielkosc_tablicy, katy_kamera, found);
			}


			cv::imshow("win1", image[i]);
			cv::imshow("win2", gray_imaged);
			cv::imshow("win3", gray_image);
			cv::imshow("win4", gray_image_kamera);

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

		
			}
			if (found2 != 0)
			{
				Punkty_obrazu_kamera.push_back(katy_kamera);

			}
			if (successes >= 30 || i == 29)
				break;
		}
		
		mode = 0;
		cv::destroyAllWindows();
		cv::calibrateCamera(Punkty_obiektu, Punkty_obrazu, image[1].size(), K, D, rvecs, tvecs, 0); //kalibracja kamery kolorowej kinecta
		Mat dst;
		/*for (int i = 0; i < 29; i++)
		{
			cv::undistort(image[i], dst, K, D);
			imshow("img_original", image[i]);
			imshow("img_undistorted", dst);
			if(cv::waitKey() == 115) continue;
		}*/

		cv::calibrateCamera(Punkty_obiektu, Punkty_obrazu_kamera, image[1].size(), K3, D3, rvecs2, tvecs2, 0);  //kalibracja oddzielnej kamery
		/*for (int i = 0; i < 29; i++)
		{
			cv::undistort(image_kamera[i], dst, K3, D3);
			imshow("img_original2", image_kamera[i]);
			imshow("img_undistorted2", dst);
			if (cv::waitKey() == 115) continue;
		}*/


		Mat K1, D1, K2, D2, R, T, E, F;
	/*	cv::stereoCalibrate(Punkty_obiektu, Punkty_obrazu, Punkty_obrazu_kamera, K1, D1, K2, D2, image[1].size(), R, T, E, F,
			TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5),
			CV_CALIB_FIX_ASPECT_RATIO +
			CV_CALIB_ZERO_TANGENT_DIST +
			CV_CALIB_SAME_FOCAL_LENGTH +
			CV_CALIB_RATIONAL_MODEL +
			CV_CALIB_FIX_K3 + CV_CALIB_FIX_K4 + CV_CALIB_FIX_K5);*/
		FileStorage fs("kalibracja_kinect", FileStorage::WRITE);
		fs << "K" << K;
		fs << "D" << D;
		fs << "punktyPionowo" << punktyPionowo;
		fs << "punktyPoziomo" << punktyPoziomo;
		fs.release();

		FileStorage fs2("kalibracja_kamera", FileStorage::WRITE);
		fs2 << "K" << K3;
		fs2 << "D" << D3;
		fs2 << "punktyPionowo" << punktyPionowo;
		fs2 << "punktyPoziomo" << punktyPoziomo;
		fs2.release();

	/*	FileStorage fs2("kalibracja_stereo", FileStorage::WRITE);
		fs2 << "K1" << K1;
		fs2 << "K2" << K2;
		fs2 << "D1" << D1;
		fs2 << "D2" << D2;
		fs2 << "R" << R;
		fs2 << "T" << T;
		fs2 << "E" << E;
		fs2 << "F" << F;
		
		fs2.release();*/
		cout << ("Kalibracja Skonczona");
		
		cv::waitKey();
		return 0;
	}
	while (mode == 3)
	{
		Mat image_depth[30], image_kinect[30], image_kamera[30];

		string nazwa = "Zdjecia/Depth/img";
		string nazwa2 = "Zdjecia/Kinect/img";
		string nazwa3 = "Zdjecia/Kamera/img";
		string nazwa4 = "Zdjecia/Rektyf/img";

		Mat K_Kinec, D_Kinec, K_Kamer, D_Kamer, R, T, R1, R2, P1, P2, Q;
		Mat map1, map2;
		FileStorage fs;


		for (int i = 1; i <= 30; i++)
		{
			string s_depth = nazwa + std::to_string(i) + ".jpg";
			image_depth[i - 1] = imread(s_depth);
			string s_kinect = nazwa2 + std::to_string(i) + ".jpg";
			image_kinect[i - 1] = imread(s_kinect);
			string s_kamera = nazwa3 + std::to_string(i) + ".jpg";
			image_kamera[i - 1] = imread(s_kamera);


			int key = cv::waitKey();
			if (key == 27)
				return 0;

		}


		fs.open("kalibracja_kinect", FileStorage::READ);
		fs["K"] >> K_Kinec;
		fs["D"] >> D_Kinec;
		fs.release();


	
		fs.open("kalibracja_kamera", FileStorage::READ);
		fs["K"] >> K_Kamer;
		fs["D"] >> D_Kamer;
		fs.release();

		fs.open("kalibracja_stereo", FileStorage::READ);
		fs["R"] >> R;
		fs["T"] >> T;
		fs.release();

		Mat dst;
		cv::stereoRectify(K_Kamer, D_Kamer, K_Kinec, D_Kinec, image_kinect->size(), R, T, R1, R2, P1, P2, Q);
		cv::initUndistortRectifyMap(K_Kamer,D_Kamer,R1,K_Kinec,image_kamera[1].size(), CV_32FC1,map1,map2);
	
		for (int i = 0; i < 29; i++)
		{
			cv::remap(image_kamera[i], dst, map1, map2, INTER_NEAREST,  BORDER_TRANSPARENT, 0);
			cv::imshow("kamera_undist", dst);
			cv::imshow("kamera original", image_kamera[i]);
			cv::imshow("kinect original", image_kinect[i]);
	

			string s_rektyf = nazwa4 + std::to_string(i) + ".jpg";
			cv::imwrite(s_rektyf, dst );

			int key = cv::waitKey();
			//TODO ZAPISAæ JAKOS TE ZDJECIA ZEBY POTEM POKAZAC BRYMUTOWI I SIE SPYTAC CZY PANDA 3
		}
		int key = cv::waitKey();
		mode = 0;
		if (key == 27)
			return 0;

	}
}
