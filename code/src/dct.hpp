#ifndef _H_DCT_
#define _H_DCT_

#define PI 3.14159265

#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <cmath>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

class DCT {
private:
    cv::Mat *_dct_mat;
    cv::Mat *_dct_mat_t;

    void *get_2D_matrix(float *ptr);
public:
    DCT();

    ~DCT();

    cv::Mat *do_block_dct(cv::Mat &input);

    cv::Mat *do_block_idct(cv::Mat &input);

};

#endif
