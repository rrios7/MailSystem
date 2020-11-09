#ifndef LIST_H
#define LIST_H

#include <iterator>

template <typename List>
class ListIterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = typename List::value_type;
  using node_pointer = typename List::node_pointer;
  using pointer = value_type*;
  using reference = const value_type&;
  using self = ListIterator;

 public:
  ListIterator(node_pointer ptr) : m_Ptr{ptr} {}

  self& operator++() {
    m_Ptr = m_Ptr->getNext();
    return *this;
  }

  self operator++(int) {
    self iterator{*this};
    m_Ptr = m_Ptr->getNext();
    return iterator;
  }

  self& operator--() {
    m_Ptr = m_Ptr->getPrev();
    return *this;
  }

  self operator--(int) {
    self iterator{*this};
    m_Ptr = m_Ptr->getPrev();
    return iterator;
  }

  reference operator*() { return *m_Ptr->getDataPtr(); }

  pointer operator->() { return m_Ptr->m_Ptr; }

  bool operator==(const self& other) { return m_Ptr == other.m_Ptr; }

  bool operator!=(const self& other) { return m_Ptr != other.m_Ptr; }

  difference_type operator-(const self& last) { return distance(*this, last); }

  difference_type distance(self first, self last) {
    difference_type n{0};
    while (first != last) {
      ++first;
      ++n;
    }
    return n;
  }

 private:
  node_pointer m_Ptr;
};

template <typename T>
class List {
 private:
  class ListNode {
   public:
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using node_pointer = ListNode*;

   public:
    ListNode() : m_Ptr{nullptr}, m_Next{nullptr}, m_Prev{nullptr} {}

    ListNode(const_reference elem)
        : m_Ptr{new value_type{elem}}, m_Next{nullptr}, m_Prev{nullptr} {}

    value_type getData() { return *m_Ptr; }
    void setData(const_reference elem) { *m_Ptr = elem; }

    pointer getDataPtr() { return m_Ptr; }
    void setDataPtr(pointer ptr) { m_Ptr = ptr; }

    node_pointer getNext() { return m_Next; }
    void setNext(const node_pointer& ptr) { m_Next = ptr; }

    node_pointer getPrev() { return m_Prev; }
    void setPrev(const node_pointer& ptr) { m_Prev = ptr; }

   private:
    pointer m_Ptr;
    node_pointer m_Next;
    node_pointer m_Prev;
  };

 public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using node = ListNode;
  using node_pointer = node*;
  using iterator = ListIterator<List<value_type>>;

 public:
  List() : m_Head{new node}, m_Length{0} {
    m_Head->setNext(m_Head);
    m_Head->setPrev(m_Head);
  }

  List(const List& other) : List{} { copy(other); }

  List(std::initializer_list<value_type> items) : List() {
    for (value_type item : items) {
      pushBack(item);
    }
  }

  ~List() { clear(); }

  iterator begin() { return iterator{m_Head->getNext()}; }
  iterator end() { return iterator{m_Head}; }

  reference front() { return *m_Head->getNext()->getDataPtr(); }
  reference back() { return *m_Head->getPrev()->getDataPtr(); }

  size_t length() const { return m_Length; }

  bool empty() const { return m_Head->getNext() == m_Head; }

  iterator find(const_reference elem) {
    if (empty())
      return end();

    node_pointer aux(m_Head->getNext());

    while (aux != m_Head) {
      if (aux->getData() == elem) {
        return aux;
      }

      aux = aux->getNext();
    }

    return end();
  }

  void pushBack(const_reference elem) { insert(m_Head->getPrev(), elem); }
  void popBack() { erase(m_Head->getPrev()); }

  void pushFront(const_reference elem) { insert(m_Head, elem); }
  void popFront() { erase(m_Head->getNext()); }

  void clear() {
    node_pointer aux;

    while (m_Head->getNext() != m_Head) {
      aux = m_Head->getNext();
      m_Head->setNext(aux->getNext());
      delete aux;
    }

    m_Head->setPrev(m_Head);
    m_Length = 0;
  }

  List& operator=(const List& other) {
    clear();
    copy(other);
    return *this;
  }

 private:
  void insert(node_pointer ptr, const_reference elem) {
    node_pointer newNode{new node{elem}};
    newNode->setPrev(ptr);
    newNode->setNext(ptr->getNext());
    ptr->getNext()->setPrev(newNode);
    ptr->setNext(newNode);
    ++m_Length;
  }

  void erase(node_pointer ptr) {
    ptr->getPrev()->setNext(ptr->getNext());
    ptr->getNext()->setPrev(ptr->getPrev());
    delete ptr;
    --m_Length;
  }

  void copy(const List& other) {
    node_pointer aux{other.m_Head->getNext()};

    while (aux != other.m_Head) {
      node_pointer newNode{new node{aux->getData()}};
      newNode->setPrev(m_Head->getPrev());
      newNode->setNext(m_Head);
      m_Head->getPrev()->setNext(newNode);
      m_Head->setPrev(newNode);
      ++m_Length;
      aux = aux->getNext();
    }
  }

 private:
  node_pointer m_Head;
  size_t m_Length;
};

#endif  // LIST_H
