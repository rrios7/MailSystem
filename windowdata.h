#ifndef WINDOWDATA_H
#define WINDOWDATA_H

#include <QDebug>
#include <fstream>
#include "avltree.h"
#include "email.h"
#include "emaildao.h"
#include "primaryindex.h"
#include "tools.h"
#include "vector.h"

class WindowData {
 public:
  static WindowData& getInstance() {
    static WindowData instance;
    return instance;
  }

 private:
  WindowData() : memorySearch{false}, primaryIndexTree{NoTree} {
    std::fstream primaryIndex{primaryIndexFileName,
                              std::ios::in | std::ios::binary};
    // If Index is not yet made
    if (!primaryIndex.is_open()) {
      primaryIndex.open(primaryIndexFileName, std::ios::out | std::ios::binary);
      // Index Update Flag
      bool needsUpdating = true;
      primaryIndex.write(reinterpret_cast<char*>(&needsUpdating), sizeof(bool));
    }
    primaryIndex.close();
    loadPrimaryIndex();
  }

  ~WindowData() { savePrimaryIndex(); }

  void savePrimaryIndex() {
    std::fstream primaryIndex{primaryIndexFileName,
                              std::ios::in | std::ios::binary};
    bool needsUpdating;
    primaryIndex.read(reinterpret_cast<char*>(&needsUpdating), sizeof(bool));
    primaryIndex.close();

    if (needsUpdating) {
      primaryIndex.open(primaryIndexFileName, std::ios::out | std::ios::binary);
      needsUpdating = false;
      primaryIndex.write(reinterpret_cast<char*>(&needsUpdating), sizeof(bool));
      avlPrimaryIndex.parseInOrder(primaryIndex);
      primaryIndex.close();
    }
  }

  void loadPrimaryIndex() {
    std::fstream primaryIndex{primaryIndexFileName,
                              std::ios::in | std::ios::binary};
    bool needsUpdating;
    primaryIndex.read(reinterpret_cast<char*>(&needsUpdating), sizeof(bool));

    qDebug() << (needsUpdating ? "Primary index needs to be updated"
                               : "Primary index is up to date");

    if (needsUpdating) {
      createPrimaryIndex();
      return;
    }

    while (!primaryIndex.eof()) {
      PrimaryIndex index;
      primaryIndex.read(reinterpret_cast<char*>(&index), sizeof(index));
      if (primaryIndex.fail())
        break;
      avlPrimaryIndex.insertData(index);
    }

    primaryIndex.close();
  }

  void createPrimaryIndex() {
    EmailDAO& db = EmailDAO::getInstance();
    long long id = 0;
    Email email = db.read(id);

    while (db.next()) {
      if (email.getId() == id && Tools::isValidEmail(email.getSender()))
        avlPrimaryIndex.insertData(PrimaryIndex{
            email.getId(),
            email.getId() * static_cast<long long>(sizeof(email))});
      email = db.read(++id);
    }
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
  enum Tree { NoTree = 0, AVL, RedBlack };

 public:
  Email& getEmail() { return email; }
  void setEmail(Email& email) { this->email = email; }

  Operation getOperation() const { return operation; }
  void setOperation(Operation operation) { this->operation = operation; }

  Page getPreviousPage() const { return previousPage; }
  void setPreviousPage(const Page& page) { previousPage = page; }

  bool isMemorySearchEnabled() const { return memorySearch; }
  void setMemorySearch(bool value) { memorySearch = value; }

  Tree getPrimaryIndexTree() const { return primaryIndexTree; }
  void setPrimaryIndexTree(const Tree& value) { primaryIndexTree = value; }

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

  void addToIndex() {
    setUpdateFlag();
    avlPrimaryIndex.insertData(PrimaryIndex{
        email.getId(), email.getId() * static_cast<long long>(sizeof(email))});
  }

  void removeFromIndex() {
    setUpdateFlag();
    avlPrimaryIndex.deleteData(PrimaryIndex{email.getId()});
  }

 public:
  // Primary & Secondary Index Data Structres
  Vector<Email> vectorInbox;
  AVLTree<PrimaryIndex> avlPrimaryIndex;

 private:
  // Window Data
  Email email;
  Operation operation;
  Page previousPage;
  bool memorySearch;
  Tree primaryIndexTree;

  // Primary & Secondary Index File Names
  const std::string primaryIndexFileName = "primaryindex.dat";
};

#endif  // WINDOWDATA_H
