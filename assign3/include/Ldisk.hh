/*
    Author: Shuangchang Li, Qiuan Wu
    I pledge my honor that I have abided by the Stevens Honor System.
    4/25/2018
*/

#ifndef LDISK_H
#define LDISK_H
#include "main.hh"

class Block{
    public:
    Block(int startid, int endid, bool free);
    
    int startid;
    int endid;
    bool free;
    bool full;
    int bytesleft;
    void print();
};

class Bytes{
    public:
    bool free;
    int id;
    Bytes(int id, bool free);
};

class LdiskNode{
    public:
    LdiskNode(std::vector<Block> blocks);
    std::vector<Block> blocks;
    void print();
    void printnode();
};

class Ldisk{
    public:
    Ldisk(std::vector<LdiskNode> nodes);
    Ldisk();
    std::vector<LdiskNode> nodes;
    void print();
    void printinbytes();
};

extern Ldisk disk;

#endif