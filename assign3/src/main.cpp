/*
    Author: Shuangchang Li, Qiuan Wu
    I pledge my honor that I have abided by the Stevens Honor System.
    4/25/2018
*/

#include "../include/main.hh"
#include "../include/Ldisk.hh"

glob_env global;
Ldisk disk;
std::vector<string> str2vec(string s, string sep)
{
    string temp = s;
    vector<std::string> ret;
    string token;
    size_t pos = 0;

    while ((pos = temp.find(sep)) != string::npos)
    {
        token = temp.substr(0, pos);
        ret.push_back(token);
        temp.erase(0, pos + sep.length());
    }

    ret.push_back(temp);

    return ret;
}

string SplitFileName(string &line)
{
    // cout << "Splitting: " << line << endl;
    size_t found = line.find_last_of("/\\");
    // cout << " path: " << line.substr(0, found) << endl;
    // cout << " file: " << line.substr(found+1) << endl;
    return line.substr(found + 1);
}

string getPath(string &line)
{
    size_t found = line.find_last_of("/\\");
    return line.substr(0, found);
}

int main(int argc, char **argv)
{
    global.Root = NULL;
    global.cur_dir = NULL;
    global.block_size = 0;
    global.disk_size = 0;

    if (argc != 9)
    {
        cout << "Usage ./assign3 "
             << "-f [input file storing information on files] "
             << "-d [input file storing information on directories] "
             << "-s [disk size] "
             << "-b [block size] " << endl;
        return 0;
    }

    char *file_list = 0;
    char *dir_list = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            file_list = argv[i + 1];
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            dir_list = argv[i + 1];
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            global.disk_size = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-b") == 0)
        {
            global.block_size = atoi(argv[i + 1]);
        }
    }

    //init blocks in Ldisk
    std::vector<LdiskNode> nodes;
    std::vector<Block> blocks;
    int block_count = global.disk_size / global.block_size;
    for (int i = 0; i < block_count; i++)
    {
        int startid = i * global.block_size;
        int endid = i * global.block_size + global.block_size - 1;
        Block temp = Block(startid, endid, true);
        blocks.push_back(temp);
    }
    LdiskNode Ldisknode = LdiskNode(blocks);
    nodes.push_back(Ldisknode);
    disk = Ldisk(nodes);

    // disk.print();
    //Open dir_list and set up the tree
    int count = 0;
    ifstream dirlist(dir_list);
    if (!dirlist)
    {
        cout << "cannot open dir_list file." << endl;
        return 0;
    }
    string line;
    while (getline(dirlist, line))
    {
        if (line.size() != 0)
        {
            if (count == 0)
            {
                string name = getPath(line);
                Node *node = new Node(name, 0, false);
                global.Root = new Tree(node);
                global.cur_dir = global.Root;
            }
            else
            {
                string filename = SplitFileName(line);
                string path = getPath(line);
                Node *node = new Node(filename, 0, false);
                global.cur_dir = new Tree(node);
                global.cur_dir->setParent(getChild(global.Root, SplitFileName(path)));
                getChild(global.Root, SplitFileName(path))->addChild(global.cur_dir);
            }
            // cout << "cur_dir: " << global.cur_dir->getnodename() << endl;
            count++;
        }
    }

    ifstream filelist;
    filelist.open(file_list);
    if (!filelist)
    {
        cout << "cannot open file_list file." << endl;
        return 0;
    }
    std::vector<string> split;
    while (getline(filelist, line))
    {
        if (line.size() != 0)
        {
            split = str2vec(line, " ");
            // cout << "----------------------------------------------------" << endl;
            // cout << "line: " << split.back() << endl;
            // cout << "filename: " << SplitFileName(split.back()) << endl;
            string path = getPath(split.back());
            string filename = SplitFileName(split.back());
            int fsize = atoi(split[split.size() - 5].c_str());
            // cout << "file size: " << fsize << endl;
            Node *node = new Node(filename, fsize, true);

            global.cur_dir = new Tree(node);
            global.cur_dir->setParent(getChild(global.Root, SplitFileName(path)));
            getChild(global.Root, SplitFileName(path))->addChild(global.cur_dir);
            //allocate file into ldisk
            addfile(node);
            // cout << "BA SIZE: " << global.cur_dir->node->blockAddresses.size() << endl;
            // global.cur_dir->node->printLfile();
        }
    }

    global.cur_dir = global.Root;
    string input;
    while (1)
    {
        cout << global.cur_dir->node->nodename << "$ ";
        getline(cin, input);

        split = str2vec(input, " ");
        if (split[0] == "cd")
        {
            if (split.size() != 2)
            {
                cout << "incorrect number of inputs." << endl;
                return 0;
            }

            if (split[1] == "..")
            {
                if (global.cur_dir->getParent() == NULL)
                {
                    cout << "erro, root directory reached." << endl;
                }
                else
                {
                    global.cur_dir = global.cur_dir->getParent();
                }
            }
            else
            {
                Tree *result = getChild(global.Root, split[1]);
                if (result == NULL)
                {
                    cout << "Erros: Directory not found." << endl;
                }
                else if (result->node->isFile == true)
                {
                    cout << "Error: Not a directory." << endl;
                }
                else
                {
                    global.cur_dir = result;
                }
            }
        }
        else if (split[0] == "ls")
        {
            if (split.size() != 1)
            {
                cout << "incorrect number of inputs." << endl;
                return 0;
            }

            // Loop to print out children of current directory
            for (int i = 0; i < global.cur_dir->children.size(); i++)
            {
                cout << global.cur_dir->children[i]->node->nodename << endl;
            }
        }
        else if (split[0] == "mkdir")
        {
            if (split.size() != 2)
            {
                cout << "incorrect number of inputs." << endl;
                return 0;
            }
            Node *node = new Node(split[1], 0, false);
            Tree *newt = new Tree(node);
            global.cur_dir->addChild(newt);
            getChild(global.Root, node->nodename)->parent = global.cur_dir;
        }
        else if (split[0] == "create")
        {
            if (split.size() != 2)
            {
                cout << "Error: incorrect number of inputs." << endl;
                return 0;
            }
            Node *file = new Node(split[1], 0, true);
            Tree *newt = new Tree(file);
            global.cur_dir->addChild(newt);
            getChild(global.Root, file->nodename)->parent = global.cur_dir;
            addfile(file);
        }
        else if (split[0] == "append")
        {
            if (split.size() != 3)
            {
                cout << "Error: incorrect number of inputs." << endl;
                return 0;
            }

            Tree *file = getChild(global.Root, split[1]);
            if (file == NULL)
            {
                cout << "File not existed." << endl;
            }
            else if (file->node->isFile == false)
            {
                cout << "Error: Input must be a file." << endl;
            }
            else
            {
                file->node->size += atoi(split[2].c_str());
                Tree *temp = new Tree(file->node);
                temp = global.cur_dir;
                global.cur_dir = getChild(global.Root, file->node->nodename);
                addfile(file->node);
                global.cur_dir = temp;
                time(&file->node->time_stamp);
            }
        }
        else if (split[0] == "remove")
        {
            if (split.size() != 3)
            {
                cout << "Error: incorrect number of inputs." << endl;
                return 0;
            }

            Tree *file = getChild(global.Root, split[1]);
            if (file == NULL)
            {
                cout << "Error: File not existed." << endl;
                return 0;
            }
            else if (file->node->isFile == false)
            {
                cout << "Error: input must be a file." << endl;
            }
            else
            {
                file->node->size -= atoi(split[2].c_str());
                if (file->node->size < 0)
                {
                    file->node->size = 0;
                }
                Tree* temp = new Tree(file->node);
                temp = global.cur_dir;
                global.cur_dir = new Tree(file->node);
                removeFile(file->node);

                global.cur_dir = temp;
                time(&(file->node->time_stamp));
            }
        }
        else if (split[0] == "delete")
        {
            if (split.size() != 2)
            {
                cout << "Error: incorrect number of inputs." << endl;
                return 0;
            }
            Tree *find = getChild(global.Root, split[1]);

            if (find == NULL)
            {
                cout << "Error: File Not Exist." << endl;
                return 0;
            }
            Tree *parent = find->parent;
            Node *node = find->node;

            if (!node->isFile)
            {
                if (find->children.size() != 0)
                {
                    cout << "Directory is Not Empty, Cannot be deleted." << endl;
                    return 0;
                }
                else
                {
                    removeChild(parent, find);
                }
            }
            else
            {
                deleteFile(node);
                removeChild(parent, find);
                time(&(parent->node->time_stamp));
            }
        }
        else if (split[0] == "exit")
        {
            if (split.size() != 1)
            {
                cout << "Error: incorrect number of inputs." << endl;
                return 0;
            }

            Tree *temp = global.Root;
            std::vector<Tree *> dump;

            while (temp != NULL)
            {
                for (int i = 0; i < temp->children.size(); i++)
                {
                    dump.push_back(temp->children[i]);
                }

                //free memory
                // cout << "i got here" << endl;
                delete temp->node;
                delete temp;
                if (dump.size() == 0)
                {
                    temp = NULL;
                    cout << "Good Bye" << endl;
                    return 0;
                }
                else
                {
                    temp = dump[0];
                    dump.erase(dump.begin());
                }
            }
        }
        else if (split[0] == "dir")
        {
            if (split.size() != 1)
            {
                cout << "Error: incorrect number of inputs." << endl;
                return 0;
            }
            global.Root->print(global.Root);
        }
        else if (split[0] == "prfiles")
        {
            if (split.size() != 1)
            {
                cout << "Error: incorrect number of inputs." << endl;
                return 0;
            }

            Tree *temp = global.Root;
            std::vector<Tree *> dump;

            while (temp != NULL)
            {
                if (temp->node->isFile)
                {
                    temp->node->printLfile();
                }

                for (int i = 0; i < temp->children.size(); i++)
                {
                    dump.push_back(temp->children[i]);
                }

                if (dump.size() == 0)
                {
                    temp = NULL;
                }
                else
                {
                    temp = dump[0];
                    dump.erase(dump.begin());
                }
            }
        }
        else if (split[0] == "prdisk")
        {
            if (split.size() != 1)
            {
                cout << "Error: Incorrect number of inputs." << endl;
                return 0;
            }

            cout << "Disk: " << endl;
            for (int a = 0; a < disk.nodes.size(); a++)
            {

                disk.nodes[a].printnode();
            }
            int fragmentation = 0;
            int wasted_bytes;
            Tree *temp = global.Root;
            std::vector<Tree *> dump;

            while (temp != NULL)
            {
                // cout << "temp size:" << dump.size() << endl;
                // cout << "temp->node->isFile" << temp->node->isFile << endl;
                if (temp->node->isFile)
                {
                    if (temp->node->blockAddresses.size() != 0)
                    {
                        int lastaddr = temp->node->blockAddresses.back();
                        // cout << "back" << endl;
                        int frontaddr = temp->node->blockAddresses.front();
                        // cout << "front" << endl;
                        wasted_bytes = 0;
                        for (int i = 0; i < disk.nodes.size(); i++)
                        {
                            for (int j = 0; j < disk.nodes[i].blocks.size(); j++)
                            {
                                if (i == disk.nodes.size() - 1 && j == disk.nodes[i].blocks.size() - 1)
                                {
                                    break;
                                }
                                else if (disk.nodes[i].blocks[j].startid == lastaddr)
                                {
                                    // cout << "inside" << endl;
                                    int block_end = disk.nodes[i].blocks[j].endid;
                                    // cout << "block end" << block_end << endl;
                                    // cout << "front " << frontaddr << endl;
                                    // cout << "used_size " << temp->node->used_size << endl;
                                    wasted_bytes = (block_end - frontaddr) - (temp->node->used_size);
                                    // cout << "wasted byes " << wasted_bytes << endl;
                                    i = disk.nodes.size();
                                    j = disk.nodes[i - 1].blocks.size();
                                    break;
                                }
                            }
                        }
                    }
                    fragmentation += wasted_bytes;
                }
                for (int i = 0; i < temp->children.size(); i++)
                {
                    dump.push_back(temp->children[i]);
                }

                if (dump.size() == 0)
                {
                    temp = NULL;
                }
                else
                {
                    temp = dump[0];
                    dump.erase(dump.begin());
                }
            }

            cout << "Fragmentation: " << fragmentation << " bytes" << endl;
        }
        else
        {
            cout << "Invalid Input\n";
        }
    }
}

