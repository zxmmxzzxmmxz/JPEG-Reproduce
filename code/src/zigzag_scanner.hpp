#ifndef _H_ZIGZAG_SCANNER_
#define _H_ZIGZAG_SCANNER_

#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <cmath>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

class ZigZagScanner {
private:
    std::map<int, std::tuple<int, int>> *_scan_map;

public:
    ZigZagScanner();

    ~ZigZagScanner();

    std::vector<float> *scan(cv::Mat &input);

    cv::Mat *descan(const std::vector<float> &input);
};


#endif
