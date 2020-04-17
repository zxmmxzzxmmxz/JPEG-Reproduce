#ifndef _H_YUV_EXTRACTOR
#define _H_YUV_EXTRACTOR

#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <cmath>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

class YUVExtractor {
public:
    YUVExtractor();

    ~YUVExtractor();

    void extract_yuv(const cv::Mat &input, const cv::Mat &y, const cv::Mat &u, const cv::Mat &v);
};

#endif