void updateDisk()
{
    // cout << "start updating..." << endl;
    for (int i = 0; i < disk.nodes.size(); i++)
    {
        int counter = 0;
        for (int j = 0; j < disk.nodes[i].blocks.size(); j++)
        {
            if (j == disk.nodes[i].blocks.size() - 1)
            {
                // cout << "Merging..." << endl;
                if (i == disk.nodes.size() - 1)
                {
                    // cout << "Finished Upding..." << endl;
                    break;
                }
                else
                {
                    if (disk.nodes[i].blocks[j].free == disk.nodes[i + 1].blocks[0].free)
                    {
                        std::vector<Block> temp;
                        for (int l = 0; l < disk.nodes[i].blocks.size(); l++)
                        {
                            temp.push_back(disk.nodes[i].blocks[l]);
                        }
                        for (int k = 0; k < disk.nodes[i + 1].blocks.size(); k++)
                        {
                            temp.push_back(disk.nodes[i + 1].blocks[k]);
                        }

                        disk.nodes[i] = temp;
                        disk.nodes.erase(disk.nodes.begin() + i + 1);
                    }
                }
            }
            else
            {
                // cout << "Spliting..." << endl;
                if (disk.nodes[i].blocks[j].free != disk.nodes[i].blocks[j + 1].free)
                {
                    std::vector<Block> temp;
                    for (int l = 0; l < j + 1; l++)
                    {
                        temp.push_back(disk.nodes[i].blocks[l]);
                    }

                    std::vector<Block> newB;
                    for (int k = j + 1; k < disk.nodes[i].blocks.size(); k++)
                    {
                        newB.push_back(disk.nodes[i].blocks[k]);
                    }
                    LdiskNode newNode = LdiskNode(newB);
                    disk.nodes[i] = temp;
                    disk.nodes.push_back(newNode);
                }
            }
        }
    }
}

