#include "huffman.hpp"


Huffman::Huffman(std::vector<float> &input_float) {
    for (auto &v : input_float) {
        input.push_back(std::to_string(v));
    }

    init();
}


Huffman::Huffman(std::vector<std::string> &input) : input(input) {
    init();
}

void Huffman::init() {
    for (auto &v : input) {
        frequencies[v]++;
    }

    INode *root = BuildTree();

    HuffCodeMap codes;
    GenerateCodes(root, HuffCode(), codes);
    delete root;

    for (HuffCodeMap::const_iterator it = codes.begin(); it != codes.end(); ++it) {
        std::stringstream code;
        std::copy(it->second.begin(), it->second.end(), std::ostream_iterator<bool>(code));
        std::string code_str = code.str();
        code_symbol_map[code_str] = it->first;
        symbol_code_map[it->first] = code_str;
    }
}


INode *Huffman::BuildTree() {
    std::priority_queue<INode *, std::vector<INode *>, NodeCmp> trees;

    for (std::map<std::string, int>::iterator iter = frequencies.begin(); iter != frequencies.end(); ++iter) {
        auto symbol = iter->first;
        auto f = iter->second;
        if (f != 0)
            trees.push(new LeafNode(f, symbol));
    }
    while (trees.size() > 1) {
        INode *childR = trees.top();
        trees.pop();

        INode *childL = trees.top();
        trees.pop();

        INode *parent = new InternalNode(childR, childL);
        trees.push(parent);
    }
    return trees.top();
}

void Huffman::GenerateCodes(const INode *node, const HuffCode &prefix, HuffCodeMap &outCodes) {
    if (const LeafNode *lf = dynamic_cast<const LeafNode *>(node)) {
        outCodes[lf->c] = prefix;
    } else if (const InternalNode *in = dynamic_cast<const InternalNode *>(node)) {
        HuffCode leftPrefix = prefix;
        leftPrefix.push_back(false);
        GenerateCodes(in->left, leftPrefix, outCodes);

        HuffCode rightPrefix = prefix;
        rightPrefix.push_back(true);
        GenerateCodes(in->right, rightPrefix, outCodes);
    }
}

void Huffman::encode() {
    encoded_text = "";
    for (auto &v : input) {
        encoded_text += symbol_code_map[v];
    }
}


void Huffman::decode() {
    decoded.clear();
    std::string temp;
    for (int i = 0; i < encoded_text.size(); i++) {
        temp += encoded_text[i];
        std::map<std::string, std::string>::const_iterator it = code_symbol_map.find(temp);
        if (it != code_symbol_map.end()) {
            decoded.push_back(atof(it->second.c_str()));
            temp = "";
        }
    }
}

std::vector<float> Huffman::get_decoded_vector() {
    return decoded;
}


std::string Huffman::get_encoded_text() {
    return encoded_text;
}


