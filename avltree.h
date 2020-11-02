#ifndef AVLTREE_H
#define AVLTREE_H

#include <exception>
#include <iostream>
#include <string>
#include <utility>

template <typename T>
class AVLTree {
 public:
  // Exception Declaration (Tree)
  class Exception : public std::exception {
   private:
    std::string msg;

   public:
    explicit Exception(const char* message) : msg(message) {}
    explicit Exception(const std::string& message) : msg(message) {}

    virtual ~Exception() throw() {}

    virtual const char* what() const throw() { return msg.c_str(); }
  };
  // Node Declaration
  class Node {
   public:
    // Exception Declaration (Node)
    class Exception : public std::exception {
     private:
      std::string msg;

     public:
      explicit Exception(const char* message) : msg(message) {}
      explicit Exception(const std::string& message) : msg(message) {}

      virtual ~Exception() throw() {}

      virtual const char* what() const throw() { return msg.c_str(); }
    };

   private:
    T* dataPtr;
    Node* left;
    Node* right;

   public:
    Node();
    Node(const T& e);
    ~Node();

    T* getDataPtr();

    T& getData();
    void setData(const T& e);

    Node*& getRight();
    void setRight(Node*& p);
    Node*& getLeft();
    void setLeft(Node*& p);
  };

 private:
  Node* root;

  void insertData(Node*& r, const T& e);

  Node*& getLowest(Node*& r);
  Node*& getHighest(Node*& r);

  Node*& findData(Node*& r, const T& e);

  void parsePreOrder(Node*& r);
  void parseInOrder(Node*& r);
  void parsePostOrder(Node*& r, bool flag);

  int getBalanceFactor(Node*& r);

  void simpleLeftRotation(Node*& r);
  void simpleRightRotation(Node*& r);
  void doubleLeftRotation(Node*& r);
  void doubleRightRotation(Node*& r);

  void doBalancing(Node*& r);

  void copyAll(Node*& from, Node*& to);

  int getHeight(Node*& r);

  void deleteAll(Node*& r);

 public:
  AVLTree();
  AVLTree(AVLTree& t);
  ~AVLTree();

  bool isEmpty() const;
  bool isLeaf(Node*& r) const;

  void insertData(const T& e);
  void deleteData(Node*& r);

  Node*& findData(const T& e);

  T& retrieve(Node*& p);

  void parsePreOrder();
  void parseInOrder();
  void parsePostOrder();

  int getHeight();
  int getLeftHeight();
  int getRightHeight();

  AVLTree& operator=(AVLTree& t);

  void deleteAll();
};

// Node Implementation
template <typename T>
AVLTree<T>::Node::Node() : dataPtr(nullptr), left(nullptr), right(nullptr) {}

template <typename T>
AVLTree<T>::Node::Node(const T& e)
    : dataPtr(new T(e)), left(nullptr), right(nullptr) {
  if (dataPtr == nullptr) {
    throw Exception("Memoria no disponible, Node");
  }
}

template <typename T>
AVLTree<T>::Node::~Node() {
  delete dataPtr;
}

template <class T>
T* AVLTree<T>::Node::getDataPtr() {
  return dataPtr;
}

template <typename T>
T& AVLTree<T>::Node::getData() {
  return *dataPtr;
}

template <typename T>
void AVLTree<T>::Node::setData(const T& e) {
  if (dataPtr == nullptr) {
    if ((dataPtr = new T(e)) == nullptr) {
      throw Exception("Memoria no disponible, setData");
    }
  } else {
    *dataPtr = e;
  }
}

template <typename T>
typename AVLTree<T>::Node*& AVLTree<T>::Node::getLeft() {
  return left;
}

template <typename T>
void AVLTree<T>::Node::setLeft(Node*& p) {
  left = p;
}

template <typename T>
typename AVLTree<T>::Node*& AVLTree<T>::Node::getRight() {
  return right;
}

template <typename T>
void AVLTree<T>::Node::setRight(Node*& p) {
  right = p;
}

// Tree implementation

template <typename T>
void AVLTree<T>::insertData(AVLTree::Node*& r, const T& e) {
  if (r == nullptr) {
    try {
      if ((r = new Node(e)) == nullptr) {
        throw Exception("Memoria no disponible, insertData");
      }
    } catch (typename Node::Exception e) {
      throw Exception(e.what());
    }

    return;
  }

  if (e < r->getData()) {
    insertData(r->getLeft(), e);
  } else {
    insertData(r->getRight(), e);
  }

  // Se termina la recursividad
  doBalancing(r);
}

template <typename T>
typename AVLTree<T>::Node*& AVLTree<T>::getLowest(AVLTree::Node*& r) {
  if (r == nullptr || r->getLeft() == nullptr) {
    return r;
  }

  return getLowest(r->getLeft());
}

template <typename T>
typename AVLTree<T>::Node*& AVLTree<T>::getHighest(AVLTree::Node*& r) {
  if (r == nullptr || r->getRight() == nullptr) {
    return r;
  }

  return getHighest(r->getRight());
}

template <typename T>
typename AVLTree<T>::Node*& AVLTree<T>::findData(AVLTree::Node*& r,
                                                 const T& e) {
  if (r == nullptr || e == r->getData()) {
    return r;
  }

  if (e < r->getData()) {
    return findData(r->getLeft(), e);
  }

  return findData(r->getRight(), e);
}

template <typename T>
void AVLTree<T>::parsePreOrder(AVLTree::Node*& r) {
  if (r == nullptr) {
    return;
  }

  std::cout << r->getData() << ",";

  parsePreOrder(r->getLeft());
  parsePreOrder(r->getRight());
}

