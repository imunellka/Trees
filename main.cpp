#include <iostream>
#include <utility>
#include <algorithm>

enum Color { RED, BLACK };

template <typename K, typename V>
struct Node {
    K key;
    V value;
    Node<K, V> *left = nullptr;
    Node<K, V> *right = nullptr;
    Node<K, V> *parent = nullptr;
    Color color = RED;
    std::pair<K, V> pair;

    Node();

    Node(const K &key, const V &value) {
        this->key = key;
        this->value = value;
    }

    Node(K key, V value, Node *left, Node *right, Node *parent, Color color) {
        this->key = key;
        this->value = value;
        this->left = left;
        this->right = right;
        this->parent = parent;
        this->color = color;
    }
};

template <typename K, typename V>
class Map {
public:
    struct Iterator {
        Iterator() {
        }

        explicit Iterator(Node<K, V> *node) : node_(node) {
        }

        const std::pair<K, V> &operator*() const {
            node_->pair = std::make_pair(node_->key, node_->value);
            return node_->pair;
        }

        const std::pair<K, V> *operator->() const {
            node_->pair = std::make_pair(node_->key, node_->value);
            return &(node_->pair);
        }

        Iterator &operator++() {
            Node<K, V> *x = node_;
            if (x->right) {
                x = min(x->right);
            } else {
                x = node_->parent;
                while (x && x->key < node_->key) {
                    x = x->parent;
                }
            }
            node_ = x;
            return *this;
        }

        Iterator operator++(int) {
            Iterator prom = Iterator{node_};
            ++*this;
            return prom;
        }

        Iterator &operator--() {
            Node<K, V> *x = node_;
            if (x->left) {
                x = max(x->left);
            } else {
                x = node_->parent;
                while (x && node_->key < x->key) {
                    x = x->parent;
                }
            }
            node_ = x;
            return *this;
        }

        Iterator operator--(int) {
            Iterator prom = Iterator{node_};
            --*this;
            return prom;
        }

        bool operator==(const Iterator &other) const {
            return node_ == other.node_;
        }

        bool operator!=(const Iterator &other) const {
            return node_ != other.node_;
        }

        Node<K, V> *max(Node<K, V> *p) {
            return p->right ? max(p->right) : p;
        }

        Node<K, V> *min(Node<K, V> *p) {
            return p->left ? min(p->left) : p;
        }

    private:
        Node<K, V> *node_;
    };

    Map() = default;

    Map(std::initializer_list<std::pair<K, V>> list) {
        for (std::pair<K, V> a : list) {
            this->insert(a.first, a.second);
        }
    }

    Map(const Map &other) {
        root = new Node<K, V>(other.root->key, other.root->value, nullptr, nullptr, nullptr,
                              other.root->color);
        ch_ = other.size();
        copy(root, other.root->left, other.root->right);
    }

    void copy(Node<K, V> *p, Node<K, V> *l, Node<K, V> *r) {
        if (l) {
            p->left = new Node<K, V>(l->key, l->value, nullptr, nullptr, p, l->color);
            copy(p->left, l->left, l->right);
        }
        if (r) {
            p->right = new Node<K, V>(r->key, r->value, nullptr, nullptr, p, r->color);
            copy(p->right, r->left, r->right);
        }
    }

    Map<K, V> &operator=(const Map &other) {
        free(root);
        ch_ = other.size();
        root = new Node<K, V>(other.root->key, other.root->value, nullptr, nullptr, nullptr,
                              other.root->color);
        copy(root, other.root->left, other.root->right);
        return *this;
    }

    ~Map() {
        free(root);
    }

    void free(Node<K, V> *begin) {
        if (!begin) {
            return;
        }
        free(begin->left);
        free(begin->right);
        delete begin;
    }

    void insert(const K &key, const V &value) {
        if (!this->root) {
            ch_++;
            this->root = new Node<K, V>(key, value, nullptr, nullptr, nullptr, Color::BLACK);
        } else {
            insertp(nullptr, this->root, key, value);
        }
    }

    void erase(const K &key) {
        Node<K, V> *node = findp(key, root);
        if (node) {
            erasep(root, node);
        }
    }

    size_t size() const {
        return ch_;
    }

    bool empty() const {
        if (ch_ < 1) {
            return true;
        }
        return false;
    }

