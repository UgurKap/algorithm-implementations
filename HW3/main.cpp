#include <iostream>
#include "hw3.h"
#include <cstdlib>

class Node{
public:
    bool isRed;
    int key;
    Node *parent;
    Node *left;
    Node *right;
    Node(int key);
};

Node::Node(int key) {
    this->key = key;
    this->parent = nullptr;
    this->left = nullptr;
    this->right = nullptr;
    this->isRed = false;
}

class RBTree{
public:
    Node *root;
    void RotateLeft(Node *node);
    void RotateRight(Node *node);
    void Insert(Node *node);
    void InsertFix(Node *node);
    void Transplant(Node *oldNode, Node *newNode);
    void Delete(Node *node);
    void DeleteFix(Node *node);
    void InOrderWalk(Node *node);
    Node* TreeMinimum(Node *begin);
    RBTree();
};

void RBTree::RotateLeft(Node *node) {
    /*
     * This function rotates the given node to left.
     * Right child of the given node will become given node's parent.
     * Given node will be the left child of its right child.
     */
    if (node->right){
        Node *temp = node->right;
        node->right = temp->left;

        if (temp->left != nullptr){
            temp->left->parent = node;
        }

        temp->parent = node->parent;

        if (node->parent == nullptr){
            root = temp;
        } else if (node == node->parent->left){
            node->parent->left = temp;
        } else {
            node->parent->right = temp;
        }

        temp->left = node;
        node->parent = temp;
    } else {
        std::cerr << "You cannot left rotate if there is no right child!" << std::endl;
    }
}

void RBTree::RotateRight(Node *node) {
    /*
     * This function rotates the given node to right.
     * Given node's left child become given node's parent.
     * Given node becomes left child's right child.
     */
    if (node->left){
        Node *temp = node->left;
        node->left = temp->right;

        if (temp->right != nullptr){
            temp->right->parent = node;
        }

        temp->parent = node->parent;

        if (node->parent == nullptr){
            root = temp;
        } else if (node == node->parent->left){
            node->parent->left = temp;
        } else {
            node->parent->right = temp;
        }

        temp->right = node;
        node->parent = temp;
    } else {
        std::cerr << "You cannot right rotate if there is no left child!" << std::endl;
    }
}

void RBTree::Insert(Node *node) {
    /*
     * This function inserts a new node into tree and colors it red.
     * It preserves binary search tree properties, however to ensure
     * that we are not violating any properties of red-black trees,
     * we call another function called InsertFix that fixes any coloring
     * issues.
     */

    Node *temp = nullptr;
    Node *traverse = this->root;
    while (traverse){
        temp = traverse;
        if (node->key < traverse->key){
            traverse = traverse->left;
        } else traverse = traverse->right;
    }
    node->parent = temp;
    if (temp == nullptr){
        this->root = node;
    } else if (node->key < temp->key){
        temp->left = node;
    } else {
        temp->right = node;
    }

    node->left = nullptr;
    node->right = nullptr;
    node->isRed = true;
    InsertFix(node);
}

void RBTree::InsertFix(Node *node) {
    /*
     * This function fixes coloring issues of the red-black tree
     * in an iterative fashion. If node's parent is black, there
     * are no coloring problems because black height does not change.
     *
     * However, if parent of the newly inserted node is red we are
     * violating the red-black tree properties(red nodes do not have
     * red children).
     */

    while (node->parent && node->parent->isRed){
        if (node->parent->parent && (node->parent == node->parent->parent->left)){
            Node *uncle = node->parent->parent->right;
            if (uncle && uncle->isRed){
                node->parent->isRed = false;
                uncle->isRed = false;
                node->parent->parent->isRed = true;
                node = node->parent->parent;
            } else {
                if (node == node->parent->right){
                    node = node->parent;
                    RotateLeft(node);
                }
                node->parent->isRed = false;
                node->parent->parent->isRed = true;
                RotateRight(node->parent->parent);
            }
        } else {
            if (!node->parent->parent) break;
            Node *uncle = node->parent->parent->left;
            if (uncle && uncle->isRed){
                node->parent->isRed = false;
                uncle->isRed = false;
                node->parent->parent->isRed = true;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left){
                    node = node->parent;
                    RotateRight(node);
                }
                node->parent->isRed = false;
                node->parent->parent->isRed = true;
                RotateLeft(node->parent->parent);
            }
        }
    }

    this->root->isRed = false;
}

void RBTree::Transplant(Node *oldNode, Node *newNode) {
    /*
     * This function transplants subtrees. Subtree with root newNode
     * will replace the subtree with root oldNode. Information about oldNode
     * is deleted after this operation.
     */

    if (oldNode == root){
        root = newNode;
    } else if (oldNode == oldNode->parent->left){
        oldNode->parent->left = newNode;
    } else {
        oldNode->parent->right = newNode;
    }

    if (newNode != nullptr) {
        newNode->parent = oldNode->parent;
    }
}

