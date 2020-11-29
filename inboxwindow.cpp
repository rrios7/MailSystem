#include "inboxwindow.h"
#include "ui_inboxwindow.h"

#include <QDebug>
#include "advancedsearch.h"
#include "emaildao.h"
#include "emailwindow.h"
#include "messagebox.h"
#include "tools.h"

InboxWindow::InboxWindow(QWidget* parent)
    : QWidget{parent}, ui{new Ui::InboxWindow} {
  ui->setupUi(this);
  ui->mailTableWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
}

InboxWindow::~InboxWindow()
{
  delete ui;
}

// Class Methods
void InboxWindow::writeEmail() {
  EmailDAO::getInstance().write(WindowData::getInstance().getEmail());
  WindowData::getInstance().addToIndex();
}

void InboxWindow::openEmail(long long id,
                            const WindowData::Operation operation) {
  WindowData& data = WindowData::getInstance();
  data.setOperation(operation);
  data.getEmail() = EmailDAO::getInstance().read(id);
  emit showPage(WindowData::EmailPage);
}

void InboxWindow::searchById(const long long id) {
  searchResults.clear();
  WindowData& data = WindowData::getInstance();
  if (data.getIndexTree() & WindowData::Paginated)
    searchByIdInPaginatedTree(id);
  else if (data.getIndexTree() & WindowData::AVL)
    searchByIdInTree(id);
  else
    searchByIdInFile(id);

  if (searchResults.empty()) {
    MessageBox::display("Couldn't find an email with that ID");
    clearEmailTable();
    return;
  }

  populateEmailTable();
}

void InboxWindow::searchByIdInFile(const long long id) {
  Email email = EmailDAO::getInstance().read(id);
  if (Tools::isValidEmail(email.getSender()))
    searchResults.pushBack(email);
}

void InboxWindow::searchByIdInTree(const long long id) {
  WindowData& data = WindowData::getInstance();

  qDebug() << "Primary Index Tree Height: " << data.primaryIndex.height();

  PrimaryIndexEntry query{id};
  auto result = data.primaryIndex.retrieve(query);
  if (result != nullptr)
    searchResults.pushBack(
        EmailDAO::getInstance().read(result->getValue().getId()));
}

void InboxWindow::searchByIdInPaginatedTree(const long long id) {
  WindowData& data = WindowData::getInstance();

  qDebug() << "Paginated Primary Index Tree Height: "
           << data.paginatedPrimaryIndex.height();

  PrimaryIndexEntry query{id};
  auto result = data.paginatedPrimaryIndex.retrieve(query);
  if (result != nullptr)
    searchResults.pushBack(
        EmailDAO::getInstance().read(result->getValue().getId()));
  else {
    auto primaryIndexResult = data.primaryIndex.retrieve(query);
    if (primaryIndexResult != nullptr) {
      data.paginatedPrimaryIndex.insert(primaryIndexResult->getValue());
      searchResults.pushBack(
          EmailDAO::getInstance().read(primaryIndexResult->getValue().getId()));
    }
  }
}

void InboxWindow::searchBySender(const char* sender) {
  searchResults.clear();
  WindowData& data = WindowData::getInstance();

  if (data.isMemorySearchEnabled())
    searchBySenderInMemory(sender);
  else if (data.getIndexTree() & WindowData::AVL) {
    auto result = searchByEmailInTree(sender, data.senderSecondaryIndex);
    for (auto id : result)
      searchByIdInTree(id);
  } else
    searchBySenderInFile(sender);

  if (searchResults.empty()) {
    MessageBox::display("Couldn't find an email with that sender");
    clearEmailTable();
    return;
  }

  populateEmailTable();
}

void InboxWindow::searchBySenderInFile(const char* sender) {
  EmailDAO& db = EmailDAO::getInstance();

  long long id = 0;
  Email email = db.read(id);

  while (db.next()) {
    if (email.getId() == id && Tools::equalEmails(email.getSender(), sender)) {
      searchResults.pushBack(email);
    }
    email = db.read(++id);
  }
}

