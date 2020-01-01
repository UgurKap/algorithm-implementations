#include <iostream>
#include <vector>
#include "hw3.h"

class Node{
public:
    bool isRed;
    int key, size;
    int ordinal;
    Node *parent;
    Node *left;
    Node *right;
    Node(int key=0, int ordinal=0);
};

Node::Node(int key, int ordinal) {
    this->key = key;
    this->ordinal = ordinal;
    this->size = 1;
    this->parent = nullptr;
    this->left = nullptr;
    this->right = nullptr;
    this->isRed = false;
}

class RBTree{
public:
    int elementCount;
    int redElements;
    int blackElements;
    Node *root;
    // Necessary nodes for delete fix(So we have no memory leak)
    Node *replacingNode;
    Node *siblingNode;
    void RotateLeft(Node *node);
    void RotateRight(Node *node);
    void Insert(Node *node);
    void InsertFix(Node *node);
    void Transplant(Node *oldNode, Node *newNode);
    void Delete(Node *node);
    void DeleteFix(Node *node, bool rep);
    void InOrderWalk(Node *node);
    Node *Select(Node *begin, int i);
    Node* TreeMinimum(Node *begin);
    void CountElements(Node *begin);
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
        temp->size = node->size;
        int s = 1;
        if (node->left) s += node->left->size;
        if (node->right) s += node->right->size;
        node->size = s;
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
        temp->size = node->size;
        int s = 1;
        if (node->left) s += node->left->size;
        if (node->right) s += node->right->size;
        node->size = s;
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
    elementCount += 1;
    Node *temp = nullptr;
    Node *traverse = this->root;
    while (traverse){
        traverse->size += 1;
        temp = traverse;
        if (node->key < traverse->key){
            traverse = traverse->left;
        } else if (node->key == traverse->key){
            if (node->ordinal < traverse->ordinal){
                traverse = traverse->left;
            } else traverse = traverse->right;
        } else {
            traverse = traverse->right;
        }
    }

    node->parent = temp;