    Iterator lowerBound(const K &key) const {
        Node<K, V> *a = root;
        Node<K, V> *res = nullptr;
        while (a != nullptr) {
            if (!(a->key < key || key < a->key)) {
                return Iterator(a);
            }
            if (key < a->key) {
                res = a;
                a = a->left;
            } else {
                a = a->right;
            }
        }
        return res ? Iterator(res) : end();
    }

    Iterator find(const K &key) const {
        Node<K, V> *tmp = findp(key, root);
        if (tmp == nullptr) {
            return end();
        }
        return Iterator(tmp);
    }

    Iterator begin() const {
        return Iterator(min(root));
    }

    Iterator end() const {
        return Iterator((max(root)->right));
    }

    Node<K, V> *root = nullptr;

private:
    int ch_ = 0;

    void erasep(Node<K, V> *root, Node<K, V> *node) {
        Node<K, V> *child = nullptr, *parent;
        Color color;
        if (node->left && node->right) {
            Node<K, V> *swap_n = min(node->right);
            daddyIssues(node, swap_n);
            child = swap_n->right;
            parent = swap_n->parent;
            color = swap_n->color;
            if (parent == node) {
                parent = swap_n;
            } else {
                if (child) {
                    child->parent = parent;
                }
                parent->left = child;
                swap_n->right = node->right;
                node->right->parent = swap_n;
            }
            swap_n->color = node->color;
            swap_n->left = node->left;
            swap_n->parent = node->parent;
            node->left->parent = swap_n;
            finite(node, child, parent, color);
            return;
        }
        parent = node->parent;
        if (node->left) {
            child = node->left;
        }
        if (node->right) {
            child = node->right;
        }
        color = node->color;
        if (child) {
            child->parent = parent;
        }
        daddyIssues(node, child);
        finite(node, child, parent, color);
    }

    Node<K, V> *findp(const K &key, Node<K, V> *t) const {
        while (t != nullptr) {
            if (key < t->key) {
                t = t->left;
            } else if (t->key < key) {
                t = t->right;
            } else {
                return t;
            }
        }
        return nullptr;
    }

    Node<K, V> *max(Node<K, V> *p) const {
        return p->right ? max(p->right) : p;
    }

    Node<K, V> *min(Node<K, V> *p) const {
        return p->left ? min(p->left) : p;
    }

    void insertp(Node<K, V> *pred, Node<K, V> *p, K key, V value) {
        if (!p) {
            p = new Node<K, V>(key, value, nullptr, nullptr, nullptr, Color::RED);
            ch_++;
            whoIsYourDaddy(pred, p);
            if (pred->color == Color::RED) {
                this->balance(p);
            }
            return;
        } else if (key < p->key) {
            return this->insertp(p, p->left, key, value);
        } else if (p->key < key) {
            return this->insertp(p, p->right, key, value);
        } else if (!(key < p->key || p->key < key)) {
            p->value = value;
        }
    }

    void finite(const Node<K, V> *node, Node<K, V> *child, Node<K, V> *parent, const Color &color) {
        if (color == Color::BLACK) {
            balanceErase(child, parent);
        }
        delete node;
        ch_--;
    }

    void balanceErase(Node<K, V> *node, Node<K, V> *parent) {
        Node<K, V> *brother;
        while (node != root && (!node || node->color == Color::BLACK)) {
            if (parent->left == node) {
                brother = parent->right;
                if (brother && brother->color == Color::RED) {
                    leftRotateWithoutColor(parent);
                    brother->color = Color::BLACK;
                    parent->color = Color::RED;
                    brother = parent->right;
                }
                if (!brother->left || brother->left->color == Color::BLACK) {
                    if (!brother->right || brother->right->color == Color::BLACK) {
                        brother->color = RED;
                        node = parent;
                        parent = node->parent;
                    } else {
                        if (!brother->right || brother->right->color == Color::BLACK) {
                            rightRotate(brother);
                            brother = parent->right;
                        }
                        leftRotateWithoutColor(parent);
                        blackSwapR(parent, brother);
                        node = root;
                    }
                } else {
                    if (!brother->right || brother->right->color == Color::BLACK) {
                        rightRotate(brother);
                        brother = parent->right;
                    }
                    leftRotateWithoutColor(parent);
                    blackSwapR(parent, brother);
                    node = root;
                }
            } else {
                brother = parent->left;
                if (brother && brother->color == Color::RED) {
                    rightRotate(parent);
                    brother = parent->left;
                }
                if (!brother->left || brother->left->color == Color::BLACK) {
                    if (!brother->right || brother->right->color == Color::BLACK) {
                        brother->color = RED;
                        node = parent;
                        parent = node->parent;
                    } else {
                        if (!brother->left || brother->left->color == Color::BLACK) {
                            leftRotate(brother);
                            brother = parent->left;
                        }
                        rightRotateWithoutColor(parent);
                        blackSwapL(parent, brother);
                        node = root;
                    }
                } else {
                    if (!brother->left || brother->left->color == Color::BLACK) {
                        leftRotate(brother);
                        brother = parent->left;
                    }
                    rightRotateWithoutColor(parent);
                    blackSwapL(parent, brother);
                    node = root;
                }
            }
        }
        if (node) {
            node->color = BLACK;
        }
    }

