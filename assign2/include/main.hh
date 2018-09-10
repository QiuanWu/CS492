#ifndef _ASSIGN2_HH_
#define _ASSIGN2_HH_
#define MEMORY 512
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <math.h>
#include <climits>

using namespace std;
// Page attributes
class page
{
  public:
    int page_number;
    int valid_bit;
    unsigned long last_time_accessed;
    int referenced;
};

// page table attributes
class page_table
{
  public:
    int p_id;
    int p_memory;
    std::vector<page *> table;
    int numofPages;

};

// Global attributes
typedef struct global
{
    std::vector<page *> physicalPages;
    int numPhysicalPages;
    int pagesLoaded;
    unsigned long pageFaults;
    int clockIndex;
} global;

extern global glob_env;
#endif