    if (temp == nullptr){
        this->root = node;
    } else if (node->key < temp->key){
        temp->left = node;
    } else if (node->key == temp->key){
        if (node->ordinal < temp->ordinal){
            temp->left = node;
        } else {
            temp->right = node;
        }
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


    if (newNode) {
        newNode->size = oldNode->size;
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
    elementCount -= 1;
    Node *temp = node;
    Node *replace = nullptr;
    bool originalIsRed = temp->isRed;
    bool rep = false;

    if (node->left == nullptr){
        replace = node->right;
        Node *traverse = temp->parent;
        while(traverse){
            traverse->size -= 1;
            traverse = traverse->parent;
        }

        if (!replace){
            rep = true;
            // If there is no replacing node, we have to create an artificial one
            replace = new Node(0, 0);
            replace->size = 0;
            this->replacingNode = replace;
            replace->isRed = false;
            replace->parent = node;
            replace->parent->right = replace;
        }

        Transplant(node, node->right);

    } else if (node->right == nullptr){
        Node *traverse = temp->parent;
        while(traverse){
            traverse->size -= 1;
            traverse = traverse->parent;
        }
        replace = node->left; // Since we check the left child first, there is no need to create an artificial node
        Transplant(node, node->left);
    } else {
        temp = TreeMinimum(node->right);
        Node *traverse = temp->parent;
        while(traverse){
            traverse->size -= 1;
            traverse = traverse->parent;
        }
        originalIsRed = temp->isRed;
        replace = temp->right;
        if (!replace){
            rep = true;
            // If there is no replacing node, we have to create an artificial one
            replace = new Node(0, 0);
            replace->size = 0;
            this->replacingNode = replace;
            replace->isRed = false;
            replace->parent = temp;
            replace->parent->right = replace;
        }
        if (temp->parent == node){
            replace->parent = temp;
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
        DeleteFix(replace, rep);

    if (rep){
        rep = false;
        if(!replacingNode->parent){
            delete replacingNode;
        } else if (replacingNode->parent->left == replacingNode){
            replacingNode->parent->left = nullptr;
            delete replacingNode;
        } else if (replacingNode->parent->right == replacingNode) {
            replacingNode->parent->right = nullptr;
            delete replacingNode;
        }
    }
}

void RBTree::DeleteFix(Node *node, bool rep) {

    Node *sibling = nullptr;
    bool siblingChange = false;
    bool replaceChange = rep;

    while (node != root && !node->isRed){
        if (node == node->parent->left){
            sibling = node->parent->right;
            if (!sibling){
                siblingChange = true;
                sibling = new Node(0, 0);
                sibling->size = 0;
                this->siblingNode = sibling;
                sibling->parent = node->parent;
                sibling->isRed = false;
                node->parent->right = sibling;
            }
            if (sibling->isRed){
                sibling->isRed = false;
                node->parent->isRed = true;
                RotateLeft(node->parent);
                sibling = node->parent->right;
            }
            if ((!sibling->left || !sibling->left->isRed) && (!sibling->right || !sibling->right->isRed)){
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
            if (!sibling){
                siblingChange = true;
                sibling = new Node(0, 0);
                sibling->size = 0;
                this->siblingNode = sibling;
                sibling->parent = node->parent;
                sibling->isRed = false;
                node->parent->left = sibling;
            }

            if (sibling->isRed){
                sibling->isRed = false;
                node->parent->isRed = true;
                RotateRight(node->parent);
                sibling = node->parent->left;
            }

            if ((!sibling->right || !sibling->right->isRed) && (!sibling->left || !sibling->left->isRed)){
                sibling->isRed = true;
                node = node->parent;
            } else {
                if (!sibling->left || !sibling->left->isRed) {
                    if (sibling->right) sibling->right->isRed = false;
                    sibling->isRed = true;
                    RotateLeft(sibling);
                    sibling = node->parent->left;
                }
                sibling->isRed = node->parent->isRed;
                node->parent->isRed = false;
                sibling->left->isRed = false;
                RotateRight(node->parent);
                node = root;
            }
        }

        if (replaceChange){
            replaceChange = false;
            if(!replacingNode->parent){
                delete replacingNode;
            } else if (replacingNode->parent->left == replacingNode){
                replacingNode->parent->left = nullptr;
                delete replacingNode;
            } else if (replacingNode->parent->right == replacingNode) {
                replacingNode->parent->right = nullptr;
                delete replacingNode;
            }
        }
        if (siblingChange){
            siblingChange = false;
            if(!siblingNode->parent){
                delete siblingNode;
            } else if (siblingNode->parent->left == siblingNode){
                siblingNode->parent->left = nullptr;
                delete siblingNode;
            } else if (siblingNode->parent->right == siblingNode){
                siblingNode->parent->right = nullptr;
                delete siblingNode;
            }
        }
    }

    if (node) node->isRed = false;
}

void RBTree::InOrderWalk(Node *node) {
    //if (!node) std::cout << "nil" << std::endl;
    if(node != nullptr) {

        //std::cout << node->key <<", " << node->ordinal << ", " << node->size<< "'s left: ";
        InOrderWalk(node->left);
        std::cout << "(" <<node->key << ", "<< node->ordinal << ", " << node->size << ")\t" << (node->isRed ? "Red" : "Black") << std::endl;
        //std::cout << node->key  <<", " << node->ordinal << ", " << node->size<< "'s right: ";
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
    root = replacingNode = siblingNode = nullptr;
    elementCount = blackElements = redElements = 0;

}

Node *RBTree::Select(Node *begin, int i) {
    int s = 1;
    if(!begin) return nullptr;
    if (begin->left) s += begin->left->size;
    int rank = s;
    if (i == rank){
        return begin;
    } else if (i < rank){
        return Select(begin->left, i);
    } else {
        return Select(begin->right, i - rank);
    }
}

void RBTree::CountElements(Node *begin) {
    if(begin != nullptr) {
        CountElements(begin->left);
        if (begin->isRed) redElements += 1;
        else blackElements += 1;
        CountElements(begin->right);
    }
}

class Warehouse{
public:
    RBTree storage;
    Node *arr;
    int minOrdinal;
    bool accept;
    std::vector<Node*> cargo;
    Warehouse(int *sizes, int *ordinals, int count);
    void sendPackages(int i, bool start=false);
    void receivePackages(std::vector<Node*> packages);
    void findMinimum();
};

Warehouse::Warehouse(int *sizes, int *ordinals, int count) {
    this->arr = new Node[count];
    accept = true;

    for (int i = 0; i < count; i++){
        arr[i].key = sizes[i];
        arr[i].ordinal = ordinals[i];
        storage.Insert(&arr[i]);
    }
}

void Warehouse::findMinimum() {
    int minSize = storage.TreeMinimum(storage.root)->key;
    while (minSize == storage.TreeMinimum(storage.root)->key){
        cargo.push_back(storage.TreeMinimum(storage.root));
        storage.Delete(storage.TreeMinimum(storage.root));
    }
}

void Warehouse::sendPackages(int i, bool start) {

    if (!(storage.Select(storage.root, i))){
        findMinimum();
        if (!start) accept = false;
    } else {
        cargo.push_back(storage.Select(storage.root, i));

        //storage.InOrderWalk(storage.root);
        Node * temp = storage.Select(storage.root, i);
        storage.Delete(temp);
        //storage.InOrderWalk(storage.root);
    }
}

void Warehouse::receivePackages(std::vector<Node*> packages) {
    minOrdinal = packages[0]->ordinal;

    for(auto it = packages.begin(); it != packages.end(); ++it){
        if ((*it)->ordinal < minOrdinal) minOrdinal = (*it)->ordinal;
    }

    cargo.clear();
    sendPackages(minOrdinal);

    if (accept){
        for(auto it = packages.begin(); it != packages.end(); ++it){
            storage.Insert(*it);
        }
    }

    accept = true;
}

HW3_Result hw3(  int eastWarehousePackageCount,
                 int eastWarehousePackageSizes [],
                 int eastWarehousePackageOrdinals [],
                 int westWarehousePackageCount,
                 int westWarehousePackageSizes [],
                 int westWarehousePackageOrdinals [] ){

    Warehouse East(eastWarehousePackageSizes, eastWarehousePackageOrdinals, eastWarehousePackageCount);
    Warehouse West(westWarehousePackageSizes, westWarehousePackageOrdinals, westWarehousePackageCount);

    East.sendPackages(0, true);
    std::vector<Node*> cargo = East.cargo;

    int packageCount = 0;
    int redNodeCount = 0;
    int blackNodeCount = 0;

    while(true){
        West.receivePackages(cargo);
        cargo = West.cargo;
        if(West.storage.elementCount == 0) {
            for(auto it = cargo.begin(); it != cargo.end(); ++it){
                East.storage.Insert(*it);
            }
            packageCount = East.storage.elementCount;
            East.storage.CountElements(East.storage.root);
            redNodeCount = East.storage.redElements;
            blackNodeCount =  East.storage.blackElements;
            break;
        }

        East.receivePackages(cargo);
        cargo = East.cargo;
        if(East.storage.elementCount == 0) {
            for(auto it = cargo.begin(); it != cargo.end(); ++it){
                West.storage.Insert(*it);
            }
            packageCount = West.storage.elementCount;
            West.storage.CountElements(West.storage.root);
            redNodeCount = West.storage.redElements;
            blackNodeCount =  West.storage.blackElements;
            break;
        }
    }

    HW3_Result result;
    result.blackNodeCount = blackNodeCount;
    result.redNodeCount = redNodeCount;
    result.packageCount = packageCount;
    return result;
}

int main() {
    int eastWarehousePackageCount = 10;
    int eastWarehousePackageSizes[10] = {1, 5, 34, 6, 7, 23, 5, 6, 34, 5};
    int eastWarehousePackageOrdinals[10] = {7, 43, 59, 80, 925, 89, 27, 45, 89, 72};

    int westWarehousePackageCount = 10;
    int westWarehousePackageSizes[10] = {10, 38, 51, 7, 38, 47, 8, 53, 2, 94};
    int westWarehousePackageOrdinals[10] = {5, 62, 86, 73, 74, 83, 68, 35, 7, 95};

    HW3_Result result = hw3(eastWarehousePackageCount, eastWarehousePackageSizes, eastWarehousePackageOrdinals,
            westWarehousePackageCount, westWarehousePackageSizes, westWarehousePackageOrdinals);


    std::cout << "Package Count: " << result.packageCount << std::endl;
    std::cout << "Red Node Count: " << result.redNodeCount << std::endl;
    std::cout << "Black Node Count: " << result.blackNodeCount << std::endl;

    return 0;
}