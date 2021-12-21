#pragma once

#include <string>
#include <windows.h>
#include <opencv2/opencv.hpp>

using namespace std;


class WindowCapture
{
	private:
		HWND hwnd;
		string window_name;

		void setup_bitmap(BITMAPINFOHEADER* bi, int width, int height);

	public:
		WindowCapture();
		WindowCapture(string window_name);

		/// Methods
		cv::Mat get_video_source();

		// Getters
		HWND get_hwnd();
};