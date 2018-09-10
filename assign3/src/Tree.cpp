/*
    Author: Shuangchang Li, Qiuan Wu
    I pledge my honor that I have abided by the Stevens Honor System.
    4/25/2018
*/
#include "../include/main.hh"

Tree::Tree(Node *n)
{
    node = n;
    parent = NULL;
    nodename = node->nodename;
}

//Adds Child to the Children vector and set the child to be under current parent
void Tree::addChild(Tree *child){
    child->setParent(this);
    children.push_back(child);
}

//set the Tree to be the parent
void Tree::setParent(Tree *p){
    parent = p;
}

//return a bool of checking if the current node has child
bool Tree::hasChildren(){
    if(children.size() > 0){
        return true;
    }
    else{
        return false;
    }
}


//return a bool of checking if current node has parent
bool Tree::hasParent()
{
    if(parent != NULL)
        return true;
    else 
        return false;
}

//return the parent of current node
Tree* Tree::getParent(){
    return parent;
}

void Tree::print(Tree *tree){
    Tree *start = tree;
    std::vector<Tree *> q;
    while(start != NULL){
        start->node->print();

        for(int i = 0; i < start->children.size(); i++)
        {
            q.push_back(start->children[i]);
        }
        if(q.size() == 0)
        {
            start = NULL;
        }
        else{
            start = q[0];
            q.erase(q.begin());
        }
    }
}
//find the tree node that holds a file with a specific filename.
Tree* getChild(Tree *root, string nodename){
    if(root != NULL)
    {
        Tree* temp = root;
        std::vector<Tree *> q;

        while(temp != NULL)
        {
            if(temp->node->nodename == nodename){
                // cout << "We Found File: " << nodename << endl;
                break;
            }

            for(int i = 0; i < temp->children.size(); i++)
            {
                q.push_back(temp->children[i]);
            }

            if(q.size() == 0)
            {
                temp = NULL;
            }
            else
            {
                temp = q[0];
                q.erase(q.begin());
            }
        }

        return temp;
    }
    else{
        return NULL;
    }
}

//remove child from the of current parent and return the tree that was deleted to free its memory
Tree* removeChild(Tree* parent, Tree* child)
{
    if(child->hasChildren())
    {
        cout << "There are files inside of this Directory, you need to delete them first."<< endl;
        return NULL;  
    }
    else{
        for(int i = 0; i < parent->children.size(); i++)
        {
            if(parent->children[i]->node->nodename == child->node->nodename)
            {
                parent->children.erase(parent->children.begin() + i);
            }
        }
        child->setParent(NULL);

        return child;
    }
}

//return the name of current file or directory.
string Tree::getnodename()
{
    return nodename;
}


Node::Node(string n, int s, bool b)
{
    this->nodename = n;
    this->size = s;
    time(&time_stamp);
    this->isFile = b;
    this->used_size = 0;
    this->blockAddress = std::vector<int>();
}

void Node::print()
{
    cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
    cout << "Filename: " << nodename << " | Is Directory: " << !isFile << " | Size: " << size << " | Timestamp: " << ctime(&time_stamp)<< endl;
}

void Node::printLfile()
{
    std::vector<int>::iterator it = this->blockAddresses.begin();
    this->print();
    cout << this->blockAddresses.size() << endl;
    if(this->blockAddresses.size() == 0)
    {
        cout << nodename << " does not belong to any Block." << endl;
    }
    else if (this->blockAddresses.size() == 1)
    {
        cout << "Block Addr: " << this->blockAddresses[0] << endl;
    }
    else
    {
        cout << "Block Addr: " << this->blockAddresses[0];
        cout << "| Pointer to the next Block: " << (&it + 1) << endl;
    }
}