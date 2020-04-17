#include "quantizer.hpp"

#include <cmath>
#include <iostream>
using namespace std;
void Quantizer::set_default() {

    
    // TODO Fill in the Luma Quantization table
    float lQ50[8][8] = {
            {16, 11, 10, 16, 24,  40,  51,  61},
            {12, 12, 14, 19, 26,  58,  60,  55},
            {14, 13, 16, 24, 40,  57,  69,  56},
            {14, 17, 22, 29, 51,  87,  80,  62},
            {18, 22, 37, 56, 68,  109, 103, 77},
            {24, 35, 55, 64, 81,  104, 113, 92},
            {49, 64, 78, 87, 103, 121, 120, 101},
            {72, 92, 95, 98, 112, 100, 103, 99}
    };

    int i = 7;
    while(i>=0){
        int j =7;
        while(j>=0){
            _y_quant_table->at<float>(i,j) = lQ50[i][j];
            j--;
        }
        i--;
    }



    // TODO Fill in Chroma Quantization table
    float cQ50[8][8] = {
            {17, 18, 24, 47, 99, 99, 99, 99},
            {18, 21, 26, 66, 99, 99, 99, 99},
            {24, 26, 56, 99, 99, 99, 99, 99},
            {47, 66, 99, 99, 99, 99, 99, 99},
            {99, 99, 99, 99, 99, 99, 99, 99},
            {99, 99, 99, 99, 99, 99, 99, 99},
            {99, 99, 99, 99, 99, 99, 99, 99},
            {99, 99, 99, 99, 99, 99, 99, 99},
    };

    i = 7;
    while(i>=0){
        int j =7;
        while(j>=0){
            _uv_quant_table->at<float>(i,j) = cQ50[i][j];
            j--;
        }
        i--;
    }

}


void Quantizer::set_constant(int yval, int uv_val) {
    float *y_quant_table_ptr = (float *) _y_quant_table->data;
    float *uv_quant_table_ptr = (float *) _uv_quant_table->data;
    for (int i = 0; i < 64; i++) {
        y_quant_table_ptr[i] = yval;
        uv_quant_table_ptr[i] = uv_val;
    }
}


Quantizer::Quantizer(int quantization_level) : quantization_level(quantization_level) {
    _y_quant_table = new cv::Mat(8, 8, CV_32FC1);
    _uv_quant_table = new cv::Mat(8, 8, CV_32FC1);

    if (quantization_level == 1) {
        set_constant(20, 20);
    } else if (quantization_level == 2) {
        set_constant(100, 100);
    } else if (quantization_level == 3) {
        set_constant(20, 80);
    } else if (quantization_level == 4) {
        set_constant(80, 20);
    } else {
        set_default();
    }
}

cv::Mat *Quantizer::quantize(cv::Mat &input, bool y) {
    cv::Mat *quantized = new cv::Mat(8, 8, CV_32FC1);
    float *quantized_ptr = (float *) quantized->data;

    cv::Mat *quantized_round = new cv::Mat(8, 8, CV_32FC1);


    // TODO Perform Quantization, store result in quantized_round

    //  if y is true -> use Luma quantization table
    if(y){
        for(int row = 0; row < 8; row++){
            for(int col = 0; col < 8; col++){
                quantized->at<float>(row,col) = std::round(input.at<float>(row,col) / _y_quant_table->at<float>(row,col));
            }
            row = row;
        }
    }

    //  if y is false -> use Chroma quantization table
    else{
        for(int row = 0; row < 8; row++){
            for(int col = 0; col < 8; col++){
                quantized->at<float>(row,col) = std::round(input.at<float>(row,col) / _uv_quant_table->at<float>(row,col));
            }
        }
    }
    // Quantization
    quantized->convertTo(*quantized_round, CV_32FC1);
    return quantized_round;
}

cv::Mat *Quantizer::dequantize(cv::Mat &input, bool y) {
    auto *dequantized = new cv::Mat(8, 8, CV_32FC1);
    // TODO Perform Dequantization, store result in dequantized

    *dequantized = y? input.mul(*_y_quant_table) : input.mul(*_uv_quant_table) ;

    return dequantized;
}
