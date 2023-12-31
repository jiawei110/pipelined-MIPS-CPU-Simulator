#include <iostream>
#include<string>

using namespace std;
template <class T>
class ChainNode {
    // constructors come here
public:
    ChainNode<T>* link;
    T data;
    ChainNode(const T& data, ChainNode<T>* link)
    {
        this->data = data;
        this->link = link;
    }
    ChainNode(const T& data)
    {
        this->data = data;
        this->link = NULL;
    }


};


template<class T>
class Chain
{
public:
    Chain() { first = NULL; } // constructor, empty chain    
    
    ~Chain()
    {
        // Chain destructor. Delete all nodes in chain.
        while (first != NULL)
        {
            ChainNode<T>* next = first->link;
            delete first;
            first = next;
        }
    }
    int lencheck() 
    {
        int len = 0;
        if (first == 0) 
        {
            return 0;
        }
        ChainNode<T>* current = first;
        while (current != 0) 
        {
            len += 1;
            current = current->link;
        }
        return len;
    }
    string* getByIndex(int index) 
    {

        if (lencheck() < index)
        {
            cout << "index error" << endl;
            return NULL;
 
        }
        int cur = 0;
        ChainNode<T>* current = first;
        while (current != NULL)
        {
            if (cur == index)
                return current->data;
            cur += 1;
            current = current->link;
        }
        return NULL;

    }
    
    
    void PrintList() {

        if (first == 0) {                      // 如果first node指向NULL, 表示list沒有資料
            cout << "List is empty.\n";
            return;
        }

        ChainNode* current = first;             // 用pointer *current在list中移動
        while (current != 0) {                 // Traversal
            cout << current->data << " ";
            current = current->link;
        }
        cout << endl;
    }

    bool IsEmpty() const { return first == NULL; }

    void Delete(int theIndex)
    {
        if (first == 0)
            throw "Cannot delete from empty chain";
        ChainNode<T>* deleteNode;
        if (theIndex < 0)
            throw "Bad insert index";
        if (theIndex == 0)
        {
            // remove first node from chain
            deleteNode = first;
            first = first->link;
            delete deleteNode;
            return;
        }
        else
        { 
            // use p as beforeNode
            ChainNode<T>* p = first;
            for (int i = 0; i < theIndex - 1; i++)
            {
                p = p->link;
            }
            deleteNode = p->link;
            p->link = p->link->link;
        }
        delete deleteNode;
        return;
    }
    void Pop() 
    {
        if (first == 0)
            throw "Cannot delete from empty chain";
        ChainNode<T>* deleteNode = first;
        if (deleteNode->link == NULL) { 
            delete deleteNode; 
            return;
        }
        while (deleteNode->link->link != NULL) 
        {
            deleteNode = deleteNode->link;
        }
        delete deleteNode->link;
        deleteNode->link = NULL;
        return;

    }
    void Push_back(const T& theElement) {

        ChainNode<T>* newNode = new ChainNode<T>(theElement);   // 配置新的記憶體

        if (first == 0) {                      // 若list沒有node, 令newNode為first
            first = newNode;
            return;
        }

        ChainNode<T>* current = first;
       
        while (current->link != NULL) {           // Traversal
            current = current->link;
        }
        current->link = new ChainNode<T>(theElement);               // 將newNode接在list的尾巴
        return;
    }

    void Insert(int theIndex, const T& theElement)
    {
        if (theIndex < 0)
            throw "Bad insert index";
        if (theIndex == 0)
            // insert at front
            first = new ChainNode<T>(theElement, first);
        else
        { 
            // use p as beforeNode
            ChainNode<T>* p = first;
            for (int i = 0; i < theIndex - 1; i++)
            {
                p = p->link;
            }
            // insert after p
            p->link = new ChainNode<T>(theElement, p->link);
        }
        return;
    }





    

private:
    ChainNode<T>* first;
};