template <typename T>
void AVLTree<T>::parseInOrder(AVLTree::Node*& r) {
  if (r == nullptr) {
    return;
  }

  parseInOrder(r->getLeft());
  std::cout << r->getData() << ",";
  parseInOrder(r->getRight());
}

template <typename T>
void AVLTree<T>::parsePostOrder(AVLTree::Node*& r, bool flag) {
  if (r == nullptr) {
    return;
  }

  parsePostOrder(r->getLeft(), flag);
  parsePostOrder(r->getRight(), flag);
}

template <typename T>
int AVLTree<T>::getBalanceFactor(AVLTree::Node*& r) {
  return getHeight(r->getRight()) - getHeight(r->getLeft());
}

template <typename T>
void AVLTree<T>::simpleLeftRotation(AVLTree::Node*& r) {
  Node* aux1(r->getRight());
  Node* aux2(aux1->getLeft());

  r->setRight(aux2);
  aux1->setLeft(r);
  r = aux1;
}

template <typename T>
void AVLTree<T>::simpleRightRotation(AVLTree::Node*& r) {
  Node* aux1(r->getLeft());
  Node* aux2(aux1->getRight());

  r->setLeft(aux2);
  aux1->setRight(r);
  r = aux1;
}

template <typename T>
void AVLTree<T>::doubleLeftRotation(AVLTree::Node*& r) {
  simpleRightRotation(r->getRight());
  simpleLeftRotation(r);
}

template <typename T>
void AVLTree<T>::doubleRightRotation(AVLTree::Node*& r) {
  simpleLeftRotation(r->getLeft());
  simpleRightRotation(r);
}

template <typename T>
void AVLTree<T>::doBalancing(AVLTree::Node*& r) {
  switch (getBalanceFactor(r)) {
    case 2:
      if (getBalanceFactor(r->getRight()) == 1) {
        //            std::cout << "RSI: " << r->getData() << std::endl;
        simpleLeftRotation(r);
      } else {
        //            std::cout << "RDI: " << r->getData() << std::endl;
        doubleLeftRotation(r);
      }
      break;
    case -2:
      if (getBalanceFactor(r->getLeft()) == -1) {
        //            std::cout << "RSD: " << r->getData() << std::endl;
        simpleRightRotation(r);
      } else {
        //            std::cout << "RDD: " << r->getData() << std::endl;
        doubleRightRotation(r);
      }
      break;
    default:
      break;
  }
}

template <typename T>
void AVLTree<T>::copyAll(Node*& from, Node*& to) {
  if (from == nullptr) {
    return;
  }

  try {
    if ((to = new Node(from->getData()) == nullptr)) {
      throw Exception("Memoria no disponible, copyAll");
    }
  } catch (typename Node::Exception e) {
    throw Exception(e.what());
  }

  copyAll(from->getLeft(), to->getLeft());
  copyAll(from->getRight(), to->getRight());
}

template <typename T>
int AVLTree<T>::getHeight(AVLTree::Node*& r) {
  if (r == nullptr) {
    return 0;
  }

  int leftHeight(getHeight(r->getLeft()));
  int rightHeight(getHeight(r->getRight()));

  return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

template <typename T>
void AVLTree<T>::deleteAll(AVLTree::Node*& r) {
  if (r == nullptr) {
    return;
  }

  parsePostOrder(r->getLeft(), false);
  parsePostOrder(r->getRight(), false);

  delete r;

  r = nullptr;
}

template <typename T>
AVLTree<T>::AVLTree() : root(nullptr) {}

template <typename T>
AVLTree<T>::AVLTree(AVLTree& t) : root(nullptr) {
  copyAll(t.root, root);
}

template <typename T>
AVLTree<T>::~AVLTree() {
  deleteAll();
}

template <typename T>
bool AVLTree<T>::isEmpty() const {
  return root == nullptr;
}

template <typename T>
bool AVLTree<T>::isLeaf(AVLTree::Node*& r) const {
  return r != nullptr && r->getLeft() == r->getRight();
}

template <typename T>
void AVLTree<T>::insertData(const T& e) {
  insertData(root, e);
}

template <typename T>
void AVLTree<T>::deleteData(AVLTree::Node*& r) {
  if (r == nullptr) {
    throw Exception("Posicion invalida");
  }

  if (isLeaf(r)) {
    delete r;

    r = nullptr;
  }

  Node*& substitute(r->getLeft() != nullptr ? r->getHighest(r->getLeft())
                                            : getLowest(r->getRight()));

  std::swap(r->getDataPtr(), substitute->getDataPtr());

  deleteData(substitute);

  // Se termina la recursividad
  doBalancing(r);
}

template <typename T>
typename AVLTree<T>::Node*& AVLTree<T>::findData(const T& e) {
  return findData(root, e);
}

template <typename T>
T& AVLTree<T>::retrieve(AVLTree::Node*& r) {
  if (r == nullptr) {
    throw Exception("Posición invalida, retrieve");
  }

  return r->getData();
}

template <typename T>
void AVLTree<T>::parsePreOrder() {
  parsePreOrder(root);
}

template <typename T>
void AVLTree<T>::parseInOrder() {
  parseInOrder(root);
}

template <typename T>
void AVLTree<T>::parsePostOrder() {
  parsePostOrder(root, true);
}

template <typename T>
int AVLTree<T>::getHeight() {
  return getHeight(root);
}

template <typename T>
int AVLTree<T>::getLeftHeight() {
  return getHeight(root->getLeft());
}

template <typename T>
int AVLTree<T>::getRightHeight() {
  return getHeight(root->getRight());
}

template <typename T>
AVLTree<T>& AVLTree<T>::operator=(AVLTree& t) {
  deleteAll();

  copyAll(t.root, root);

  return *this;
}

template <typename T>
void AVLTree<T>::deleteAll() {
  deleteAll(root);
}

#endif // AVLTREE_H
