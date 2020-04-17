#include "encoder_decoder.hpp"

#include <iostream>
using namespace std;
EncoderDecoder::EncoderDecoder(const cv::Mat &im, int quantization_level) : quantization_level(quantization_level) {
    cv::Mat im_yuv = cv::Mat(im.rows, im.cols, im.type());
    cv::cvtColor(im, im_yuv, cv::COLOR_BGR2YCrCb);

    height = im.rows;
    width = im.cols;

    ex_height = height % 8;
    ex_width = width % 8;
    type = im.type();

    cv::Mat new_yuv(height + ex_height, width + ex_width, im_yuv.type());
    if (ex_width != 0 || ex_height != 0) {
        im_yuv.copyTo(new_yuv(cv::Rect(0, 0, width, height)));
        im_yuv = new_yuv;
    }

    width = width + ex_width;
    height = height + ex_height;

    cv::Mat y = cv::Mat(height, width, CV_8U);
    cv::Mat u = cv::Mat(height, width, CV_8U);
    cv::Mat v = cv::Mat(height, width, CV_8U);


    YUVExtractor *extractor = new YUVExtractor();
    extractor->extract_yuv(im_yuv, y, u, v);

    y_block_ext = new BlockExtractor();
    u_block_ext = new BlockExtractor();
    v_block_ext = new BlockExtractor();

    y_block_ext->extract_blocks(y);
    u_block_ext->extract_blocks(u);
    v_block_ext->extract_blocks(v);


    y_block_vector = y_block_ext->get_blocks();
    u_block_vector = u_block_ext->get_blocks();
    v_block_vector = v_block_ext->get_blocks();
}


void EncoderDecoder::encode() {
    DCT *dct = new DCT();
    Quantizer *quantizer = new Quantizer(quantization_level);
    ZigZagScanner *scanner = new ZigZagScanner();
    auto loop_size = y_block_vector->size();
    encode(dct, quantizer, scanner, y_block_vector, y_block_ext, loop_size);
    encode(dct, quantizer, scanner, u_block_vector, u_block_ext, loop_size, false);
    encode(dct, quantizer, scanner, v_block_vector, v_block_ext, loop_size, false);
}


cv::Mat *EncoderDecoder::decode() {
    width = width + ex_width;
    height = height + ex_height;

    DCT *dct = new DCT();
    Quantizer *quantizer = new Quantizer(quantization_level);
    ZigZagScanner *scanner = new ZigZagScanner();

    auto loop_size = y_block_vector->size();
    cv::Mat *decoded_y = decode(dct, quantizer, scanner, y_block_vector, y_block_ext, loop_size);
    cv::Mat *decoded_u = decode(dct, quantizer, scanner, u_block_vector, u_block_ext, loop_size, false);
    cv::Mat *decoded_v = decode(dct, quantizer, scanner, v_block_vector, v_block_ext, loop_size, false);

    std::vector<cv::Mat> mixed_channels;
    mixed_channels.push_back(*decoded_y);
    mixed_channels.push_back(*decoded_u);
    mixed_channels.push_back(*decoded_v);
    cv::Mat yuv_output(height, width, type);

    cv::merge(mixed_channels, yuv_output);

    width = width - ex_width;
    height = height - ex_height;

    cv::Mat new_orig_1(height, width, type);
    if (ex_width != 0 || ex_height != 0) {
        yuv_output(cv::Rect(0, 0, width, height)).copyTo(new_orig_1);
        yuv_output = new_orig_1;
    }

    cv::Mat *bgr_output = new cv::Mat(height, width, type);
    cv::cvtColor(yuv_output, *bgr_output, cv::COLOR_YCrCb2BGR);


    return bgr_output;
}

cv::Mat *EncoderDecoder::encode_decode() {
    cv::Mat *decoded_y = encode_decode(y_block_vector, y_block_ext);
    cv::Mat *decoded_u = encode_decode(u_block_vector, u_block_ext, false);
    cv::Mat *decoded_v = encode_decode(v_block_vector, v_block_ext, false);

    std::vector<cv::Mat> mixed_channels;
    mixed_channels.push_back(*decoded_y);
    mixed_channels.push_back(*decoded_u);
    mixed_channels.push_back(*decoded_v);
    cv::Mat yuv_output(height, width, type);

    cv::merge(mixed_channels, yuv_output);

    width = width - ex_width;
    height = height - ex_height;

    cv::Mat new_orig_1(height, width, type);
    if (ex_width != 0 || ex_height != 0) {
        yuv_output(cv::Rect(0, 0, width, height)).copyTo(new_orig_1);
        yuv_output = new_orig_1;
    }

    cv::Mat *bgr_output = new cv::Mat(height, width, type);
    cv::cvtColor(yuv_output, *bgr_output, cv::COLOR_YCrCb2BGR);

    return bgr_output;
}

