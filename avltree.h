#ifndef AVLTREE_H
#define AVLTREE_H

#include <exception>
#include <iostream>
#include <string>

template <class T>
class AVLTree {
 private:
  class Node {
   private:
    T* dataPtr;
    Node* right;
    Node* left;

   public:
    class Exception : public std::exception {
     private:
      std::string msg;

     public:
      explicit Exception(const char* message) : msg(message) {}

      explicit Exception(const std::string& message) : msg(message) {}

      virtual ~Exception() throw() {}

      virtual const char* what() const throw() { return msg.c_str(); }
    };

    Node();
    Node(const T& e);
    ~Node();

    T* getDataPtr();
    T& getData();
    Node*& getRight();
    Node*& getLeft();

    void setDataPtr(T*);
    void setData(const T&);
    void setRight(Node* p);
    void setLeft(Node* p);
  };

  Node* root;

  void copyAll(AVLTree<T>&);

  void insertData(Node*&, const T&);
  Node*& findData(Node*&, const T&);
  bool ifExistingElement(Node*&, const T&);
  void deleteData(Node*&);
  void copyAll(Node*&);
  int getHeight(Node*&);
  void deleteAll(Node*&);
  void parsePreOrder(Node*&);
  void parseInOrder(Node*&, std::ostream&);
  void parsePostOrder(Node*&);

  int getBalanceFactor(Node*&);
  void simpleLeftRotation(Node*&);
  void simpleRightRotation(Node*&);
  void doubleLeftRotation(Node*&);
  void doubleRightRotation(Node*&);
  void doBalancing(Node*&);

 public:
  class Exception : public std::exception {
   private:
    std::string msg;

   public:
    explicit Exception(const char* message) : msg(message) {}

    explicit Exception(const std::string& message) : msg(message) {}

    virtual ~Exception() throw() {}

    virtual const char* what() const throw() { return msg.c_str(); }
  };

  AVLTree();
  AVLTree(const AVLTree&);
  ~AVLTree();

  bool isEmpty() const;
  void insertData(const T&);
  bool isLeaf(Node*&);
  Node*& getLowest(Node*&);
  Node*& getHighest(Node*&);
  Node*& findData(const T&);
  bool ifExistingElement(const T&);
  void deleteData(const T&);
  T& retrieve(Node*&);
  int getHeight();
  void deleteAll();

  Node*& getRoot();

  int getHeightNode(Node*&);

  void parsePreOrder();
  void parseInOrder(std::ostream& out);
  void parsePostOrder();

  AVLTree& operator=(const AVLTree&);
};

template <class T>
AVLTree<T>::Node::Node() : dataPtr(nullptr), left(nullptr), right(nullptr) {}

template <class T>
AVLTree<T>::Node::Node(const T& e)
    : dataPtr(new T(e)), right(nullptr), left(nullptr) {
  if (dataPtr == nullptr) {
    throw Exception("Memoria no disponible, constructor del Nodo");
  }
}

template <class T>
AVLTree<T>::Node::~Node() {
  delete dataPtr;
}

template <class T>
T* AVLTree<T>::Node::getDataPtr() {
  return dataPtr;
}

template <class T>
T& AVLTree<T>::Node::getData() {
  if (dataPtr == nullptr) {
    throw Exception("Dato no disponible, Nodo-getData");
  }
  return *dataPtr;
}

template <class T>
typename AVLTree<T>::Node*& AVLTree<T>::Node::getRight() {
  return right;
}

template <class T>
typename AVLTree<T>::Node*& AVLTree<T>::Node::getLeft() {
  return left;
}

template <class T>
void AVLTree<T>::Node::setDataPtr(T* p) {
  dataPtr = p;
}

template <class T>
void AVLTree<T>::Node::setData(const T& e) {
  if (dataPtr == nullptr) {
    if ((dataPtr = new T(e)) == nullptr) {
      throw Exception("Memoria no disponible, Node::setData");
    }
  } else {
    *dataPtr = e;
  }
}

template <class T>
void AVLTree<T>::Node::setLeft(Node* p) {
  left = p;
}

template <class T>
void AVLTree<T>::Node::setRight(Node* p) {
  right = p;
}

template <class T>
bool AVLTree<T>::ifExistingElement(Node*& r, const T& e) {
  if (r->getData() == e) {
    return true;
  } else if (r == nullptr) {
    return false;
  } else if (e < r->getData()) {
    return findData(r->getLeft(), e);
  } else {
    return findData(r->getRight(), e);
  }
}

template <class T>
bool AVLTree<T>::ifExistingElement(const T& e) {
  return ifExistingElement(root, e);
}

template <class T>
AVLTree<T>::AVLTree() : root(nullptr) {}

template <class T>
AVLTree<T>::AVLTree(const AVLTree& bt) : root(nullptr) {
  deleteAll();
  copyAll(bt);
}

template <class T>
AVLTree<T>::~AVLTree() {
  deleteAll();
}

template <class T>
typename AVLTree<T>::Node*& AVLTree<T>::getRoot() {
  return root;
}

template <class T>
bool AVLTree<T>::isEmpty() const {
  return root = nullptr;
}

template <class T>
void AVLTree<T>::insertData(const T& e) {
  insertData(root, e);
}

template <class T>
void AVLTree<T>::insertData(Node*& r, const T& e) {
  if (r == nullptr) {
    try {
      if ((r = new Node(e)) == nullptr) {
        throw Exception("Memoria no disponible, insertData");
      }
    } catch (typename Node::Exception ex) {
      throw Exception(ex.what());
    }
  } else {
    if (e < r->getData()) {
      insertData(r->getLeft(), e);
    } else {
      insertData(r->getRight(), e);
    }
    doBalancing(r);
  }
}

