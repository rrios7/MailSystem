#ifndef HASHMAP_H
#define HASHMAP_H

#include <QDebug>
#include <fstream>
#include <string>
#include "list.h"

class HashMap {
  class HashNode {
   public:
    HashNode(std::string key) : m_Key{key}, m_Value{0} {}
    HashNode(std::string key, long long value) : m_Key{key}, m_Value{value} {}

    std::string getKey() const { return m_Key; }
    void setKey(const std::string& key) { m_Key = key; }

    long long getValue() const { return m_Value; }
    void setValue(const long long value) { m_Value = value; }

    bool operator==(const HashNode& other) { return m_Key == other.m_Key; }

   private:
    std::string m_Key;
    long long m_Value;
  };

 public:
  HashMap() : m_Data{new List<HashNode>[TABLE_SIZE]} {}

  void clear() {
    for (int i = 0; i < TABLE_SIZE; ++i)
      m_Data[i].clear();

    m_Count = 0;
  }

  bool empty() { return m_Count == 0; }

  int count() { return m_Count; }

  void insert(const std::string& key, const long long value) {
    long long pos = hash(key);
    HashNode entry{key, value};
    m_Data[pos].pushBack(entry);
    ++m_Count;
  }

  bool contains(const std::string& key) const {
    long long pos = hash(key);

    for (HashNode e : m_Data[pos])
      if (e.getKey() == key)
        return true;

    return false;
  }

  List<long long> retrieve(const std::string& key) const {
    List<long long> result;
    long long pos = hash(key);

    qDebug() << "Number of nodes in bucket: " << m_Data[pos].length();

    for (HashNode e : m_Data[pos])
      if (e.getKey() == key)
        result.pushBack(e.getValue());

    qDebug() << "Number of nodes that matched search criteria: "
             << result.length();

    return result;
  }

  void erase(const std::string& key) {
    if (!contains(key))
      return;

    long long pos = hash(key);
    auto list = m_Data[pos];
    list.erase(list.find(HashNode{key}));
    --m_Count;
  }

  void serialize() {
    std::ofstream file{"hashtable.txt"};
    for (int pos = 0; pos < TABLE_SIZE; ++pos)
      for (HashNode e : m_Data[pos])
        file << e.getKey() << "|" << e.getValue() << '\n';
  }

  bool deserialize() {
    clear();
    std::ifstream file{"hashtable.txt"};

    if (!file.is_open())
      return false;

    std::string key, value;

    while (!file.eof()) {
      getline(file, key, '|');
      if (key == "")
        break;
      getline(file, value);
      insert(key, std::stoll(value));
    }
    return true;
  }

 private:
  long long hash(const std::string& key) const {
    unsigned long long h = 0, g;

    for (char c : key) {
      h = (h << 4) + c;
      g = h & 0xF0000000;

      if (g > 1) {
        h ^= (g << 24);
        h ^= g;
      }
    }

    return h % TABLE_SIZE;
  }

 private:
  const static int TABLE_SIZE = 211;
  List<HashNode>* m_Data;
  int m_Count;
};
#endif // HASHMAP_H
