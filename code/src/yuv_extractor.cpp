#include "yuv_extractor.hpp"

YUVExtractor::YUVExtractor() {
}

YUVExtractor::~YUVExtractor() {
}

void YUVExtractor::extract_yuv(const cv::Mat &input, const cv::Mat &y, const cv::Mat &u, const cv::Mat &v) {
    uint8_t *pixelPtr = (uint8_t *) input.data;
    int cn = input.channels();
    for (int i = 0; i < input.rows; i++) {
        for (int j = 0; j < input.cols; j++) {
            cv::Scalar_<uint8_t> yuvPixel;
            yuvPixel.val[0] = pixelPtr[i * input.cols * cn + j * cn + 0]; // Y
            yuvPixel.val[1] = pixelPtr[i * input.cols * cn + j * cn + 1]; // U
            yuvPixel.val[2] = pixelPtr[i * input.cols * cn + j * cn + 2]; // V

            y.data[i * input.cols + j] = (int) yuvPixel.val[0];
            u.data[i * input.cols + j] = (int) yuvPixel.val[1];
            v.data[i * input.cols + j] = (int) yuvPixel.val[2];
        }
    }
}
