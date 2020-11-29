#ifndef PAGINATEDAVLTREE_H
#define PAGINATEDAVLTREE_H

#include <QDebug>
#include <chrono>
#include <iostream>

template <typename T>
class PaginatedAVLTree {
 public:
  const static int MAX_PAGES = 10;
  class PaginatedAVLNode {
   public:
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using node_pointer = PaginatedAVLNode*;

   public:
    PaginatedAVLNode(T value)
        : m_BalanceFactor{0},
          m_Height{0},
          m_Value{value},
          m_Timestamp{std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count()},
          m_Left{nullptr},
          m_Right{nullptr} {}

    int getBalanceFactor() const { return m_BalanceFactor; }
    void setBalanceFactor(const int balanceFactor) {
      m_BalanceFactor = balanceFactor;
    }

    int getHeight() const { return m_Height; }
    void setHeight(const int height) { m_Height = height; }

    value_type getValue() const { return m_Value; }
    reference getValue() { return m_Value; }
    void setValue(const_reference value) { m_Value = value; }

    long long getTimestamp() { return m_Timestamp; }
    void setTimestamp(long long timestamp) { m_Timestamp = timestamp; }
    void updateTimestamp() {
      m_Timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count();
    }

    node_pointer getLeft() { return m_Left; }
    void setLeft(node_pointer ptr) { m_Left = ptr; }

    node_pointer getRight() { return m_Right; }
    void setRight(node_pointer ptr) { m_Right = ptr; }

   private:
    int m_BalanceFactor;
    int m_Height;
    value_type m_Value;
    long long m_Timestamp;
    node_pointer m_Left;
    node_pointer m_Right;
  };

 public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using node = PaginatedAVLNode;
  using node_pointer = node*;

 public:
  PaginatedAVLTree() : m_Root{nullptr}, m_Count{0} {}

  bool empty() { return m_Root == nullptr; }

  int count() { return m_Count; }

  int height() { return (m_Root == nullptr ? 0 : m_Root->getHeight()); }

  bool contains(const_reference value) {
    return retrieve(m_Root, value) != nullptr;
  }

  node_pointer retrieve(const_reference value) {
    return retrieve(m_Root, value);
  }

  void insert(const_reference value) {
    if (m_Count >= MAX_PAGES) {
      qDebug() << "Page limit reached on Paginated AVL Tree, Deleting LRU Node";
      deleteLRU();
    }
    m_Root = insert(m_Root, value);
    ++m_Count;
    qDebug() << "Node ID: " << value.getId()
             << " inserted in Paginated AVL Tree";
  }

  void erase(const_reference value) {
    if (contains(value)) {
      m_Root = erase(m_Root, value);
      --m_Count;
    }
  }
  void inOrder(node_pointer lru) { inOrderUtil(m_Root, lru); }

 private:
  node_pointer retrieve(node_pointer root, const_reference value) {
    if (root == nullptr)
      return nullptr;

    if (root->getValue() == value) {
      root->updateTimestamp();
      qDebug() << "ID: " << root->getValue().getId()
               << " Updated timestamp: " << root->getTimestamp();
      return root;
    }

    if (value < root->getValue())
      return retrieve(root->getLeft(), value);

    return retrieve(root->getRight(), value);
  }

  node_pointer insert(node_pointer root, const_reference value) {
    if (root == nullptr) {
      root = new node(value);
      return root;
    }

    if (value < root->getValue())
      root->setLeft(insert(root->getLeft(), value));
    else
      root->setRight(insert(root->getRight(), value));

    update(root);

    return balance(root);
  }

  void update(node_pointer root) {
    int leftHeight = (root->getLeft() ? root->getLeft()->getHeight() : -1);
    int rightHeight = (root->getRight() ? root->getRight()->getHeight() : -1);
    root->setHeight(std::max(leftHeight, rightHeight) + 1);
    root->setBalanceFactor(rightHeight - leftHeight);
  }

  node_pointer balance(node_pointer root) {
    if (root->getBalanceFactor() == -2) {
      if (root->getLeft()->getBalanceFactor() <= 0)
        return leftLeftCase(root);
      else
        return leftRightCase(root);
    } else if (root->getBalanceFactor() == 2) {
      if (root->getRight()->getBalanceFactor() >= 0)
        return rightRightCase(root);
      else
        return rightLeftCase(root);
    }

    return root;
  }

  node_pointer leftLeftCase(node_pointer root) { return rightRotation(root); }

  node_pointer leftRightCase(node_pointer root) {
    root->setLeft(leftRotation(root->getLeft()));
    return leftLeftCase(root);
  }

  node_pointer rightRightCase(node_pointer root) { return leftRotation(root); }

  node_pointer rightLeftCase(node_pointer root) {
    root->setRight(rightRotation(root->getRight()));
    return rightRightCase(root);
  }

  node_pointer leftRotation(node_pointer root) {
    node_pointer newParent{root->getRight()};
    root->setRight(newParent->getLeft());
    newParent->setLeft(root);
    update(root);
    update(newParent);
    return newParent;
  }

  node_pointer rightRotation(node_pointer root) {
    node_pointer newParent{root->getLeft()};
    root->setLeft(newParent->getRight());
    newParent->setRight(root);
    update(root);
    update(newParent);
    return newParent;
  }

  node_pointer erase(node_pointer root, const_reference value) {
    if (root == nullptr)
      return nullptr;

    if (value == root->getValue()) {
      if (root->getLeft() == nullptr) {
        node_pointer rightChild{root->getRight()};
        delete root;
        return rightChild;
      } else if (root->getRight() == nullptr) {
        node_pointer leftChild{root->getLeft()};
        delete root;
        return leftChild;
      } else {
        if (root->getLeft()->getHeight() > root->getRight()->getHeight()) {
          value_type successorValue = findMax(root->getLeft());
          root->setValue(successorValue);
          root->setLeft(erase(root->getLeft(), successorValue));
        } else {
          value_type successorValue = findMin(root->getRight());
          root->setValue(successorValue);
          root->setRight(erase(root->getRight(), successorValue));
        }
      }
    } else if (value < root->getValue())
      root->setLeft(erase(root->getLeft(), value));
    else
      root->setRight(erase(root->getRight(), value));

    update(root);

    return balance(root);
  }

  value_type findMin(node_pointer root) {
    while (root->getLeft() != nullptr)
      root = root->getLeft();

    return root->getValue();
  }

  value_type findMax(node_pointer root) {
    while (root->getRight() != nullptr)
      root = root->getRight();

    return root->getValue();
  }

  void inOrderUtil(node_pointer root, node_pointer lru) {
    if (root == nullptr)
      return;

    inOrderUtil(root->getLeft(), lru);
    inOrderUtil(root->getRight(), lru);

    if (root->getTimestamp() < lru->getTimestamp()) {
      lru->setValue(root->getValue());
      lru->setTimestamp(root->getTimestamp());
    }
  }

  void deleteLRU() {
    node_pointer lru = new node{m_Root->getValue()};
    inOrder(lru);
    qDebug() << "Least Recently Used Node: " << lru->getValue().getId();
    erase(lru->getValue());
    delete lru;
  }

 private:
  node_pointer m_Root;
  int m_Count;
};

#endif  // PAGINATEDAVLTREE_H
