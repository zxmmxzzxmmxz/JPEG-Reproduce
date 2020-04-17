#include "dct.hpp"

#include <cmath>
#define MATRIX_LENGTH 8

DCT::DCT() {
    _dct_mat = new cv::Mat(8, 8, CV_32FC1);
    _dct_mat_t = new cv::Mat(8, 8, CV_32FC1);

    // TODO Calculate the 8x8 Matrix to be used for DCT and IDCT
    for(int row =0; row < MATRIX_LENGTH; row++){
        for(int col=0; col < MATRIX_LENGTH; col++){
            _dct_mat->at<float>(row, col) =
                    row != 0
                    ? (sqrt(0.25) * std::cos(PI*(2*col + 1)*row/(MATRIX_LENGTH * 2)) * cos(PI * (col + 0.5)* row/4))
                    : (1/sqrt(MATRIX_LENGTH) * cos(row/4. * PI * (col + 0.5)));
        }
    }

    // TODO transpose _dct_mat and store the result in _dct_mat_t
    int i = 7;
    while(i>=0){
        int j =7;
        while(j>=0){
            _dct_mat_t->at<float>(i,j) = _dct_mat->at<float>(j,i);
            j--;
        }
        i--;
    }
}

DCT::~DCT() {
}

cv::Mat *DCT::do_block_dct(cv::Mat &input) {
    cv::Mat *output_dct = new cv::Mat(8, 8, CV_32FC1);
    cv::Mat input_dct(8, 8, CV_32FC1);
    input.convertTo(input_dct, CV_32FC1);

    // TODO Perform DCT, store result in output_dct and return it.
    cv::Mat result = ((*(_dct_mat)) *(input)) * (*_dct_mat_t);
    result.convertTo(*output_dct, CV_32FC1);
    return output_dct;
}

cv::Mat *DCT::do_block_idct(cv::Mat &input) {
    cv::Mat *output_idct = new cv::Mat(8, 8, CV_32FC1);
    cv::Mat input_dct(8, 8, CV_32FC1);
    input.convertTo(input_dct, CV_32FC1);

    // TODO Perform IDCT and return output_idct
    cv::Mat result = ((*(_dct_mat_t)) *(input)) * (*_dct_mat);
    result.convertTo(*output_idct, CV_32FC1);

    return output_idct;
}

 