void RBTree::Delete(Node *node) {
    /*
     * This function deletes the given node from the tree.
     *
     * temp either represents the node that is going to be deleted
     * or the node that is going to replace the deleted node.
     *
     * replace represents the node that is going to replace temp.
     *
     * After doing the operations to make sure wo do not violate
     * binary search tree properties, we call DeleteFix if deleted node
     * or node that replaced the deleted node was originally black.
     * Otherwise, we might have different number of black nodes in different
     * paths that lead to leaves which would violate the red-black tree
     * properties.
     */
    Node *temp = node;
    Node *replace = nullptr;
    bool originalIsRed = temp->isRed;
    if (node->left == nullptr){
        replace = node->right;
        Transplant(node, node->right);
    } else if (node->right == nullptr){
        replace = node->left;
        Transplant(node, node->left);
    } else {
        temp = TreeMinimum(node->right);
        originalIsRed = temp->isRed;
        replace = temp->right;
        if (temp->parent == node){
            if (replace) replace->parent = temp;
        } else {
            Transplant(temp, temp->right);
            temp->right = node->right;
            temp->right->parent = temp;
        }
        Transplant(node, temp);
        temp->left = node->left;
        temp->left->parent = temp;
        temp->isRed = node->isRed;
    }

    if(!(originalIsRed))
        DeleteFix(replace);
}

void RBTree::DeleteFix(Node *node) {
    Node *sibling;
    while (node && (node != root && !node->isRed)){
        if (node->parent && (node == node->parent->left)){
            sibling = node->parent->right;
            if (sibling && sibling->isRed){
                sibling->isRed = false;
                node->parent->isRed = true;
                RotateLeft(node->parent);
                sibling = node->parent->right;
            }
            if ((!sibling->left || !sibling->left->isRed) && (!sibling->right || !sibling->right)){
                sibling->isRed = true;
                node = node->parent;
            } else {
                if (!sibling->right || !sibling->right->isRed){
                    if (sibling->left) sibling->left->isRed = false;
                    sibling->isRed = true;
                    RotateRight(sibling);
                    sibling = node->parent->right;
                }
                sibling->isRed = node->parent->isRed;
                node->parent->isRed = false;
                sibling->right->isRed = false;
                RotateLeft(node->parent);
                node = root;
            }
        } else {
            sibling = node->parent->left;
            if (sibling && sibling->isRed){
                sibling->isRed = false;
                node->parent->isRed = true;
                RotateLeft(node->parent);
                sibling = node->parent->left;
            }
            if ((sibling->right || !sibling->right->isRed) && (sibling->left || !sibling->left->isRed)){
                sibling->isRed = true;
                node = node->parent;
            } else {
                if (!sibling->left || !sibling->left->isRed) {
                    if (sibling->right) sibling->right->isRed = false;
                    sibling->isRed = true;
                    RotateRight(sibling);
                    sibling = node->parent->left;
                }
                sibling->isRed = node->parent->isRed;
                node->parent->isRed = false;
                sibling->left->isRed = false;
                RotateLeft(node->parent);
                node = root;
            }
        }
    }
    if (node) node->isRed = false;
}

void RBTree::InOrderWalk(Node *node) {
    //if (!node) std::cout << "nil" << std::endl;
    if(node != nullptr) {

        //std::cout << node->key << "'s left: ";
        InOrderWalk(node->left);
        std::cout << node->key << "\t" << (node->isRed ? "Red" : "Black") << std::endl;
        //std::cout << node->key << "'s right: ";
        InOrderWalk(node->right);
    }
}

Node *RBTree::TreeMinimum(Node *begin) {
    Node *retNode = begin;
    Node *traverse = begin;
    while(traverse){
        retNode = traverse;
        traverse = traverse->left;
    }
    return retNode;
}

RBTree::RBTree() {
    root = nullptr;
}


int main() {
    /*RBTree tree;
    Node *arr[15] = {new Node(974), new Node(834), new Node(204), new Node(463), new Node(332),
                     new Node(293), new Node(706), new Node(126), new Node(980), new Node(630),
                     new Node(718), new Node(760), new Node(76), new Node(607), new Node(805)};

    for (int i = 0; i < 15; i++){
        arr[i] = new Node((rand() % 1000 + 1));
    }

    for (int i = 0; i < 15; i++){
        tree.Insert(arr[i]);
    }

    std::cout << "Array" << std::endl;
    for (int i = 0; i < 15; i++){
        std::cout << arr[i]->key << "\t";
    }
    std::cout << std::endl;

    std::cout << "Tree" << std::endl;
    tree.InOrderWalk(tree.root);

    std::cout << "Deleting: " << arr[0]->key << " and " << arr[3]->key << std::endl;
    tree.Delete(arr[0]);

    tree.Delete(arr[3]);
    std::cout << "Tree After Delete 1:" << std::endl;
    tree.InOrderWalk(tree.root);
    tree.Delete(arr[1]);
    std::cout << "Tree After Delete 2:" << std::endl;
    tree.InOrderWalk(tree.root);
    tree.Delete(arr[11]);
    std::cout << "Tree After Delete 3:\n\n" << std::endl;
    tree.InOrderWalk(tree.root);*/

    return 0;
}