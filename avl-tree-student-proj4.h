/**
* file: avl-tree-student-proj4.h
* author: Michael Mathews
* course: CSI 3334
* assignment: project 4
* due date: March 25, 2022
* version: 11.7
*
* This file is the implementation for the AVLTree, AVLNode, and EncryptionTree
* classes
*/

#ifndef avl_tree_student_proj4_h
#define avl_tree_student_proj4_h

#include "avl-tree-prof-proj4.h"
#include <queue>
/* AVLNode Destructor
*  parameters:
*      none
*  return value: none
*
* This function deletes AVLNodes
*/
template <class Base>
AVLNode<Base>::~AVLNode(){
    if(this->left){
        delete this->left;
    }
    if(this->right){
        delete this->right;
    }
}

/* printPreorder
*  parameters:
*      os - output stream to write to
*      indent - amount of spaces before the data in each node
*  return value: none
*
* This function prints a tree of AVLNodes in preorder
*/
template <class Base>
void AVLNode<Base>::printPreorder(ostream &os, string indent) const{
    const int UNINDENT_START = 0, UNINDENT_END = 2;
    //print Me Left Right
    os << indent << this->data << endl;
    if(this->left){
        indent.append("  ");
        this->left->printPreorder(os, indent);
    }
    else {
        indent.append("  ");
        os << indent << "NULL" << endl;
    }
    
    if(this->right){
        this->right->printPreorder(os, indent);
    }
    else {
        os << indent << "NULL" << endl;
    }
    indent.erase(UNINDENT_START, UNINDENT_END);
}

/* singleRotateLeft
*  parameters:
*      none
*  return value: pointer to the new top node after a rotation of an
*                AVL tree/subtree
*
* This function rotates an AVL tree/subtree left one time
*/
template <class Base>
AVLNode<Base> *AVLNode<Base>::singleRotateLeft(){
    AVLNode<Base> *rightChild = this->right;
    this->right = rightChild->left;
    rightChild->left = this;
    return rightChild;
}
/* singleRotsingleRotateRightateLeft
*  parameters:
*      none
*  return value: pointer to the new top node after a rotation of an
*                AVL tree/subtree
*
* This function rotates an AVL tree/subtree right one time
*/
template <class Base>
AVLNode<Base> *AVLNode<Base>::singleRotateRight(){
    AVLNode<Base> *leftChild = this->left;
    this->left = leftChild->right;
    leftChild->right = this;
    return leftChild;
}
/* doubleRotateLeftRight
*  parameters:
*      none
*  return value: pointer to the new top node after a rotation of an
*                AVL tree/subtree
*
* This function rotates an AVL tree/subtree left and then right for a Left
* Right case
*/
template <class Base>
AVLNode<Base> *AVLNode<Base>::doubleRotateLeftRight(){
    this->left = this->left->singleRotateLeft();
    return this->singleRotateRight();
}
/* doubleRotateRightLeft
*  parameters:
*      none
*  return value: pointer to the new top node after a rotation of an
*                AVL tree/subtree
*
* This function rotates an AVL tree/subtree right and then left for a Right
* Left case
*/
template <class Base>
AVLNode<Base> *AVLNode<Base>::doubleRotateRightLeft(){
    this->right = this->right->singleRotateRight();
    return this->singleRotateLeft();
}

/* minNode
*  parameters:
*      none
*  return value: smallest AVLNode in an AVL tree
*
* This function finds the smallest Node in a AVLNode
* (node with the smallest value)
*/
template <class Base>
const AVLNode<Base>* AVLNode<Base>::minNode() const{
    const AVLNode *leftPtr = this;
    while(leftPtr->left){
        leftPtr = leftPtr->left;
    }
    return leftPtr;
}

/* maxNode
*  parameters:
*      none
*  return value: largest AVLNode in an AVL tree
*
* This function finds the largest Node in an AVL tree
* (node with the largest value)
*/
template <class Base>
const AVLNode<Base>* AVLNode<Base>::maxNode() const{
    const AVLNode *rightPtr = this;
    while(rightPtr->right){
        rightPtr = rightPtr->right;
    }
    return rightPtr;
}

/* insert
*  parameters:
*      item - data to be stored in an AVL tree
*  return value: none
*
* This function inserts a value into its correct place in an AVL tree
*/
template <class Base>
void AVLTree<Base>::insert(const Base &item){
    AVLNode<Base> *ptr = this->root, *parent = this->root;
    vector<AVLNode<Base> *> vect;
    bool found = false, first = true;

    //search for item
    while(ptr and !found){
        if(item < ptr->data){
            vect.push_back(ptr);
            parent = ptr;
            ptr = ptr->left;
        }
        else if(ptr->data < item){
            vect.push_back(ptr);
            parent = ptr;
            ptr = ptr->right;
        }
        else{
            found = true;
        }
        first = false;
    }
    
    //if first element, create first node
    if(first){
        AVLNode<Base> *newNode = new AVLNode<Base>(item);
        this->root = newNode;
        vect.push_back(this->root);
    }
    //if not in avl, insert
    else if(!found){
        AVLNode<Base> *newNode = new AVLNode<Base>(item);
        if(item < parent->data){
            parent->left = newNode;
        }
        else{
            parent->right = newNode;
        }
        //add last ptr to vector
        vect.push_back(newNode);
    }
    //update heights
    for(int i = vect.size()-1; i >= 0; i--){
        vect[i]->AVLNode<Base>::updateHeight();
    }
    //rebalance tree
    this->rebalancePathToRoot(vect);
}

