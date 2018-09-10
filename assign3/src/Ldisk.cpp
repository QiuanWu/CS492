/*
    Author: Shuangchang Li, Qiuan Wu
    I pledge my honor that I have abided by the Stevens Honor System.
    4/25/2018
*/
#include "../include/Ldisk.hh"

Block::Block(int startid, int endid, bool free)
{
    this->startid = startid;
    this->endid = endid;
    this->free = free;
    this->full = false;
    this->bytesleft = global.block_size;
    void print();
}

void Block::print()
{
    cout << this->startid << "-" << this->endid << "\t" << (this->free ? "Free." : "In use.") << endl;
}

LdiskNode::LdiskNode(std::vector<Block> blocks)
{
    this->blocks = blocks;

    void print();
}
void LdiskNode::print()
{
    for (int i = 0; i < this->blocks.size(); i++)
    {
        this->blocks[i].print();
    }
}

void LdiskNode::printnode()
{
    cout <<(this->blocks.front().free ? "Free: " : "In use: ") << this->blocks.front().startid << "-" << this->blocks.back().endid <<  endl;
}

Ldisk::Ldisk(std::vector<LdiskNode> nodes)
{
    this->nodes = nodes;

    void print();
}

Ldisk::Ldisk()
{
}

void Ldisk::print()
{
    for (int i = 0; i < this->nodes.size(); i++)
    {
        this->nodes[i].print();
    }
}

void Ldisk::printinbytes()
{
    std::vector<Block> temp;
    for(int i = 0; i < this->nodes.size(); i++)
    {
        for(int j = 0; j < this->nodes[i].blocks.size(); j++)
        {
            Block tempA = this->nodes[i].blocks[j];
            if(tempA.free == false)
            {
                if(tempA.full == true)
                {
                    int start = tempA.startid;
                    int end = tempA.endid;
                    Block tempB = Block(start, end, false);
                    temp.push_back(tempB);
                }
                else if(tempA.full == false)
                {
                    int start = tempA.startid;
                    int end = tempA.endid - tempA.bytesleft;
                    Block tempB = Block(start, end, false);
                    temp.push_back(tempB);

                    start = end + 1;
                    end = tempA.endid;
                    tempB = Block(start, end, true);
                    temp.push_back(tempB);
                }
            }
            else if(tempA.free == true)
            {
                int start = tempA.startid;
                int end = tempA.endid;
                Block tempB = Block(start, end, true);
                temp.push_back(tempB);

            }
        }
    }
    LdiskNode bytesNode = LdiskNode(temp);
    for(int k = 0; k < temp.size(); k++)
    {
        bytesNode.blocks[k].print();
    }
}