void InboxWindow::searchBySenderInMemory(const char* sender) {
  WindowData& data = WindowData::getInstance();

  if (data.vectorInbox.empty())
    data.loadInboxToMemory();

  Email query;
  query.setSender(sender);

  int pos = data.vectorInbox.binarySearch(query);

  if (pos == -1)
    return;

  while (pos - 1 >= 0 &&
         Tools::equalEmails(data.vectorInbox[pos - 1].getSender(), sender))
    --pos;

  while (pos < static_cast<int>(data.vectorInbox.length()) &&
         Tools::equalEmails(data.vectorInbox[pos].getSender(), sender))
    searchResults.pushBack(data.vectorInbox[pos++]);
}

List<long long> InboxWindow::searchByEmailInTree(
    const char* email,
    AVLTree<SecondaryIndexEntry>& tree) {
  qDebug() << "Email Secondary Index Tree Height:  " << tree.height();

  List<long long> queryResults;

  SecondaryIndexEntry query{email};
  auto result = tree.retrieve(query);

  if (result != nullptr)
    for (long long id : result->getValue().getReferenceList())
      queryResults.pushBack(id);

  return queryResults;
}

void InboxWindow::clearEmailTable() {
  ui->mailTableWidget->clearContents();
  ui->mailTableWidget->setRowCount(0);
}

void InboxWindow::populateEmailTable() {
  ui->mailTableWidget->clearContents();
  ui->mailTableWidget->setRowCount(searchResults.length());

  int row = 0;
  for (Email email : searchResults) {
    QWidget* widget = new QWidget();
    QTableWidgetItem* id = new QTableWidgetItem(QString::number(email.getId()));
    QTableWidgetItem* sender = new QTableWidgetItem(email.getSender());
    QTableWidgetItem* receiver = new QTableWidgetItem(email.getReceiver());
    QTableWidgetItem* subject = new QTableWidgetItem(email.getSubject());

    QPushButton* readButton = new QPushButton();
    readButton->setIcon(QIcon("://icons/eye.png"));
    readButton->setMinimumSize(25, 25);
    readButton->setIconSize(QSize(20, 20));
    readButton->setStyleSheet("border-radius: 12px;");
    readButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    connect(readButton, &QPushButton::clicked, this,
            &InboxWindow::onReadButtonClicked);

    QPushButton* editButton = new QPushButton();
    editButton->setIcon(QIcon("://icons/pencil.png"));
    editButton->setMinimumSize(25, 25);
    editButton->setIconSize(QSize(20, 20));
    editButton->setStyleSheet(
        "QPushButton {border-radius: 12px; background-color: rgb(82, 224, "
        "102);}"
        "QPushButton:hover { background-color: rgb(39, 216, 63);}");
    editButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    connect(editButton, &QPushButton::clicked, this,
            &InboxWindow::onEditButtonClicked);

    QPushButton* deleteButton = new QPushButton();
    deleteButton->setIcon(QIcon("://icons/delete.png"));
    deleteButton->setMinimumSize(25, 25);
    deleteButton->setIconSize(QSize(20, 20));
    deleteButton->setStyleSheet(
        "QPushButton {border-radius: 12px; background-color: rgb(242, 22, "
        "43);}"
        "QPushButton:hover { background-color: rgb(203, 11, 29);}");
    deleteButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    connect(deleteButton, &QPushButton::clicked, this,
            &InboxWindow::onDeleteButtonClicked);

    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(readButton);
    layout->addWidget(editButton);
    layout->addWidget(deleteButton);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(2, 0, 2, 0);
    widget->setLayout(layout);
    ui->mailTableWidget->setItem(row, 0, id);
    ui->mailTableWidget->setItem(row, 1, sender);
    ui->mailTableWidget->setItem(row, 2, receiver);
    ui->mailTableWidget->setItem(row, 3, subject);
    ui->mailTableWidget->setCellWidget(row, 4, widget);
    ++row;
  }

  ui->mailTableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
}

// Pseudo Slots
void InboxWindow::onReadButtonClicked() {
  long long id = getClickedId();
  if (id == -1)
    return;

  openEmail(id, WindowData::Read);
}