/* remove
*  parameters:
*      item - data to be removed from an AVL tree
*  return value: none
*
* This function removes a value from a AVL tree
*/
template <class Base>
void AVLTree<Base>::remove(const Base &item){
    AVLNode<Base> *ptr = this->root, *parent = ptr;
    vector<AVLNode<Base> *> vect;
    bool found = false;
    //find parent and node to remove
    while(ptr and !found){
        if(item < ptr->data){
            vect.push_back(ptr);
            parent = ptr;
            ptr = ptr->left;
        }
        else if(ptr->data < item){
            vect.push_back(ptr);
            parent = ptr;
            ptr = ptr->right;
        }
        else{
            found = true;
        }
    }
    
    //if it exists, delete it
    if(found){
        //if two children
        if(ptr->left and ptr->right){
            AVLNode<Base> *leftMost = ptr->right, *leftMostParent = ptr;
            int ndx;
            //add node to remove to path, remember location
            vect.push_back(ptr);
            ndx = vect.size()-1;
            if (leftMost->left != nullptr) {
                while (leftMost->left != nullptr) {
                    //add path to leftmost to vector
                    vect.push_back(leftMost);
                    leftMostParent = leftMost;
                    leftMost = leftMost->left;
                }
                leftMostParent->left = leftMost->right;
                leftMost->right = ptr->right;
            }
            //put leftMost in node to remove's place
            vect[ndx] = leftMost;
            
            leftMost->left = ptr->left;
            if (parent->left == ptr) {
                parent->left = leftMost;
            }
            else if(parent->right == ptr){
                parent->right = leftMost;
            }
            else {
                this->root = leftMost;
            }
            ptr->left = ptr->right = nullptr;
            delete ptr;
            ptr = nullptr;
        }
        //if one child
        else if(ptr->left or ptr->right){
            AVLNode<Base> *child = nullptr;
            if (ptr->left != nullptr) {
                child = ptr->left;
                ptr->left = nullptr;
            }
            else {
                child = ptr->right;
                ptr->right = nullptr;
            }
            if (parent->left == ptr) {
                parent->left = child;
            }
            else if(parent->right == ptr){
                parent->right = child;
            }
            else {
                this->root = child;
            }
            ptr->left = ptr->right = nullptr;
            delete ptr;
            ptr = nullptr;
        }
        //if no children
        else{
            if (ptr == parent->left) {
                parent->left = nullptr;
            }
            else if(ptr == parent->right) {
                parent->right = nullptr;
            }
            else{
                this->root = nullptr;
            }
            ptr->left = ptr->right = nullptr;
            delete ptr;
            ptr = nullptr;
        }
        //update heights
        for(int i = vect.size()-1; i >= 0; i--){
            vect[i]->AVLNode<Base>::updateHeight();
        }
        //rebalance tree
        this->rebalancePathToRoot(vect);
    }
}

/* printLevelOrder
*  parameters:
*      os - output stream to write to
*  return value: none
*
* This function prints an AVL tree in level order
*/
template <class Base>
void AVLTree<Base>::printLevelOrder(ostream &os) const{
    queue<AVLNode<Base> *> q;
    AVLNode<Base> *current;
    int lineCounter = 1;
    const int NODE_MAX = 20;
    if(this->root){
        q.push(this->root);
        while(!q.empty()){
            current = q.front();
            //if the current node's left exists, add it to queue
            if(current){
                if(current->left){
                    q.push(current->left);
                }
                else{
                    q.push(nullptr);
                }
                //if the current node's right exists, add it to queue
                if(current->right){
                    q.push(current->right);
                }
                else{
                    q.push(nullptr);
                }
            }
            
            //print current node
            if(lineCounter == 1){
                if(q.front()){
                    os << q.front()->getData();
                }
                else{
                    os << "NULL";
                }
                lineCounter++;
            }
            else{
                if(q.front()){
                    os << " " << q.front()->getData();
                }
                else{
                    os << " NULL";
                }
                lineCounter++;
                //make sure not more than 20 nodes on a line
                if(lineCounter > NODE_MAX){
                    os << endl;
                    lineCounter = 1;
                }
            }
            //take current off queue
            q.pop();
        }
    }
    else{
        os << "NULL" << endl;
    }
    
    if(lineCounter > 1){
        os << endl;
    }
}

