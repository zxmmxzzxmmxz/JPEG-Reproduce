#ifndef _H_QUANTIZER_
#define _H_QUANTIZER_

#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <cmath>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


class Quantizer {
public :
    // Luma quantization table
    cv::Mat *_y_quant_table;
    // Chrome quantization table
    cv::Mat *_uv_quant_table;

    int quantization_level;
public:
    Quantizer(int quantization_level = 0);

    ~Quantizer();

    cv::Mat *quantize(cv::Mat &input, bool y = true);

    cv::Mat *dequantize(cv::Mat &input, bool y = true);

    void set_default();

    void set_constant(int yval, int uv_val);

};

#endif