void InboxWindow::onEditButtonClicked() {
  long long id = getClickedId();
  if (id == -1)
    return;

  openEmail(id, WindowData::Update);
}

void InboxWindow::onDeleteButtonClicked() {
  long long id = getClickedId();
  if (id == -1)
    return;

  Email email{id};
  EmailDAO::getInstance().write(email);
  WindowData::getInstance().setEmail(email);
  WindowData::getInstance().removeFromIndex();
  MessageBox::display("Email deleted successfully");
  clearEmailTable();
}

long long InboxWindow::getClickedId() {
  QWidget* widget = qobject_cast<QWidget*>(sender()->parent());
  if (!widget)
    return -1;

  int row = ui->mailTableWidget->indexAt(widget->pos()).row();
  long long id = ui->mailTableWidget->item(row, 0)->text().toLongLong();
  return id;
}

// Slots

void InboxWindow::on_searchButton_clicked() {
  QString query = ui->searchLineEdit->text();

  if (query.isEmpty()) {
    MessageBox::display("Please enter a valid ID or Sender");
    clearEmailTable();
    return;
  }

  bool successfulConversion;
  long long id = query.toLongLong(&successfulConversion);

  if (!successfulConversion && !Tools::validateEmail(ui->searchLineEdit)) {
    MessageBox::display("Please enter a valid email");
    clearEmailTable();
    return;
  }

  if (successfulConversion)
    searchById(id);
  else
    searchBySender(query.toStdString().c_str());
}

void InboxWindow::on_newEmailButton_clicked() {
  WindowData::getInstance().setOperation(WindowData::Write);
  emit showPage(WindowData::EmailPage);
}

void InboxWindow::on_advancedSearchButton_clicked() {
  WindowData& data = WindowData::getInstance();

  if (data.getIndexTree() == WindowData::NoTree) {
    MessageBox::display(
        "Enable Searching by Index in order to use advanced search");
    return;
  }

  QString sender, receiver;
  int searchType = 0;
  AdvancedSearch advSearch{sender, receiver, searchType};
  advSearch.exec();

  searchResults.clear();

  List<long long> queryResults;
  if (searchType == 0) {
    if (!sender.isEmpty())
      queryResults = searchByEmailInTree(sender.toStdString().c_str(),
                                         data.senderSecondaryIndex);
    else
      queryResults = searchByEmailInTree(receiver.toStdString().c_str(),
                                         data.receiverSecondaryIndex);
  } else {
    List<long long> a = searchByEmailInTree(sender.toStdString().c_str(),
                                            data.senderSecondaryIndex);
    List<long long> b = searchByEmailInTree(receiver.toStdString().c_str(),
                                            data.receiverSecondaryIndex);
    auto aPointer = a.begin();
    auto bPointer = b.begin();

    // AND SEARCH
    if (searchType == 1) {
      while (aPointer != a.end() && bPointer != b.end()) {
        qDebug() << "A: " << *aPointer;
        qDebug() << "B: " << *bPointer;

        if (*aPointer == *bPointer) {
          queryResults.pushBack(*aPointer);
          ++aPointer;
          ++bPointer;
        } else if (*aPointer < *bPointer)
          ++aPointer;
        else
          ++bPointer;
      }
    } else {
      // OR SEARCH
      while (aPointer != a.end() && bPointer != b.end()) {
        qDebug() << "A: " << *aPointer;
        qDebug() << "B: " << *bPointer;

        if (*aPointer == *bPointer) {
          queryResults.pushBack(*aPointer);
          ++aPointer;
          ++bPointer;
        } else if (*aPointer < *bPointer) {
          queryResults.pushBack(*aPointer);
          ++aPointer;
        } else {
          queryResults.pushBack(*bPointer);
          ++bPointer;
        }
      }

      while (aPointer != a.end()) {
        queryResults.pushBack(*aPointer);
        ++aPointer;
      }

      while (bPointer != b.end()) {
        queryResults.pushBack(*bPointer);
        ++bPointer;
      }
    }
  }

  if (queryResults.empty()) {
    MessageBox::display("No results found");
    return;
  }

  for (auto id : queryResults)
    searchByIdInTree(id);

  populateEmailTable();
}