void addfile(Node *node)
{
    int size_not_used = node->size;
    int bytesNeeded = size_not_used - node->used_size;
    if (bytesNeeded < 0)
    {
        bytesNeeded = 0;
    }

    int blocks_to_use = ceil((bytesNeeded * 1.0) / global.block_size);
    // cout << "block to use: " << blocks_to_use << endl;
    while (blocks_to_use > 0)
    {
        for (int j = 0; j < disk.nodes.size(); j++)
        {
            for (int k = 0; k < disk.nodes[j].blocks.size(); k++)
            {
                if (disk.nodes[j].blocks[k].free)
                {

                    if (bytesNeeded >= global.block_size)
                    {
                        disk.nodes[j].blocks[k].free = false;
                        disk.nodes[j].blocks[k].full = true;
                        disk.nodes[j].blocks[k].bytesleft = 0;
                        node->used_size += global.block_size;
                        size_not_used -= global.block_size;
                        bytesNeeded -= global.block_size;
                        // cout << "here "<< j * global.block_size  << endl;
                        //add block address to lfile
                        global.cur_dir->node->blockAddresses.push_back(disk.nodes[j].blocks[k].startid);
                        // cout << "BA: "<< global.cur_dir->node->blockAddresses[0] << endl;
                    }
                    else
                    {
                        disk.nodes[j].blocks[k].free = false;
                        disk.nodes[j].blocks[k].bytesleft = global.block_size - bytesNeeded;
                        node->used_size += bytesNeeded;
                        size_not_used -= bytesNeeded;
                        bytesNeeded = 0;
                        global.cur_dir->node->blockAddresses.push_back(disk.nodes[j].blocks[k].startid);
                    }
                    blocks_to_use--;
                    break;
                }
                if (j == disk.nodes.size() - 1 && k == disk.nodes[j].blocks.size() - 1)
                {
                    // cout << "Error: disk is full..." << endl;
                    cout << "Error: OUT OF SPACE!" << endl;
                    int blocks_left_to_use = blocks_to_use;
                    // cout << "There are " << blocks_left_to_use << " blocks of this file that cannot be added!" << endl;
                    blocks_to_use = 0;
                    break;
                }
            }
        }
    }
    // cout << "LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOK HERE" << global.cur_dir->node->blockAddresses.size() << endl;
    // disk.print();
    // disk.printinbytes();
    updateDisk();
    // cout << "Disk Size: " << disk.nodes.size() << endl;
    // cout << "Disk: " << endl;
    // for (int a = 0; a < disk.nodes.size(); a++)
    // {

    //     disk.nodes[a].printnode();
    // }
    // cout << endl;
}

