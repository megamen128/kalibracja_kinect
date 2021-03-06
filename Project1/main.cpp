
#include "OpenNI.h"
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <vector>
#include <string>

using namespace cv;
using namespace std;
#define C_DEPTH_STREAM 0
#define C_COLOR_STREAM 1
#define C_STREAM_TIMEOUT 2000
int mode=0;
bool m_alignedStreamStatus, m_colorStreamStatus, m_depthStreamStatus;
openni::Status m_status;
const openni::DepthPixel *depthPixel[30];

int main()
{
	
	while (mode == 0) {
		int wybor=0;
		std::cout << "Wpisz odpowiedni numer" << std::endl;
		std::cout << "1 - Zapisz zdjecia z kamery kinecta oraz zwyklej kamery" << std::endl;
		std::cout << "2 - Kalibracja zapisanych zdjec" << std::endl;
		std::cout << "3 - Rektyfikacja zdjec z kamery oddzielnej do kamery kinecta" << std::endl;

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
			const openni::DepthPixel* pDepthRow;
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
		m_streams = new openni::VideoStream*[2];
		m_streams[C_COLOR_STREAM] = &m_color;
		m_streams[C_DEPTH_STREAM] = &m_depth;


		char c = ' ';
		while (c != 27) {
			cv::Mat bufferImage;
			cv::vector<cv::Mat> Vec;

			Mat frame;
			cap >> frame; // get a new frame from camera
			cv::flip(frame, frame, 1);

			cv::imshow("Color2", frame);
		

			bool colorCaptured = false;
			bool depthCaptured = false;
			if (m_colorStreamStatus & m_depthStreamStatus)
			{
				while (!colorCaptured || ! depthCaptured || m_depthTimeStamp != m_colorTimeStamp)
				{
					m_status = openni::OpenNI::waitForAnyStream(m_streams, 2, &m_currentStream, C_STREAM_TIMEOUT);
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
						 pDepthRow = (const openni::DepthPixel*)m_depthFrame.getData();
						break;
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
	
			cv::imshow("Color", dataStream[C_COLOR_STREAM]);
			cv::imshow("Depth", dataStream[C_DEPTH_STREAM]);
			int x, y;
			//openni::CoordinateConverter::convertDepthToColor(m_depth, m_color, 0, 0, 0, &x, &y);
			//std::cout << x << y;
			string nazwa_kin = "Zdjecia/Kinect/img";
			string nazwa_dep = "Zdjecia/Depth/img";
			string nazwa_kol = "Zdjecia/Kamera/img";

			c = cv::waitKey(10);
			if(i<=30 && (c==83 || c==115) )
			{
					
				
					string temp_kin = nazwa_kin + std::to_string(i) + ".jpg";
					string temp_dep = nazwa_dep + std::to_string(i) + ".xml";
					string temp_kol = nazwa_kol + std::to_string(i) + ".jpg";
					printf("Zapisano nr %d\n",i);

					cv::FileStorage fp(temp_dep, cv::FileStorage::WRITE);
					fp << "dframe" << dataStream[C_DEPTH_STREAM];
					fp.release();

					cv::imshow("Img_Kinect", dataStream[C_COLOR_STREAM]);
					cv::imshow("Img_Kamera", frame);
					cv::imshow("Img_Depth", dataStream[C_DEPTH_STREAM]);

					cv::imwrite(temp_kin, dataStream[C_COLOR_STREAM]);
				//	cv::imwrite(temp_dep, dataStream[C_DEPTH_STREAM]);
					cv::imwrite(temp_kol, frame);

				//	openni::CoordinateConverter::convertDepthToColor(m_depth,m_color,dataStream[0].)
					i++;
			}
		}

		//FileStorage fs("kalibracja_kinect", FileStorage::WRITE);
		//fs << "DPixel" << depthPixel;

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
		Mat image[30], image_depth[30], image_kamera[30];
		for (int i = 1; i <=30; i++)
		{
			string s = nazwa + std::to_string(i) + ".jpg";
			image[i-1] = imread(s);
			string s_depth = nazwa2 + std::to_string(i) + ".xml";
			//image_depth[i - 1] = imread(s_depth);

			cv::FileStorage fs(s_depth, cv::FileStorage::READ);
			fs["dframe"] >> image_depth[i-1];

			string s_kamera = nazwa3 + std::to_string(i) + ".jpg";
			image_kamera[i - 1] = imread(s_kamera);

		}

		for (int i =0; i<30; i++ )
		{
			Mat gray_image, gray_imaged, gray_image_kamera;
			cvtColor(image[i], gray_image, CV_BGR2GRAY);
			gray_imaged = image_depth[i];
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
			if ( found != 0  && found2 != 0 )
			{
				cout << ("Przegladanie wyznaczonych punktow, Wcisnij dowolny klawisz")<<endl;

 				Punkty_obrazu.push_back(katy);
				Punkty_obiektu.push_back(obj);
				Punkty_obrazu_kamera.push_back(katy_kamera);


				successes++;
			}

		}
		
		double rms;
		cv::destroyAllWindows();

		cout << "Aby anulowac kalibracje wcisnij ESC" << endl << "Wcisniecie innego klawisza rozpoczyna kalibracje" << endl;
		int key = waitKey(0);
		if (key == 27)
			return 0;
		else
		cv::destroyAllWindows();
		cout << ("Kalibracja kamery kinecta...") << endl;
		rms = cv::calibrateCamera(Punkty_obiektu, Punkty_obrazu, image[1].size(), K, D, rvecs, tvecs); //kalibracja kamery kolorowej kinecta	
		cout << "Skalibrowano z b��dem RMS =" << rms << endl;

		cout << ("Kalibracja kamery PS Eye...") << endl;
		rms = cv::calibrateCamera(Punkty_obiektu, Punkty_obrazu_kamera, image[1].size(), K3, D3, rvecs2, tvecs2);  //kalibracja oddzielnej kamery
		cout << "Skalibrowano z b��dem RMS =" << rms << endl;

		Mat H1, H2;
		Mat K1, D1, K2, D2, R, T, E,F;

		cout << ("Kalibracja stereo obydwu kamer...") << endl;
		rms = cv::stereoCalibrate(Punkty_obiektu, Punkty_obrazu, Punkty_obrazu_kamera, K, D, K3, D3, image_kamera[1].size(), R, T, E, F,
			TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 1e-6),
			CV_CALIB_FIX_INTRINSIC);
		cout << "Skalibrowano z b��dem RMS =" << rms << endl;

		FileStorage fs("kalibracja_kinect", FileStorage::WRITE);
		fs << "K" << K;
		fs << "D" << D;
		fs.release();

		FileStorage fs2("kalibracja_kamera", FileStorage::WRITE);
		fs2 << "K" << K3;
		fs2 << "D" << D3;

		fs2.release();

		FileStorage fs3("kalibracja_stereo", FileStorage::WRITE);
		fs3 << "R" << R;
		fs3 << "T" << T;

		//fs3 << "P1" << P1;
		//fs3 << "P2" << P2;
	
		fs3.release();
		cout << ("Kalibracja Skonczona");
		
		

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
			string s_depth = nazwa + std::to_string(i) + ".xml";

			cv::FileStorage fs(s_depth, cv::FileStorage::READ);
			fs["dframe"] >> image_depth[i - 1];


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

		Rect validRoi[2];
		Mat dst;
		cv::stereoRectify(K_Kamer, D_Kamer, K_Kinec, D_Kinec, image_kamera[1].size(), R, T, R1, R2, P1, P2, Q,
		0, 1, image_kamera[1].size(), &validRoi[0], &validRoi[1]);


		cv::initUndistortRectifyMap(K_Kamer,D_Kamer,R1,P1,image_kamera[1].size(), CV_32FC1,map1,map2);
	
		for (int i = 0; i < 30; i++)
		{
			cv::remap(image_kamera[i], dst, map1, map2, cv::INTER_LINEAR);
			cv::imshow("kamera_undist", dst);
			cv::imshow("kamera original", image_kamera[i]);
			cv::imshow("kinect original", image_kinect[i]);
			cv::imshow("kinect depth", image_depth[i]);

	

			string s_rektyf = nazwa4 + std::to_string(i) + ".jpg";
			cv::imwrite(s_rektyf, dst );

			if (cv::waitKey() == 27) return 0;
		}

	}
}