    void whoIsYourDaddy(Node<K, V> *pred, Node<K, V> *p) {
        if (p->key < pred->key) {
            pred->left = p;
        } else {
            pred->right = p;
        }
        p->parent = pred;
    }

    void balance(Node<K, V> *p) {
        while (p->parent && p->parent->parent && Color::RED == p->parent->color) {
            Node<K, V> *daddy = p->parent;
            Node<K, V> *grandpa = p->parent->parent;
            if (grandpa->left == daddy) {
                Node<K, V> *uncle = grandpa->right;
                if (uncle && Color::RED == uncle->color) {
                    colorSwap(daddy, uncle, grandpa);
                    p = grandpa;
                } else {
                    if (daddy->right == p) {
                        leftRotateWithoutColor(daddy);
                        p = daddy;
                        daddy = p->parent;
                    }
                    daddy->color = BLACK;
                    grandpa->color = RED;
                    rightRotateWithoutColor(grandpa);
                }
            } else {
                Node<K, V> *uncle = grandpa->left;
                if (uncle && Color::RED == uncle->color) {
                    colorSwap(daddy, uncle, grandpa);
                    p = grandpa;
                } else {
                    if (daddy->left == p) {
                        rightRotateWithoutColor(daddy);
                        p = daddy;
                        daddy = p->parent;
                    }
                    daddy->color = BLACK;
                    grandpa->color = RED;
                    leftRotateWithoutColor(grandpa);
                }
            }
        }
        this->root->color = Color::BLACK;
    }

    void blackSwapR(Node<K, V> *parent, Node<K, V> *brother) const {
        brother->color = parent->color;
        parent->color = BLACK;
        brother->right->color = BLACK;
    }

    void blackSwapL(Node<K, V> *parent, Node<K, V> *brother) const {
        brother->color = parent->color;
        parent->color = BLACK;
        brother->left->color = BLACK;
    }

    void colorSwap(Node<K, V> *pl, Node<K, V> *pr, Node<K, V> *p) {
        pl->color = Color::BLACK;
        pr->color = Color::BLACK;
        p->color = Color::RED;
    }

    Node<K, V> *leftRotate(Node<K, V> *p) {
        Node<K, V> *child = p->right;
        p->right = child->left;
        if (child->left) {
            child->left->parent = p;
        }
        child->parent = p->parent;
        daddyIssues(p, child);
        child->left = p;
        p->parent = child;
        child->color = Color::BLACK;
        p->color = Color::RED;
        return child;
    }

    Node<K, V> *leftRotateWithoutColor(Node<K, V> *p) {
        Node<K, V> *child = p->right;
        p->right = child->left;
        if (child->left) {
            child->left->parent = p;
        }
        child->parent = p->parent;
        daddyIssues(p, child);
        child->left = p;
        p->parent = child;
        return child;
    }

    void daddyIssues(Node<K, V> *p, Node<K, V> *child) {
        if (!p->parent) {
            this->root = child;
        } else {
            if (p->parent->left == p) {
                p->parent->left = child;
            } else {
                p->parent->right = child;
            }
        }
    }

    Node<K, V> *rightRotate(Node<K, V> *p) {
        Node<K, V> *child = p->left;
        p->left = child->right;
        if (child->right) {
            child->right->parent = p;
        }
        child->parent = p->parent;
        daddyIssues(p, child);
        child->right = p;
        p->parent = child;
        child->color = Color::BLACK;
        p->color = Color::RED;
        return child;
    }

    Node<K, V> *rightRotateWithoutColor(Node<K, V> *p) {
        Node<K, V> *child = p->left;
        p->left = child->right;
        if (child->right) {
            child->right->parent = p;
        }
        child->parent = p->parent;
        daddyIssues(p, child);
        child->right = p;
        p->parent = child;
        return child;
    }
};