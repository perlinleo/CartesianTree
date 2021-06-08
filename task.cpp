// Copyright 2021 Leonid Perlin

#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>
#include <stack>
#include <vector>

template <class T>
struct DefaultComparator {
  bool operator()(const T &l, const T &r) { return l < r; }
};

class ITree {
 protected:
  template <class Node>
  int countWidth(Node *&root) {
    if (!root) {
      return 0;
    }
    std::queue<Node *> nodes;
    nodes.push(root);
    int currentLayerW = 1, nextLayerW = 0, maxWidth = 0;
    while (!nodes.empty()) {
      for (int i = 0; i < currentLayerW; i++) {
        Node *curr = nodes.front();
        nodes.pop();
        if (curr->right) {
          nodes.push(curr->right);
          nextLayerW++;
        }
        if (curr->left) {
          nodes.push(curr->left);
          nextLayerW++;
        }
      }
      if (nextLayerW > maxWidth) {
        maxWidth = nextLayerW;
      }
      currentLayerW = nextLayerW;
      nextLayerW = 0;
    }
    return std::max(currentLayerW, maxWidth);
  }
};

template <class T, class Comparator = DefaultComparator<T>>
class Tree : public ITree {
  struct Node {
    T value;
    Node *left;
    Node *right;

    Node() : left(nullptr), right(nullptr), value(0) {}

    explicit Node(T &value) : value(value), left(nullptr), right(nullptr) {}
  };

 public:
  Tree() : root(nullptr){};
  ~Tree() {
    _postOrder(root, [](Node *node) { delete node; });
  }

  void add(T &value) {
    if (!root) {
      root = new Node(value);
      return;
    }
    Node *curr = root;
    while (curr) {
      if (!comp(value, curr->value)) {
        if (curr->right) {
          curr = curr->right;
        } else {
          curr->right = new Node(value);
          break;
        }
      } else {
        if (curr->left) {
          curr = curr->left;
        } else {
          curr->left = new Node(value);
          break;
        }
      }
    }
  }

  int maxWidth() { return countWidth(root); }

 private:
  template <class Action>
  void _postOrder(Tree::Node *node, Action action) {
    std::stack<Node *> history;
    Node *lastNode = nullptr;
    while (!history.empty() || node) {
      if (node) {
        history.push(node);
        node = node->left;
      } else {
        Node *top = history.top();
        if (top->right && lastNode != top->right) {
          node = top->right;
        } else {
          action(top);
          lastNode = history.top();
          history.pop();
        }
      }
    }
  }

  Node *root;
  Comparator comp;
};

template <class T, class Comparator = DefaultComparator<T>>
class Treap : public ITree {
  struct Node {
    T value;
    size_t priority{};
    Node *left;
    Node *right;

    Node() : left(nullptr), right(nullptr), value(0) {}

    Node(T &value, size_t priority)
        : value(value), priority(priority), left(nullptr), right(nullptr) {}
  };

 public:
  Treap() : root(nullptr) {}
  ~Treap() {
    _postOrder(root, [](Node *node) { delete node; });
  }
  void add(T &value, size_t priority) { addRec(root, value, priority); }
  int maxWidth() { return countWidth(root); };

 private:
  void split(Treap::Node *node, T &value, Treap::Node *&left,
             Treap::Node *&right) {
    if (!node) {
      right = left = nullptr;
    } else if (!comp(value, node->value)) {
      split(node->right, value, node->right, right);
      left = node;
    } else {
      split(node->left, value, left, node->left);
      right = node;
    }
  }

  void addRec(Treap::Node *&node, T &value, size_t priority) {
    if (!node) {
      node = new Node(value, priority);
    } else if (node->priority < priority) {
      Node *newNode = new Node(value, priority);
      split(node, value, newNode->left, newNode->right);
      node = newNode;
    } else {
      if (!comp(value, node->value)) {
        addRec(node->right, value, priority);
      } else {
        addRec(node->left, value, priority);
      }
    }
  }

  template <class Action>
  void _postOrder(Treap::Node *&node, Action action) {
    if (!node) {
      return;
    }
    _postOrder(node->left, action);
    _postOrder(node->right, action);
    action(node);
  }

  Comparator comp;
  Node *root;
};

void test1() {
  Treap<int64_t> treap;
  Tree<int64_t> tree;
  int64_t values[10] = {5, 18, 25, 50, 30, 15, 20, 22, 40, 45};
  int64_t priors[10] = {11, 8, 7, 12, 30, 15, 10, 5, 20, 9};
  for (int i = 0; i < 10; i++) {
    treap.add(values[i], priors[i]);
    tree.add(values[i]);
  }
  assert((treap.maxWidth() - tree.maxWidth()) == 1);
}

void test2() {
  Treap<int64_t> treap;
  Tree<int64_t> tree;
  int64_t values[10] = {38, 37, 47, 35, 12, 0, 31, 21, 30, 41};
  int64_t priors[10] = {19, 5, 15, 0, 3, 42, 37, 45, 26, 6};
  for (int i = 0; i < 10; i++) {
    treap.add(values[i], priors[i]);
    tree.add(values[i]);
  }
  assert((treap.maxWidth() - tree.maxWidth()) == 1);
}

int main() {
  test1();
  test2();
  Treap<int64_t> treap;
  Tree<int64_t> tree;
  int amount;
  std::cin >> amount;

  for (int i = 0; i < amount; i++) {
    int64_t value;
    size_t priority;
    std::cin >> value >> priority;
    treap.add(value, priority);
    tree.add(value);
  }
  std::cout << treap.maxWidth() - tree.maxWidth() << std::endl;
  return 0;
}
