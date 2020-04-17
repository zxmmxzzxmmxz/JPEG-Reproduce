#include "block_extractor.hpp"
#include <iostream>

using namespace std;

BlockExtractor::BlockExtractor() {
    _block_vector = new std::vector<cv::Mat *>();
    _huf_vector = new std::vector<Huffman *>();
    _per_row = 0;
    _per_col = 0;
    _samples_number = 0;
}

BlockExtractor::~BlockExtractor() {
}

void BlockExtractor::add_huffman(Huffman *huf) {
    _huf_vector->push_back(huf);
}

Huffman *BlockExtractor::get_huffman(int index) {
    return _huf_vector->at(index);
}

void BlockExtractor::clear_huffman() {
    _huf_vector->clear();
}

void BlockExtractor::extract_blocks(const cv::Mat &mat) {
    _per_row = mat.cols / 8;
    _per_col = mat.rows / 8;

    _samples_number = _per_row * _per_col;

    // TODO Extract 8x8 blocks from mat, and push every block to _block_vector
    for(int row = 0; row < _per_row; row++){
        for(int col = 0; col < _per_col; col++){
            auto *block = new cv::Mat(8, 8, CV_32FC1);
            cv::Mat _cur_sub = mat(cv::Range(row * 8, row * 8 + 8), cv::Range(col * 8, col * 8 + 8));
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                    if(!std::isnan(_cur_sub.at<float>(i,j))){
                        block->at<float>(i, j) =_cur_sub.at<unsigned char>(i, j);
                    }else{
                        block->at<float>(i, j) =_cur_sub.at<unsigned char>(i, j - 1);
                    }
                }
            }
            _block_vector->push_back(block);
        }
    }
}

std::vector<cv::Mat *> *BlockExtractor::get_blocks() {
    return _block_vector;
}

int BlockExtractor::get_per_row() const {
    return _per_row;
}

int BlockExtractor::get_per_col() const {
    return _per_col;
}