template <class T>
bool AVLTree<T>::isLeaf(Node*& r) {
  if (r == nullptr) {
    return false;
  }
  return r != nullptr and r->getLeft() == r->getRight();
}

template <class T>
typename AVLTree<T>::Node*& AVLTree<T>::getLowest(Node*& r) {
  if (r == nullptr or r->getLeft() == nullptr) {
    return r;
  }
  return getLowest(r->getLeft());
}

template <class T>
typename AVLTree<T>::Node*& AVLTree<T>::getHighest(Node*& r) {
  if (r == nullptr or r->getRight() == nullptr) {
    return r;
  }
  return getHighest(r->getRight());
}

template <class T>
typename AVLTree<T>::Node*& AVLTree<T>::findData(const T& e) {
  return findData(root, e);
}

template <class T>
typename AVLTree<T>::Node*& AVLTree<T>::findData(Node*& r, const T& e) {
  if (r == nullptr or r->getData() == e) {
    return r;
  }
  if (e < r->getData()) {
    return findData(r->getLeft(), e);
  }
  return findData(r->getRight(), e);
}

template <class T>
void AVLTree<T>::deleteData(const T& e) {
  deleteData(findData(e));
}

template <class T>
void AVLTree<T>::deleteData(Node*& r) {
  if (r == nullptr) {
    throw Exception("Insuficiencia de datos");
  }
  if (isLeaf(r)) {
    delete r;
    r = nullptr;
    return;
  }
  Node*& posSust(r->getLeft() != nullptr ? posSust = getHighest(r->getLeft())
                                         : posSust = getLowest(r->getRight()));
  r->setData(posSust->getData());
  deleteData(posSust);
}

template <class T>
T& AVLTree<T>::retrieve(Node*& r) {
  return r->getData();
}

template <class T>
int AVLTree<T>::getHeight() {
  return getHeight(root);
}

template <class T>
int AVLTree<T>::getHeight(Node*& r) {
  if (r == nullptr) {
    return 0;
  }
  if (isLeaf(r)) {
    return 1;
  }
  int lH(getHeight(r->getLeft()));
  int rH(getHeight(r->getRight()));

  return 1 + (lH > rH ? lH : rH);
}

template <class T>
int AVLTree<T>::getHeightNode(Node*& r) {
  return getHeight(r);
}

template <class T>
void AVLTree<T>::deleteAll() {
  deleteAll(root);
}

template <class T>
void AVLTree<T>::deleteAll(Node*& r) {
  if (r == nullptr) {
    return;
  }
  deleteAll(r->getLeft());
  deleteAll(r->getRight());
  delete r;
  r = nullptr;
}

template <class T>
void AVLTree<T>::parsePreOrder() {
  return parsePreOrder(root);
}

template <class T>
void AVLTree<T>::parsePreOrder(Node*& r) {
  if (r == nullptr) {
    return;
  }

  std::cout << r->getData() << " | ";
  parsePreOrder(r->getLeft());
  parsePreOrder(r->getRight());
}

template <class T>
void AVLTree<T>::parseInOrder(std::ostream& out) {
  parseInOrder(root, out);
}

template <class T>
void AVLTree<T>::parseInOrder(Node*& r, std::ostream& out) {
  if (r == nullptr) {
    return;
  }
  parseInOrder(r->getLeft(), out);
  out.write(reinterpret_cast<char*>(&r->getData()), sizeof(T));
  parseInOrder(r->getRight(), out);
}

template <class T>
void AVLTree<T>::parsePostOrder() {
  parsePostOrder(root);
}

template <class T>
void AVLTree<T>::parsePostOrder(Node*& r) {
  if (r == nullptr) {
    return;
  }
  parsePostOrder(r->getLeft());
  parsePostOrder(r->getRight());
  std::cout << r->getData() << " | ";
}

template <class T>
AVLTree<T>& AVLTree<T>::operator=(const AVLTree& t) {
  deleteAll();
  copyAll(t);
  return *this;
}

template <class T>
int AVLTree<T>::getBalanceFactor(Node*& r) {
  return getHeight(r->getRight()) - getHeight(r->getLeft());
}

template <class T>
void AVLTree<T>::simpleLeftRotation(Node*& r) {
  Node* aux1(r->getRight());
  Node* aux2(aux1->getLeft());

  r->setRight(aux2);
  aux1->setLeft(r);
  r = aux1;
}

template <class T>
void AVLTree<T>::simpleRightRotation(Node*& r) {
  Node* aux1(r->getLeft());
  Node* aux2(aux1->getRight());

  r->setLeft(aux2);
  aux1->setRight(r);
  r = aux1;
}

template <class T>
void AVLTree<T>::doubleLeftRotation(Node*& r) {
  simpleRightRotation(r->getRight());
  simpleLeftRotation(r);
}

template <class T>
void AVLTree<T>::doubleRightRotation(Node*& r) {
  simpleLeftRotation(r->getLeft());
  simpleRightRotation(r);
}

template <class T>
void AVLTree<T>::doBalancing(Node*& r) {
  switch (getBalanceFactor(r)) {
    case -2:
      if (getBalanceFactor(r->getLeft()) == -1) {
        simpleRightRotation(r);
      } else {
        doubleRightRotation(r);
      }
      break;
    case 2:
      if (getBalanceFactor(r->getRight()) == 1) {
        simpleLeftRotation(r);
      } else {
        doubleLeftRotation(r);
      }
      break;
  }
}

#endif  // AVLTREE_H
