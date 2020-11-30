#ifndef WINDOWDATA_H
#define WINDOWDATA_H

#include <QDebug>
#include <fstream>
#include "avltree.h"
#include "email.h"
#include "emaildao.h"
#include "hashmap.h"
#include "paginatedavltree.h"
#include "primaryindexentry.h"
#include "secondaryindexentry.h"
#include "tools.h"
#include "vector.h"

class WindowData {
 public:
  static WindowData& getInstance() {
    static WindowData instance;
    return instance;
  }

 private:
  WindowData() : memorySearch{false}, indexType{NoTree} {
    std::fstream file{primaryIndexFileName, std::ios::in | std::ios::binary};
    // If Index is not yet made
    if (!file.is_open()) {
      file.open(primaryIndexFileName, std::ios::out | std::ios::binary);
      // Index Update Flag
      bool needsUpdating = true;
      file.write(reinterpret_cast<char*>(&needsUpdating), sizeof(bool));
    }
    file.close();
    loadPrimaryIndex();
    loadSecondaryIndex();
  }

  ~WindowData() { savePrimaryIndex(); }

  void savePrimaryIndex() {
    std::fstream file{primaryIndexFileName, std::ios::in | std::ios::binary};
    bool needsUpdating;
    file.read(reinterpret_cast<char*>(&needsUpdating), sizeof(bool));
    file.close();

    if (needsUpdating) {
      file.open(primaryIndexFileName, std::ios::out | std::ios::binary);
      needsUpdating = false;
      file.write(reinterpret_cast<char*>(&needsUpdating), sizeof(bool));
      primaryIndex.inOrder(file);
      file.close();
    }
  }

  void loadPrimaryIndex() {
    std::fstream file{primaryIndexFileName, std::ios::in | std::ios::binary};
    bool needsUpdating;
    file.read(reinterpret_cast<char*>(&needsUpdating), sizeof(bool));

    qDebug() << (needsUpdating ? "Primary index needs to be updated"
                               : "Primary index is up to date");

    if (needsUpdating) {
      createPrimaryIndex();
      return;
    }

    while (!file.eof()) {
      PrimaryIndexEntry entry;
      file.read(reinterpret_cast<char*>(&entry), sizeof(entry));
      if (file.fail())
        break;
      primaryIndex.insert(entry);

      if (paginatedPrimaryIndex.count() < paginatedPrimaryIndex.MAX_PAGES) {
        paginatedPrimaryIndex.insert(entry);
      }
    }

    file.close();
  }

  void createPrimaryIndex() {
    EmailDAO& db = EmailDAO::getInstance();
    long long id = 0;
    Email email = db.read(id);

    while (db.next()) {
      if (email.getId() == id && Tools::isValidEmail(email.getSender()))
        primaryIndex.insert(PrimaryIndexEntry{
            email.getId(),
            email.getId() * static_cast<long long>(sizeof(email))});
      email = db.read(++id);
    }
    savePrimaryIndex();
  }

  void loadSecondaryIndex() {
    EmailDAO& db = EmailDAO::getInstance();

    std::fstream file{primaryIndexFileName, std::ios::in | std::ios::binary};
    bool needsUpdating;
    file.read(reinterpret_cast<char*>(&needsUpdating), sizeof(bool));

    while (!file.eof()) {
      PrimaryIndexEntry entry;
      file.read(reinterpret_cast<char*>(&entry), sizeof(entry));
      if (file.fail())
        break;

      Email email = db.read(entry.getId());

      addElementToInvertedList(senderSecondaryIndex, email.getSender(),
                               email.getId());
      addElementToInvertedList(receiverSecondaryIndex, email.getReceiver(),
                               email.getId());
    }

    file.close();
  }

  void addElementToInvertedList(AVLTree<SecondaryIndexEntry>& tree,
                                std::string email,
                                long long id) {
    auto invertedList = tree.retrieve(SecondaryIndexEntry{email});

    if (invertedList != nullptr) {
      invertedList->getValue().getReferenceList().pushOrdered(id);
    } else
      tree.insert(SecondaryIndexEntry{email, id});
  }

  void setUpdateFlag() {
    std::fstream primaryIndex{primaryIndexFileName,
                              std::ios::out | std::ios::binary};
    bool needsUpdating = true;
    primaryIndex.write(reinterpret_cast<char*>(&needsUpdating), sizeof(bool));
    primaryIndex.close();
  }

 public:
  WindowData(WindowData const&) = delete;
  void operator=(WindowData const&) = delete;

 public:
  enum Page { InboxPage = 0, EmailPage, BackupPage, SettingsPage };
  enum Operation { None = 0, Read, Write, Update, Delete };
  enum Index {
    NoTree = 1 << 0,
    AVL = 1 << 1,
    Paginated = 1 << 2,
    HashTable = 1 << 3
  };

 public:
  Email& getEmail() { return email; }
  void setEmail(Email& email) { this->email = email; }

  Operation getOperation() const { return operation; }
  void setOperation(Operation operation) { this->operation = operation; }

  Page getPreviousPage() const { return previousPage; }
  void setPreviousPage(const Page& page) { previousPage = page; }

  bool isMemorySearchEnabled() const { return memorySearch; }
  void setMemorySearch(bool value) { memorySearch = value; }

  Index getIndexType() const { return indexType; }
  void setIndexType(const Index& value) { indexType = value; }

  void loadInboxToMemory() {
    EmailDAO& db = EmailDAO::getInstance();
    long long id = 0;
    Email email = db.read(id);

    while (db.next()) {
      if (email.getId() == id && Tools::isValidEmail(email.getSender()))
        vectorInbox.pushBack(email);
      email = db.read(++id);
    }
    vectorInbox.sort();
  }

  void loadHashTable() {
    EmailDAO& db = EmailDAO::getInstance();
    long long id = 0;
    Email email = db.read(id);

    while (db.next()) {
      if (email.getId() == id && Tools::isValidEmail(email.getSender()))
        senderHashTable.insert(email.getSender(), id);
      email = db.read(++id);
    }
  }

  void addToIndex() {
    setUpdateFlag();
    primaryIndex.insert(PrimaryIndexEntry{
        email.getId(), email.getId() * static_cast<long long>(sizeof(email))});
  }

  void removeFromIndex() {
    setUpdateFlag();
    primaryIndex.erase(PrimaryIndexEntry{email.getId()});
    paginatedPrimaryIndex.erase(PrimaryIndexEntry{email.getId()});
  }

 public:
  // Primary & Secondary Index Data Structres
  Vector<Email> vectorInbox;
  AVLTree<PrimaryIndexEntry> primaryIndex;
  AVLTree<SecondaryIndexEntry> senderSecondaryIndex;
  AVLTree<SecondaryIndexEntry> receiverSecondaryIndex;
  PaginatedAVLTree<PrimaryIndexEntry> paginatedPrimaryIndex;
  HashMap senderHashTable;

 private:
  // Window Data
  Email email;
  Operation operation;
  Page previousPage;
  bool memorySearch;
  Index indexType;

  // Primary & Secondary Index File Names
  const std::string primaryIndexFileName = "primaryindex.dat";
};

#endif  // WINDOWDATA_H