/* rebalancePathToRoot
*  parameters:
*      path - vector that stores the route taken to get to the desired node
*  return value: none
*
* This function rebalances an AVL tree from bottom to top
*/
template <class Base>
void AVLTree<Base>::rebalancePathToRoot(vector<AVLNode<Base> *> const &path){
    //set equal to bottom node
    int i = path.size()-1, diff;
    AVLNode<Base> *node, *newTop;
    //rebalance up to first node
    while(i >= 0){
        //update current node
        node = path[i];
        //find if imbalance
        diff = AVLNode<Base>::getHeight(node->left) -
               AVLNode<Base>::getHeight(node->right);
        if(diff < -1){
            //find taller tree on right side
            diff = AVLNode<Base>::getHeight(node->right->left) -
                   AVLNode<Base>::getHeight(node->right->right);
            //rotate
            if(diff <= 0){
                //set parent to the correct child
                if(i < 1){
                    newTop = node->singleRotateLeft();
                    this->root = newTop;
                }
                else if(path[i-1]->right == node){
                    newTop = node->singleRotateLeft();
                    path[i-1]->right = newTop;
                }
                else{
                    newTop = node->singleRotateLeft();
                    path[i-1]->left = newTop;
                }
                //update height of old and new top node
                node->updateHeight();
                newTop->updateHeight();
            }
            else{
                //set parent to the correct child
                if(i < 1){
                    newTop = node->doubleRotateRightLeft();
                    this->root = newTop;
                }
                else if(path[i-1]->right == node){
                    newTop = node->doubleRotateRightLeft();
                    path[i-1]->right = newTop;
                }
                else{
                    newTop = node->doubleRotateRightLeft();
                    path[i-1]->left = newTop;
                }
                //update heights of new top, left, and right
                newTop->left->updateHeight();
                newTop->right->updateHeight();
                newTop->updateHeight();
            }
            //update rest of heights
            for(int j = i; j >= 0; j--){
                path[j]->updateHeight();
            }
        }
        else if(diff > 1){
            //find taller tree on left side
            diff = AVLNode<Base>::getHeight(node->left->left) -
                   AVLNode<Base>::getHeight(node->left->right);
            //rotate
            if(diff >= 0){
                //set parent to the correct child
                if(i < 1){
                    newTop = node->singleRotateRight();
                    this->root = newTop;
                }
                else if(path[i-1]->right == node){
                    newTop = node->singleRotateRight();
                    path[i-1]->right = newTop;
                }
                else{
                    newTop = node->singleRotateRight();
                    path[i-1]->left = newTop;
                }
                //update height of old and new top node
                node->updateHeight();
                newTop->updateHeight();
            }
            else{
                if(i < 1){
                    newTop = node->doubleRotateLeftRight();
                    this->root = newTop;
                }
                else if(path[i-1]->right == node){
                    newTop = node->doubleRotateLeftRight();
                    path[i-1]->right = newTop;
                }
                else{
                    newTop = node->doubleRotateLeftRight();
                    path[i-1]->left = newTop;
                }
                //update heights of new top, left, and right
                newTop->left->updateHeight();
                newTop->right->updateHeight();
                newTop->updateHeight();
            }
            //update new heights
            for(int j = path.size()-1; j >= 0; j--){
                path[j]->updateHeight();
            }
        }
        //go up tree one
        i--;
    }
}


/* encrypt
*  parameters:
*      item - data to be encrypted using codebook
*  return value: encoded data in the form of a string
*
* This function encodes data based on its location in an AVL tree codebook
*/
template <class Base>
string EncryptionTree<Base>::encrypt(const Base &item) const{
    string secret;
    bool done = false;
    const AVLNode<Base> *ptr = this->root;
    //start with root
    secret.push_back('r');
    //while not done and at least one child exists
    while(!done){
        if(ptr){
            //if less than current node, go left
            if(item < ptr->getData()){
                secret.push_back('0');
                ptr = ptr->getLeft();
            }
            //if greater than current node, go right
            else if(ptr->getData() < item){
                secret.push_back('1');
                ptr = ptr->getRight();
            }
            //if equal to current node, done
            else {
                done = true;
            }
        }
        //if not found
        else {
            done = true;
            secret = "?";
        }
    }
    return secret;
}

/* decrypt
*  parameters:
*      path - directions to be decrypted using codebook
*  return value: pointer to the data found using the path
*
* This function decodes a path to find data based on its location
* in an AVL tree codebook
*/
template <class Base>
const Base * EncryptionTree<Base>::decrypt(const string &path) const{
    const Base *item;
    const AVLNode<Base> *ptr = this->root;
    //make sure path starts at root
    assert(path[0] == 'r');
    //find word in codebook if word exists
    for(unsigned int i = 1; ptr and i < path.length(); i++){
        if(path[i] == '0'){
            ptr = ptr->getLeft();
        }
        else if(path[i] == '1'){
            ptr = ptr->getRight();
        }
    }
    
    if(ptr){
        item = &ptr->getData();
    }
    else {
        item = NULL;
    }
    //return pointer to value or NULL if encrypted path not valid
    return item;
}

#endif /* avl_tree_student_proj4_h */
