#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>

template <class T>
class Vector {
 public:
  Vector() : m_Counter(0), m_Size(m_initialSize) {
    data = new T[m_initialSize];
  }

  ~Vector() { delete[] data; }

  bool empty() { return m_Counter == 0; }

  size_t length() { return m_Counter; }

  void pushBack(const T& element) {
    data[m_Counter++] = element;

    if (m_Counter == m_Size)
      expand();
  }

  void popBack() {
    if (empty())
      return;

    m_Counter--;
  }

  T& operator[](const size_t& pos) { return data[pos]; }

  void sort() {
    T* aux = new T[m_Size];
    mergeSort(aux, 0, m_Counter - 1);

    delete[] aux;
  }

  int binarySearch(const T& element) {
    int left = 0;
    int right = m_Counter - 1;
    int mid;

    while (left <= right) {
      mid = (left + right) / 2;
      if (data[mid] == element)
        return mid;

      if (data[mid] < element)
        left = mid + 1;
      else
        right = mid - 1;
    }

    return -1;
  }

 private:
  void expand() {
    T* temp = new T[m_Size * 2];

    for (size_t i = 0; i < m_Counter; i++) {
      temp[i] = data[i];
    }

    delete[] data;
    data = temp;
    m_Size *= 2;
  }

  void mergeSort(T* aux, const int& left, const int& right) {
    if (left == right)
      return;

    int mid = (left + right) / 2;

    mergeSort(aux, left, mid);
    mergeSort(aux, mid + 1, right);

    // Join the leaves of recursion tree
    merge(aux, left, mid, right);
  }

  void merge(T* aux, const int& left, const int& mid, const int& right) {
    for (int i = left; i <= right; i++) {
      aux[i] = data[i];
    }

    int leftP = left;
    int rightP = mid + 1;

    for (int index = leftP; index <= right; index++) {
      if (leftP > mid)
        data[index] = aux[rightP++];
      else if (rightP > right)
        data[index] = aux[leftP++];
      else if (aux[leftP] < aux[rightP])
        data[index] = aux[leftP++];
      else
        data[index] = aux[rightP++];
    }
  }

 private:
  const static size_t m_initialSize = 1024;
  T* data;
  std::size_t m_Counter;
  std::size_t m_Size;
};

#endif  // VECTOR_H
