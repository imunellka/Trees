#include <iostream>
#include <algorithm>

struct Node {
    int height;
    Node *left = nullptr;
    Node *right = nullptr;
    int value;
};

class AVLTree {
public:
    AVLTree() {
    }

    int getHeight() {
        return height(root_);
    }

    int height(Node *p) {
        if (p == nullptr) {
            return 0;
        }
        return p->height;
    }

    Node *insert(int x, Node *t) {
        if (t == nullptr) {
            ch_++;
            t = new Node();
            t->value = x;
            t->height = 1;
            t->left = t->right = nullptr;
        } else if (x < t->value) {
            t->left = insert(x, t->left);
        } else if (x > t->value) {
            t->right = insert(x, t->right);
        }
        t->height = std::max(height(t->left), height(t->right)) + 1;
        return balance(t);
    }

    void insert(int x) {
        if (x == 0) {
            return;
        }
        root_ = insert(x, root_);
    }

    Node *erase(int value, Node *t) {
        if (t == nullptr) {
            return nullptr;
        }
        if (value < t->value) {
            t->left = erase(value, t->left);
        } else if (value > t->value) {
            t->right = erase(value, t->right);
        } else {
            Node *l = t->left;
            Node *r = t->right;
            delete t;
            ch_--;
            if (r == nullptr) {
                return l;
            }
            Node *min = r;
            while (min->left != nullptr) {
                min = min->left;
            }
            min->right = rWithoutMin(r);
            min->left = l;
            min->height = std::max(height(min->left), height(min->right)) + 1;
            return balance(min);
        }
        t->height = std::max(height(t->left), height(t->right)) + 1;
        return balance(t);
    }

    Node *rWithoutMin(Node *t) {
        if (t->left == nullptr) {
            return t->right;
        }
        t->left = rWithoutMin(t->left);
        return balance(t);
    }

    void erase(int value) {
        root_ = erase(value, root_);
    };

    Node *rightRotate(Node *t) {
        Node *netmp = t->left;
        t->left = netmp->right;
        netmp->right = t;
        t->height = std::max(height(t->left), height(t->right)) + 1;
        netmp->height = std::max(height(netmp->left), height(netmp->right)) + 1;
        return netmp;
    }

    Node *leftRotate(Node *t) {
        Node *netmp = t->right;
        t->right = netmp->left;
        netmp->left = t;
        t->height = std::max(height(t->left), height(t->right)) + 1;
        netmp->height = std::max(height(netmp->left), height(netmp->right)) + 1;
        return netmp;
    }

    Node *balance(Node *t) {
        if (height(t->right) - height(t->left) == 2) {
            if (height(t->right->right) - height(t->right->left) < 0) {
                t->right = rightRotate(t->right);
            }
            return leftRotate(t);
        }
        if (height(t->left) - height(t->right) == 2) {
            if (height(t->left->right) - height(t->left->left) > 0) {
                t->left = leftRotate(t->left);
            }
            return rightRotate(t);
        }
        return t;
    }

    Node *find(int value, Node *t) {
        while (t != nullptr) {
            if (value < t->value) {
                t = t->left;
            } else if (value > t->value) {
                t = t->right;
            } else {
                return t;
            }
        }
        return nullptr;
    }

    int *find(int value) {
        Node *tmp = find(value, root_);
        if (tmp == nullptr) {
            return nullptr;
        }
        return &(tmp->value);
    }

    int *traversal() {
        a_ = new int[ch_];
        i_ = 0;
        traversal(root_);
        return a_;
    }

    void traversal(Node *t) {
        if (t == nullptr) {
            return;
        }
        traversal(t->left);
        a_[i_++] = t->value;
        traversal(t->right);
    }

    int *lowerBound(int value) {
        Node *a = root_;
        Node *res = nullptr;
        while (a != nullptr) {
            if (a->value >= value) {
                res = a;
                a = a->left;
            } else {
                a = a->right;
            }
        }
        return res ? &(res->value) : nullptr;
    }

    bool empty() {
        if (ch_ == 0) {
            return true;
        }
        return false;
    }

    Node *getRoot() {
        return root_;
    }

    int getSize() {
        return ch_;
    }

    ~AVLTree() {
        free(root_);
    }

    void free(Node *begin) {
        if (!begin) {
            return;
        }
        free(begin->left);
        free(begin->right);
        delete begin;
    }

private:
    Node *root_ = nullptr;
    int ch_ = 0;
    int *a_;
    int i_ = 0;
};

int main() {
    AVLTree *avl_tree = new AVLTree();
    avl_tree->insert(1);
    avl_tree->insert(2);
    avl_tree->insert(3);
    avl_tree->insert(4);
    avl_tree->insert(5);
    avl_tree->insert(9);
    avl_tree->insert(7);
    avl_tree->insert(10);
    avl_tree->erase(5);
    std::cout << avl_tree->getHeight() << " - высота" << std::endl;
    std::cout << avl_tree->getSize() << " - размер" << std::endl;
    auto s = avl_tree->lowerBound(10);
    if (s == nullptr) std::cout << avl_tree->getSize() << " - размер" << std::endl;
    delete avl_tree;
    return 0;
}