void deleteFile(Node *node)
{
    int bytestoremove = node->used_size;
    

    int blocks_to_remove = bytestoremove / global.block_size;
    // cout << "deleting... " << endl;
    // node->printLfile();
    // cout << "Used Bytes: " << node->used_size << endl;
    // cout << "size: " << node->size << endl;
    // cout << "blocks_to_remove: " << blocks_to_remove << endl;
    int counter;
    while (blocks_to_remove > 0)
    {
        // cout << blocks_to_remove << endl;
        // cout << "deleting... "<< endl;
        std::vector<int>::iterator it = node->blockAddresses.end();
        it--;
        int blockId = (*it) / global.block_size;
        // cout << "Block id:" << blockId << endl;

        for (int i = disk.nodes.size() - 1; i >= 0; i--)
        {
            for (int j = disk.nodes[i].blocks.size() - 1; j >= 0; j--)
            {
                // cout << "Block id: " << blockId << endl;
                // cout << "start id: " << disk.nodes[i].blocks[j].startid << endl;
                // cout << "end id: " << disk.nodes[i].blocks[j].endid << endl;
                if (blockId >= disk.nodes[i].blocks[j].startid && blockId <= disk.nodes[i].blocks[j].endid)
                {
                    if (bytestoremove >= global.block_size)
                    {
                        disk.nodes[i].blocks[j].free = true;
                        disk.nodes[i].blocks[j].full = false;
                        disk.nodes[i].blocks[j].bytesleft = global.block_size;

                        node->used_size -= global.block_size;
                        // cout << "deleting...2 " << endl;
                        bytestoremove -= global.block_size;
                        // cout << "deleting...3 " << endl;
                        // cout << "here "<< i * global.block_size  << endl;
                        //add block address to lfile
                        // getChild(global.Root, node->nodename)->node->printLfile();
                        // cout << "block addr " << getChild(global.Root, node->nodename)->node->blockAddresses.size() << endl;
                        getChild(global.Root, node->nodename)->node->blockAddresses.erase(it);
                        // cout << "deleting...1 " << endl;
                    }
                    else
                    {
                        disk.nodes[i].blocks[j].free = false;
                        disk.nodes[i].blocks[j].bytesleft = global.block_size - bytestoremove;
                        node->used_size -= bytestoremove;
                        bytestoremove = 0;
                        global.cur_dir->node->blockAddresses.erase(it);
                    }
                    blocks_to_remove--;
                    break;
                }

                if (i == 0 && j == 0)
                {
                    cout << "Error: disk is Empty..." << endl;
                    // cout << "There are " << blocks_left_to_use << " blocks of this file that cannot be added!" << endl;
                    blocks_to_remove = 0;
                    break;
                }
            }
        }
    }
    updateDisk();
}

