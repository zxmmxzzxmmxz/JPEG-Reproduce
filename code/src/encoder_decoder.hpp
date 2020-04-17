#ifndef _H_ENCODER_DECODER_
#define _H_ENCODER_DECODER_

#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <cmath>
#include <iterator>
#include <algorithm>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "yuv_extractor.hpp"
#include "block_extractor.hpp"
#include "dct.hpp"
#include "quantizer.hpp"
#include "zigzag_scanner.hpp"
#include "huffman.hpp"


class EncoderDecoder {
private:
    BlockExtractor *y_block_ext;
    BlockExtractor *u_block_ext;
    BlockExtractor *v_block_ext;

    std::vector<cv::Mat *> *y_block_vector;
    std::vector<cv::Mat *> *u_block_vector;
    std::vector<cv::Mat *> *v_block_vector;

    int height, width, type;
    int ex_height, ex_width;

    int quantization_level;

    cv::Mat *encode_decode(std::vector<cv::Mat *> *samples_vector, BlockExtractor *block_extractor, bool y = true);

    void encode(DCT *dct, Quantizer *quantizer, ZigZagScanner *scanner, std::vector<cv::Mat *> *samples_vector,
                BlockExtractor *sampler, int loop_size, bool y = true);

    cv::Mat *decode(DCT *dct, Quantizer *quantizer, ZigZagScanner *scanner, std::vector<cv::Mat *> *samples_vector,
                    BlockExtractor *sampler, int loop_size, bool y = true);
public:
    EncoderDecoder(const cv::Mat &im, int quantization_level = 0);

    ~EncoderDecoder();

    cv::Mat *encode_decode();

    cv::Mat *decode();

    void encode();
};

#endif
