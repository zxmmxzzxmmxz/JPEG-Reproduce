#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <queue>
#include <sstream>
#include <iterator>
#include <algorithm>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "encoder_decoder.hpp"

#define PI 3.14159265

using namespace std;

bool save_image(const std::string &image_name, cv::Mat &image) {
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    std::string full_path = image_name;
    return imwrite(full_path, image);
}

/*
 * The entry point of the program.
 * It takes two inputs:
 * 1- Input image: full path to an input image to be encoded
 * 2- Quantization level: 0, 1, 2, 3, or 4
 */
int main(int argc, char **argv) {
    std::string file_name;

    // 0: default
    // 1: const_low
    // 2: const_high
    // 3: low_luma
    // 4: low_chroma
    int quantization_level = 0;

    if (argc == 2) {
        file_name = argv[1];
    } else if (argc == 3) {
        file_name = argv[1];
        quantization_level = atoi(argv[2]);
    } else {
        std::cout << "This operation is not supported" << std::endl;
        return 1;
    }

    cv::Mat original_image = cv::imread(file_name);
    int im_width = original_image.cols;
    int im_height = original_image.rows;

    int ex_width = im_width % 8;
    int ex_height = im_height % 8;

    cv::Mat new_orig(im_height + ex_height, im_width + ex_width, original_image.type());
    if (ex_width != 0 || ex_height != 0) {
        original_image.copyTo(new_orig(cv::Rect(0, 0, im_width, im_height)));
        original_image = new_orig;
    }

    im_width = im_width + ex_width;
    im_height = im_height + ex_height;

    // Start encoder
    cv::Mat orig_ds_4(im_height / 4, im_width / 4, original_image.type());
    cv::resize(original_image, orig_ds_4, orig_ds_4.size());
    EncoderDecoder *codec_4 = new EncoderDecoder(orig_ds_4, quantization_level);
    cv::Mat *codec_4_output = codec_4->encode_decode();
    cv::Mat codec_4_us_2(codec_4_output->rows * 2, codec_4_output->cols * 2, codec_4_output->type());
    cv::resize(*codec_4_output, codec_4_us_2, codec_4_us_2.size());
    cv::Mat orig_ds_2(im_height / 2, im_width / 2, original_image.type());
    cv::resize(original_image, orig_ds_2, orig_ds_2.size());
    cv::Mat subtract_4_2 = orig_ds_2 - codec_4_us_2;

    EncoderDecoder *codec_2 = new EncoderDecoder(subtract_4_2, quantization_level);
    cv::Mat *codec_2_output = codec_2->encode_decode();

    cv::Mat sum_4_2 = codec_4_us_2 + *codec_2_output;

    cv::Mat codec_2_us_2(sum_4_2.rows * 2, sum_4_2.cols * 2, sum_4_2.type());
    cv::resize(sum_4_2, codec_2_us_2, codec_2_us_2.size());

    cv::Mat subtract_1_2 = original_image - codec_2_us_2;
    
    EncoderDecoder *codec_1 = new EncoderDecoder(subtract_1_2, quantization_level);
    codec_1->encode();
    // Finish Encoder

    // I4 codec_4_final_output
    cv::Mat *codec_4_final = codec_4->decode();
    cv::Mat codec_4_final_output = *codec_4_final;

    // I2 codec_2_final_output
    cv::Mat codec_4_us_2_(codec_4_final->rows * 2, codec_4_final->cols * 2, codec_4_final->type());
    cv::resize(*codec_4_final, codec_4_us_2_, codec_4_us_2_.size());

    cv::Mat *codec_2_final = codec_2->decode();
    cv::Mat codec_2_final_output = *codec_2_final + codec_4_us_2_;

    // I1 codec_1_final_output
    cv::Mat codec_2_us_2_(codec_2_final_output.rows * 2, codec_2_final_output.cols * 2, codec_2_final_output.type());
    cv::resize(codec_2_final_output, codec_2_us_2_, codec_2_us_2_.size());

    cv::Mat *codec_1_final = codec_1->decode();
    cv::Mat codec_1_final_output = *codec_1_final + codec_2_us_2_;

    // End Decoder

    im_width = im_width - ex_width;
    im_height = im_height - ex_height;

    // Show the three images
    cv::Mat new_orig_1(im_height, im_width, original_image.type());
    cv::Mat new_orig_2(im_height / 2, im_width / 2, original_image.type());
    cv::Mat new_orig_4(im_height / 4, im_width / 4, original_image.type());
    if (ex_width != 0 || ex_height != 0) {
        codec_1_final_output(cv::Rect(0, 0, im_width, im_height)).copyTo(new_orig_1);
        codec_2_final_output(cv::Rect(0, 0, im_width / 2, im_height / 2)).copyTo(new_orig_2);
        codec_4_final_output(cv::Rect(0, 0, im_width / 4, im_height / 4)).copyTo(new_orig_4);

        codec_1_final_output = new_orig_1;
        codec_2_final_output = new_orig_2;
        codec_4_final_output = new_orig_4;
    }

    cv::namedWindow("I4", cv::WINDOW_AUTOSIZE);
    cv::imshow("I4", codec_4_final_output);

    cv::namedWindow("I2", cv::WINDOW_AUTOSIZE);
    cv::imshow("I2", codec_2_final_output);

    cv::namedWindow("I1", cv::WINDOW_AUTOSIZE);
    cv::imshow("I1", codec_1_final_output);

    // Uncomment the following if you need to save the image to disk
    // save_image(std::to_string(quantization_level) + ".jpg", codec_1_final_output);

    cv::waitKey(0);
    return 0;
}

