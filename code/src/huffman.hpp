#ifndef _H_HUFFMAN_
#define _H_HUFFMAN_

#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <cmath>
#include <climits>
#include <queue>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>

typedef std::vector<bool> HuffCode;
typedef std::map<std::string, HuffCode> HuffCodeMap;

class INode {
public:
    const int f;

    virtual ~INode() {}

protected:
    INode(int f) : f(f) {}
};

class InternalNode : public INode {
public:
    INode *const left;
    INode *const right;

    InternalNode(INode *c0, INode *c1) : INode(c0->f + c1->f), left(c0), right(c1) {}

    ~InternalNode() {
        delete left;
        delete right;
    }
};

class LeafNode : public INode {
public:
    const std::string c;

    LeafNode(int f, const std::string &c) : INode(f), c(c) {}
};

struct NodeCmp {
    bool operator()(const INode *lhs, const INode *rhs) const { return lhs->f > rhs->f; }
};

class Huffman {
private:
    std::map<std::string, int> frequencies;
    std::map<std::string, std::string> code_symbol_map;
    std::map<std::string, std::string> symbol_code_map;

    std::vector<std::string> input;
    std::vector<float> decoded;

    std::string encoded_text;

    void init();

public:
    Huffman(std::vector<float> &input_float);

    Huffman(std::vector<std::string> &input);

    ~Huffman();

    INode *BuildTree();

    void GenerateCodes(const INode *node, const HuffCode &prefix, HuffCodeMap &outCodes);

    void encode();

    void decode();

    std::vector<float> get_decoded_vector();

    std::string get_encoded_text();
};

#endif