void
EncoderDecoder::encode(DCT *dct, Quantizer *quantizer, ZigZagScanner *scanner, std::vector<cv::Mat *> *samples_vector,
                       BlockExtractor *sampler, int loop_size, bool y) {
    sampler->clear_huffman();

    // START ENCODER
    float old_dc = 0;
    float dc = 0;
    float dc_diff = 0;
    // loop over the blocks
    for (int j = 0; j < loop_size; j++) {
        cv::Mat y_input = *(samples_vector->at(j));
        // DCT
        cv::Mat *dct_output = dct->do_block_dct(y_input);
        // Quantization
        cv::Mat *quantized = quantizer->quantize(*dct_output, y);

        // ZigZag scan
        auto scanner_output = scanner->scan(*quantized);
        auto scanner_output_size = scanner_output->size();

        // TODO Apply DPCM for DC components
        // DPCM
        dc = scanner_output->front();
        dc_diff = j==0? dc : dc-old_dc;
        old_dc = dc;

        // RLC
        std::vector<float> acs;
        acs.reserve(scanner_output_size - 1);
        std::copy(scanner_output->begin() + 1, scanner_output->end(), std::back_inserter(acs));

        // Perform Entropy Coding
        std::vector<float> huffman_input;

        huffman_input.push_back(dc_diff);
        huffman_input.push_back(1);

        float prev_ac = acs[0];
        int count = 1;

        for (int i = 1; i < acs.size(); i++) {
            auto curr_ac = acs[i];

            if (curr_ac == prev_ac) {
                count++;
                if (i == acs.size() - 1) {
                    huffman_input.push_back(prev_ac);
                    huffman_input.push_back(count);
                }
            } else {
                if( i == acs.size() -1){
                    huffman_input.push_back(prev_ac);
                    huffman_input.push_back(count);

                    huffman_input.push_back(curr_ac);
                    huffman_input.push_back(count);

                }else{
                    huffman_input.push_back(prev_ac);
                    huffman_input.push_back(count);

                    prev_ac = curr_ac;
                    count = 1;
                }

            }
        }

        Huffman *huff = new Huffman(huffman_input);
        huff->encode();
        sampler->add_huffman(huff);
    }
}


cv::Mat *
EncoderDecoder::decode(DCT *dct, Quantizer *quantizer, ZigZagScanner *scanner, std::vector<cv::Mat *> *samples_vector,
                       BlockExtractor *sampler, int loop_size, bool y) {
    // START DECODER
    cv::Mat *decoded = new cv::Mat(height, width, CV_8U);

    int curr_row = 0;
    int curr_col = 0;
    float prev_dc = 0;
    // loop over the blocks
    for (int i = 0; i < loop_size; i++) {
        Huffman *huf = sampler->get_huffman(i);
        huf->decode();

        auto huffman_output = huf->get_decoded_vector();
        // inverse RLC
        std::vector<float> decoded_vector;
        bool is_dc = true;

        for (int k = 0; k < huffman_output.size(); k += 2) {
            auto ac = huffman_output[k];
            auto cnt = huffman_output[k + 1];

            // TODO Get the DC components of this block
            if (is_dc) {
                is_dc = false;
                if(i != 0){
                    ac = ac + prev_dc;
                    prev_dc = ac;
                }
                else{
                    prev_dc = ac;
                }
            }
            for (int j = 0; j < cnt; j++) {
                decoded_vector.push_back(ac);
            }
        }


        cv::Mat *dequant_input = scanner->descan(decoded_vector);
        cv::Mat *dequantized = quantizer->dequantize(*dequant_input, y);

        cv::Mat *idct_output = dct->do_block_idct(*dequantized);

        (*idct_output).copyTo((*decoded)(cv::Rect(curr_col * 8, curr_row * 8, 8, 8)));
        curr_col++;
        if ((i + 1) % sampler->get_per_row() == 0) {
            curr_row++;
            curr_col = 0;
        }
    }
    return decoded;
}

cv::Mat *
EncoderDecoder::encode_decode(std::vector<cv::Mat *> *samples_vector, BlockExtractor *block_extractor, bool y) {
    DCT *dct = new DCT();
    Quantizer *quantizer = new Quantizer(quantization_level);
    ZigZagScanner *scanner = new ZigZagScanner();
    auto loop_size = samples_vector->size();
    // encoder
    encode(dct, quantizer, scanner, samples_vector, block_extractor, loop_size, y);
    // decoder
    cv::Mat *decoded = decode(dct, quantizer, scanner, samples_vector, block_extractor, loop_size, y);
    return decoded;
}


