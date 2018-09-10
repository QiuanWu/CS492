/*
    Author: Shuangchang Li, Qiuan Wu
    I pledge my honor that I have abided by the Stevens Honor System.
    4/25/2018
*/
#ifndef _MAIN_HH
#define _MAIN_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <list>
#include <iterator>
#include <cmath>
#include <stdlib.h>
#include <algorithm>
#include <cstddef>
#include "Ldisk.hh"

using namespace std;

class Node{
    public:
    string nodename;
    int size;
    time_t time_stamp;
    bool isFile;
    int used_size;
    //linded list of block address
    std::vector<int> blockAddress;
    Node(string nodename, int size, bool isFile);
    void print();
    void printLfile();
    std::vector<int> blockAddresses;
};

class Tree{    
    public:
    Tree *parent;
    std::vector<Tree *> children;
    string nodename;
    Node *node;
    Tree(Node *node);
    Tree();
    void addChild(Tree *child);
    bool hasChildren();
    void setParent(Tree *parent);
    bool hasParent();
    Tree *getParent();
    void print(Tree* start);
    string getnodename();
};

Tree* removeChild(Tree* parent, Tree* child);
Tree *getChild(Tree* root, string nodename);

typedef struct glob_env{
    Tree* Root;
    Tree * cur_dir;
    int disk_size;
    int block_size;

} glob_env;

void addfile(Node *node);
void deleteFile(Node *node);
void removeFile(Node *node);
extern glob_env global;
#endif