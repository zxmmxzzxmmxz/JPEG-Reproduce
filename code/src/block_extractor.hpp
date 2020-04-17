#ifndef _H_SAMPLER
#define _H_SAMPLER

#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <cmath>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "huffman.hpp"

class BlockExtractor {

private:
    std::vector<cv::Mat *> *_block_vector;
    std::vector<Huffman *> *_huf_vector;
    int _per_row;
    int _per_col;
    int _samples_number;
public:
    BlockExtractor();

    ~BlockExtractor();

    // Extracts an 8x8 block from mat, and pushes the samples to _block_vector
    void extract_blocks(const cv::Mat &mat);

    std::vector<cv::Mat *> *get_blocks();

    int get_per_row() const;

    int get_per_col() const;

    void add_huffman(Huffman *huf);

    Huffman *get_huffman(int index);

    void clear_huffman();
};

#endif
