
#include "OpenNI.h"
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <vector>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/core/core.hpp>

#define C_DEPTH_STREAM 0
#define C_COLOR_STREAM 1
#define C_NUM_STREAMS 2
#define C_MODE_COLOR 0x01
#define C_MODE_DEPTH 0x02
#define C_MODE_ALIGNED 0x04
#define C_STREAM_TIMEOUT 2000

bool m_alignedStreamStatus, m_colorStreamStatus, m_depthStreamStatus;
openni::Status m_status;

int main()
{
	cv::vector<cv::Mat> dataStream;
	m_colorStreamStatus = true;
	m_depthStreamStatus = false;
	m_alignedStreamStatus = false;
	openni::Device m_device;
	openni::VideoStream m_depth, m_color, **m_streams;
	openni::VideoFrameRef m_colorFrame;
	uint64_t m_colorTimeStamp;
	int m_currentStream;
	cv::Mat m_colorImage;

	m_status = openni::STATUS_OK;
	const char* deviceURI = openni::ANY_DEVICE;
	m_status = openni::OpenNI::initialize();
	std::cout << "After initialization: " << std::endl;
	std::cout << openni::OpenNI::getExtendedError() << std::endl;
	m_status = m_device.open(deviceURI);

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
	m_streams = new openni::VideoStream*[C_NUM_STREAMS];
	m_streams[C_COLOR_STREAM] = &m_color;
	m_streams[C_DEPTH_STREAM] = &m_depth;

	char c = ' ';
	while (c != 27) {
		cv::Mat bufferImage;
		cv::vector<cv::Mat> Vec;

		bool colorCaptured = false;
		if (m_colorStreamStatus)
		{
			while (!colorCaptured)
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
					std::cout << "Color Timestamp: " << m_colorTimeStamp << std::endl;
					colorCaptured = true;
					cv::cvtColor(bufferImage, m_colorImage, CV_BGR2RGB);
					break;
				default:
					break;
				}
			}
			Vec.push_back(m_colorImage);
			bufferImage.release();
			dataStream = Vec;
			std::cout << Vec.size() << std::endl;

		}
		else
		{
			m_colorImage = cv::Mat::zeros(10, 10, CV_8UC1);
		}
		dataStream.push_back(m_colorImage);
		bufferImage.release();
		std::cout << dataStream.size() << std::endl;

		cv::imshow("Color", dataStream[C_COLOR_STREAM]);
		c = cv::waitKey(10);
		if (c == '1')
		{
			cv::imwrite("Images/img1.jpg", dataStream[C_COLOR_STREAM]);
			cv::namedWindow("Img1", CV_WINDOW_AUTOSIZE);
			cv::imshow("Img1", dataStream[C_COLOR_STREAM]);

		}
		if (c == '2')
		{
			cv::imwrite("Images/img2.jpg", dataStream[C_COLOR_STREAM]);
			cv::namedWindow("Img2", CV_WINDOW_AUTOSIZE);
			cv::imshow("Img2", dataStream[C_COLOR_STREAM]);
		}
		if (c == '3')
		{
			cv::imwrite("Images/img3.jpg", dataStream[C_COLOR_STREAM]);
			cv::namedWindow("Img3", CV_WINDOW_AUTOSIZE);
			cv::imshow("Img3", dataStream[C_COLOR_STREAM]);
		}
		if (c == '4')
		{
			cv::imwrite("Images/img4.jpg", dataStream[C_COLOR_STREAM]);
			cv::namedWindow("Img4", CV_WINDOW_AUTOSIZE);
			cv::imshow("Img4", dataStream[C_COLOR_STREAM]);
		}
		if (c == '5')
		{
			cv::imwrite("Images/img5.jpg", dataStream[C_COLOR_STREAM]);
			cv::namedWindow("Img5", CV_WINDOW_AUTOSIZE);
			cv::imshow("Img5", dataStream[C_COLOR_STREAM]);
		}
		if (c == '6')
		{
			cv::imwrite("Images/img6.jpg", dataStream[C_COLOR_STREAM]);
			cv::namedWindow("Img6", CV_WINDOW_AUTOSIZE);
			cv::imshow("Img6", dataStream[C_COLOR_STREAM]);
		}
		if (c == '7')
		{
			cv::imwrite("Images/img7.jpg", dataStream[C_COLOR_STREAM]);
			cv::namedWindow("Img7", CV_WINDOW_AUTOSIZE);
			cv::imshow("Img7", dataStream[C_COLOR_STREAM]);
		}
		if (c == '8')
		{
			cv::imwrite("Images/img8.jpg", dataStream[C_COLOR_STREAM]);
			cv::namedWindow("Img8", CV_WINDOW_AUTOSIZE);
			cv::imshow("Img8", dataStream[C_COLOR_STREAM]);
		}
	}	
		m_colorFrame.release();
		m_depth.stop();
		m_color.stop();
		openni::OpenNI::shutdown();
		m_device.close();
}