void removeFile(Node *node)
{
    int bytestoremove = node->used_size - node->size;
    if (bytestoremove < global.block_size)
    {
        std::vector<int>::iterator it = node->blockAddresses.end();
        it--;
        
        int blockId = (*it) / global.block_size;
        for (int i = disk.nodes.size() - 1; i >= 0; i--)
        {
            for (int j = disk.nodes[i].blocks.size() - 1; j >= 0; j--)
            {
                if (blockId >= disk.nodes[i].blocks[j].startid && blockId <= disk.nodes[i].blocks[j].endid)
                {
                    disk.nodes[i].blocks[j].free = false;
                    disk.nodes[i].blocks[j].bytesleft = global.block_size - bytestoremove;
                    node->used_size -= bytestoremove;
                    bytestoremove = 0;
                    global.cur_dir->node->blockAddresses.erase(it);         
        
                }
                
                break;
            }
        }
        
    }
    

    int blocks_to_remove = bytestoremove / global.block_size;
    int counter;
    while (blocks_to_remove > 0)
    {
        // cout << blocks_to_remove << endl;
        // cout << "deleting... "<< endl;
        std::vector<int>::iterator it = node->blockAddresses.end();
        it--;
        int blockId = (*it) / global.block_size;
        // cout << "Block id:" << blockId << endl;

        for (int i = disk.nodes.size() - 1; i >= 0; i--)
        {
            for (int j = disk.nodes[i].blocks.size() - 1; j >= 0; j--)
            {
                // cout << "Block id: " << blockId << endl;
                // cout << "start id: " << disk.nodes[i].blocks[j].startid << endl;
                // cout << "end id: " << disk.nodes[i].blocks[j].endid << endl;
                if (blockId >= disk.nodes[i].blocks[j].startid && blockId <= disk.nodes[i].blocks[j].endid)
                {
                    if (bytestoremove >= global.block_size)
                    {
                        disk.nodes[i].blocks[j].free = true;
                        disk.nodes[i].blocks[j].full = false;
                        disk.nodes[i].blocks[j].bytesleft = global.block_size;

                        node->used_size -= global.block_size;
                        // cout << "deleting...2 " << endl;
                        bytestoremove -= global.block_size;
                        // cout << "deleting...3 " << endl;
                        // cout << "here "<< i * global.block_size  << endl;
                        //add block address to lfile
                        // getChild(global.Root, node->nodename)->node->printLfile();
                        // cout << "block addr " << getChild(global.Root, node->nodename)->node->blockAddresses.size() << endl;
                        getChild(global.Root, node->nodename)->node->blockAddresses.erase(it);
                        // cout << "deleting...1 " << endl;
                    }
                    else
                    {
                        disk.nodes[i].blocks[j].free = false;
                        disk.nodes[i].blocks[j].bytesleft = global.block_size - bytestoremove;
                        node->used_size -= bytestoremove;
                        bytestoremove = 0;
                        global.cur_dir->node->blockAddresses.erase(it);
                    }
                    blocks_to_remove--;
                    break;
                }

                if (i == 0 && j == 0)
                {
                    cout << "Error: disk is Empty..." << endl;
                    // cout << "There are " << blocks_left_to_use << " blocks of this file that cannot be added!" << endl;
                    blocks_to_remove = 0;
                    break;
                }
            }
        }
    }
    updateDisk();
}