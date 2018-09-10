/*
 * CS492 Assign2
 * Qiuan Wu & Shuangchang Li
 * 
 * I pledge my honor that I have abided by the Stevens Honor System
 * 
 */

#include "../include/main.hh"

global glob_env;

bool power_of_two(int n){
    return (n & (n-1)) == 0;
}

int main(int argc, char **argv)
{
    char *plist;
    char *ptrace;
    int pageSize;
    char *algorithm;
    bool prePaging;
    vector<page_table *> processes;

    // Input error checking: number of arguments, page size, prepaging
    if (argc != 6)
    {
        cerr << "Usage: ./filename <path to plist.txt> <path to ptrace.txt> pageSize algorithm prePaging" << endl;
        return 1;
    }

    plist = argv[1];
    ptrace = argv[2];
    pageSize = atoi(argv[3]);
    algorithm = argv[4];

    // Check for whether the page size is the power of 2
    // Whether the page size is within limites
    if (pageSize < 1 || pageSize > 32 || !power_of_two(pageSize))
    {
        cerr << "Error: Invalid page size!" << endl;
        return 1;
    }
    if (!strcmp(argv[5], "+"))
    {
        prePaging = 1;
    }
    else if (!strcmp(argv[5], "-"))
    {
        prePaging = 0;
    }
    else
    {
        cerr << "Error: Invalid prepaging input!" << endl;
        return 1;
    }

    // Read in plist and pair elements, put all processes into a vector of "page_table" named processes
    int processCount = 0;
    string line;
    ifstream inPlist;
    inPlist.open(plist);
    if (!inPlist)
    {
        cout << "Cannot open plist file." << endl;
        return 1;
    }
    while (getline(inPlist, line))
    {
        if (line.size() != 0)
        {
            string processID = line.substr(0, line.find(' '));
            string addressSpace = line.substr(line.find(' ') + 1);
            page_table *pt = new page_table();
            pt->p_id = atoi(processID.c_str());
            pt->p_memory = atoi(addressSpace.c_str());
            processes.push_back(pt);
            processCount++;
            //cout << processID << endl;
        }
    }

    // Create physical memory page
    glob_env.numPhysicalPages = MEMORY / pageSize;

    int memLocationsEachProcess = MEMORY / processCount;
    int pagesToLoad = memLocationsEachProcess / pageSize;
    glob_env.pagesLoaded = 0;

    // Calculate number of pages for each process and initialize bit
    int pageID = 0;
    for (int i = 0; i < processCount; i++)
    {
        processes[i]->numofPages = ceil(processes[i]->p_memory / (pageSize * 1.0));
        for (int j = 0; j < (processes[i]->numofPages); j++)
        {
            page *currPage = new page();
            currPage->valid_bit = 0;
            // Record the page id
            currPage->page_number = pageID;
            currPage->referenced = 0;
            processes[i]->table.push_back(currPage);
            ++pageID;

            // Load the pages
            if (j < pagesToLoad)
            {
                // If the page is loaded, its valid bit is set to 1
                // Its access time is set to 0
                // Push the page into physical pages
                processes[i]->table[j]->last_time_accessed = 0;
                processes[i]->table[j]->valid_bit = 1;
                glob_env.physicalPages.push_back(processes[i]->table[j]);
                glob_env.pagesLoaded++;
            }
        }
        pageID = 0;
    }

    // Read in ptrace and pair elements
    int requestCount = 0;
    ifstream inPtrace;
    inPtrace.open(ptrace);
    if (!inPtrace)
    {
        cout << "Cannot open ptrace file." << endl;
        return 1;
    }

    int pid;
    int addressSpaceInt;
    unsigned long accessTime = 0;
    glob_env.pageFaults = 0;
    while (getline(inPtrace, line))
    {
        int nextPageIndex;
        if (line.size() != 0)
        {
            accessTime++;
            string processID = line.substr(0, line.find(' '));
            string addressSpace = line.substr(line.find(' ') + 1);
            pid = atoi(processID.c_str());
            addressSpaceInt = atoi(addressSpace.c_str());
            //cout << "pid: " << pid << " address space: " << addressSpaceInt << endl;
            // cout << "pid: " << pid << " address space: " << addressSpaceInt << endl;
            // Get the page index of the current process address space
            page *currPage = new page();
            currPage->page_number = ceil(addressSpaceInt / (pageSize * 1.0)) - 1;
            //cout << "page_number check: " << currPage->page_number << endl;
            //cout << "Nextpage " << processes[pid]->table[currPage->page_number]->page_number << endl;

            // Set the time accessed and reference

            // cout << "currpage page number " << currPage->page_number<< endl;
            int i = currPage->page_number;

            // cout << "i: " << i << endl;
            // cout << "accessTime " << accessTime<< endl;
            // cout << "table size: " << processes[pid]->table.size() << endl;

            processes[pid]->table[i]->last_time_accessed = accessTime;

            // If the page is already loaded in the physical memory, its valid bit is 1
            // If the page is not loaded, page fault. Run algorithms
            if (processes[pid]->table[i]->valid_bit == 0)
            {
                processes[pid]->table[i]->valid_bit = 1;
                nextPageIndex = processes[pid]->table[i]->page_number;

                if (nextPageIndex == processes[pid]->table.size())
                {
                    nextPageIndex = 0;
                }

                // cout << "pid " << pid;
                // cout << ", next index page: " << nextPageIndex << endl;
                // cout << "next page number:" << processes[pid]->table[nextPageIndex]->page_number << endl;
                // cout << "valid-bit: " << processes[pid]->table[nextPageIndex]->valid_bit << endl;
                // cout << "next page Index: " << nextPageIndex << endl;
                // cout << "table size: " << processes[pid]->table.size() << endl << endl << endl;

                while (processes[pid]->table[nextPageIndex]->valid_bit)
                {
                    // If there is no page that is invalid after clocking back to the currPage
                    if (nextPageIndex == processes[pid]->table.size())
                    {
                        nextPageIndex = -1;

                        // cout << "next page index has become -1"<< endl;
                        break;
                    }
                    // If the next page index comes to the end of page, set it back to the beginning
                    if (nextPageIndex + 1 == processes[pid]->table.size())
                    {
                        nextPageIndex = 0;
                    }
                    else
                    {
                        nextPageIndex++;
                    }
                }

                page *page = processes[pid]->table[i];
                if (!strcmp(algorithm, "FIFO"))
                {
                    // cout << "AL pid " << pid<<endl;
                    // cout << "AL page_number " << page->page_number << endl;
                    if (glob_env.pagesLoaded < glob_env.numPhysicalPages)
                    {
                        glob_env.physicalPages.push_back(page);
                        glob_env.pagesLoaded++;
                    }
                    // If there is no empty slot
                    else
                    {

                        // Find the removed page in page table
                        for (int i = 0; i < processes.size(); i++)
                        {
                            for (int j = 0; j < processes[i]->table.size(); j++)
                            {
                                if (processes[i]->table[j] == glob_env.physicalPages.front())
                                {
                                    processes[i]->table[j]->valid_bit = 0;
                                }
                            }
                        }
                        // Remove the first page
                        glob_env.physicalPages.erase(glob_env.physicalPages.begin());
                        glob_env.physicalPages.push_back(page);
                    }

                    // If prepaging
                    if (prePaging && nextPageIndex != -1)
                    {
                        page = processes[pid]->table[nextPageIndex];
                        processes[pid]->table[nextPageIndex]->valid_bit = 1;
                        if (glob_env.pagesLoaded < glob_env.numPhysicalPages)
                        {
                            glob_env.physicalPages.push_back(page);
                            glob_env.pagesLoaded++;
                        }
                        // If there is no empty slot
                        else
                        {

                            // Find the removed page in page table
                            for (int i = 0; i < processes.size(); i++)
                            {
                                for (int j = 0; j < processes[i]->table.size(); j++)
                                {
                                    if (processes[i]->table[j] == glob_env.physicalPages.front())
                                    {
                                        processes[i]->table[j]->valid_bit = 0;
                                    }
                                }
                            }
                            // Remove the first page
                            glob_env.physicalPages.erase(glob_env.physicalPages.begin());
                            glob_env.physicalPages.push_back(page);
                        }
                    }
                }
                if (!strcmp(algorithm, "LRU"))
                {
                    int index;
                    unsigned long minIndex_Time = LONG_MAX;
                    int minIndex;
                    for (index = 0; index < glob_env.physicalPages.size(); index++)
                    {
                        if (glob_env.physicalPages[index]->last_time_accessed < minIndex_Time)
                        {
                            minIndex_Time = glob_env.physicalPages[index]->last_time_accessed;
                            minIndex = index;
                        }
                    }

                    for (int i = 0; i < processes.size(); i++)
                    {
                        for (int j = 0; j < processes[i]->table.size(); j++)
                        {
                            if (processes[i]->table[j] == glob_env.physicalPages[minIndex])
                                processes[i]->table[j]->valid_bit = 0;
                        }
                    }
                    // Remove the index page
                    glob_env.physicalPages.erase(glob_env.physicalPages.begin() + minIndex);
                    page->last_time_accessed = accessTime;
                    // Insert the current page
                    glob_env.physicalPages.push_back(page);

                    // If prepaging
                    if (prePaging && nextPageIndex != -1)
                    {
                        processes[pid]->table[nextPageIndex]->valid_bit = 1;
                        page = processes[pid]->table[nextPageIndex];

                        minIndex = 0;
                        for (index = 0; index < glob_env.physicalPages.size(); index++)
                        {
                            if (glob_env.physicalPages[index]->last_time_accessed < minIndex_Time)
                            {
                                minIndex_Time = glob_env.physicalPages[index]->last_time_accessed;
                                minIndex = index;
                            }
                        }

                        for (int i = 0; i < processes.size(); i++)
                        {
                            for (int j = 0; j < processes[i]->table.size(); j++)
                            {
                                if (processes[i]->table[j] == glob_env.physicalPages[minIndex])
                                    processes[i]->table[j]->valid_bit = 0;
                            }
                        }
                        // Remove the index page
                        glob_env.physicalPages.erase(glob_env.physicalPages.begin() + minIndex);
                        page->last_time_accessed = accessTime;
                        // Insert the current page
                        glob_env.physicalPages.push_back(page);
                    }
                }

                if (!strcmp(algorithm, "Clock"))
                {
                    page->referenced = 1;
                    int clockIndex = glob_env.clockIndex;
                    // Loop through the physical memory in a clock
                    // That is, as index hits the last elem, set it back to 0 and continue the loop
                    while (glob_env.physicalPages[clockIndex]->referenced == 1)
                    {

                        glob_env.physicalPages[clockIndex]->referenced = 0;
                        // cout << "ClockIndex: " << clockIndex << endl;

                        if (clockIndex == glob_env.physicalPages.size() - 1)
                        {
                            clockIndex = 0;
                        }
                        else
                        {
                            clockIndex++;
                        }
                    }

                    for (int i = 0; i < processes.size(); i++)
                    {
                        for (int j = 0; j < processes[i]->table.size(); j++)
                            if (processes[i]->table[j] == glob_env.physicalPages[clockIndex])
                                processes[i]->table[j]->valid_bit = 0;
                    }
                    glob_env.physicalPages[clockIndex] = page;
                    if (clockIndex == glob_env.physicalPages.size() - 1)
                    {
                        clockIndex = 0;
                    }
                    else
                    {
                        clockIndex++;
                    }
                    glob_env.clockIndex = clockIndex;
                    // If prepaging
                    if (prePaging && nextPageIndex != -1)
                    {
                        processes[pid]->table[nextPageIndex]->valid_bit = 1;
                        processes[pid]->table[nextPageIndex]->referenced = 1;
                        page = processes[pid]->table[nextPageIndex];

                        page->referenced = 1;
                        int clockIndex = glob_env.clockIndex;
                        // Loop through the physical memory in a clock
                        // That is, as index hits the last elem, set it back to 0 and continue the loop
                        while (glob_env.physicalPages[clockIndex]->referenced == 1)
                        {

                            glob_env.physicalPages[clockIndex]->referenced = 0;
                            // cout << "ClockIndex: " << clockIndex << endl;

                            if (clockIndex == glob_env.physicalPages.size() - 1)
                            {
                                clockIndex = 0;
                            }
                            else
                            {
                                clockIndex++;
                            }
                        }

                        for (int i = 0; i < processes.size(); i++)
                        {
                            for (int j = 0; j < processes[i]->table.size(); j++)
                                if (processes[i]->table[j] == glob_env.physicalPages[clockIndex])
                                    processes[i]->table[j]->valid_bit = 0;
                        }
                        glob_env.physicalPages[clockIndex] = page;
                        if (clockIndex == glob_env.physicalPages.size() - 1)
                        {
                            clockIndex = 0;
                        }
                        else
                        {
                            clockIndex++;
                        }
                        glob_env.clockIndex = clockIndex;
                    }
                }
                glob_env.pageFaults++;
            }
            //cout << "Page faults " << glob_env.pageFaults << endl;
        }
    }

    cout << "Page faults for " << algorithm << " is: " << glob_env.pageFaults << endl;
    return 0;
}