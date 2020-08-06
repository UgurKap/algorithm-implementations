#include <iostream>

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
    

    if (newNode != nullptr) {
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
    Node *temp = node;
    Node *replace = nullptr;
    bool originalIsRed = temp->isRed;
    bool rep = false;

    if (node->left == nullptr){
        replace = node->right;
        Node *traverse = temp;
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
    if (!node) std::cout << "nil" << std::endl;
    if(node != nullptr) {

        std::cout << node->key <<", " << node->ordinal << ", " << node->size<< "'s left: ";
        InOrderWalk(node->left);
        std::cout << "(" <<node->key << ", "<< node->ordinal << ", " << node->size << ")\t" << (node->isRed ? "Red" : "Black") << std::endl;
        std::cout << node->key  <<", " << node->ordinal << ", " << node->size<< "'s right: ";
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

int main() {
    RBTree tree;
    Node *arr[15] = {new Node(974), new Node(834), new Node(204), new Node(463), new Node(332),
                     new Node(293), new Node(706), new Node(126), new Node(980), new Node(630),
                     new Node(718), new Node(760), new Node(76), new Node(607), new Node(805)};
    for(int i = 0; i < 15; i++){
        tree.Insert(arr[i]);
    }

    // std::cout << tree.Select(tree.root, 5)->key;
    tree.Delete(arr[3]);
    tree.Delete(arr[1]);
    tree.Delete(arr[11]);
    tree.InOrderWalk(tree.root);

    return 0;
}
