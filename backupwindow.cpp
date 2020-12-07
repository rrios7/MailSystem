#include "backupwindow.h"
#include "ui_backupwindow.h"

#include <QFileDialog>
#include <fstream>
#include "emaildao.h"
#include "messagebox.h"
#include "rsa.h"
#include "tools.h"

BackupWindow::BackupWindow(QWidget* parent)
    : QWidget{parent}, ui{new Ui::BackupWindow} {
  ui->setupUi(this);
  ui->mailTableWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
}

BackupWindow::~BackupWindow() {
  delete ui;
}

// Class Methods
void BackupWindow::writeEmail() {
  writeEmail(WindowData::getInstance().getEmail().getId(), false);
}

void BackupWindow::writeEmail(const long long id, bool deleteMode) {
  if (managedFile.right(3) == "csv")
    writeEmailToCSV(id, deleteMode);
  else
    writeEmailToProprietary(id, deleteMode);

  clearEmailTable();
}

void BackupWindow::writeEmailToCSV(const long long id, const bool deleteMode) {
  CSV::Reader reader(managedFile.toStdString());
  reader.skipHeaders();

  CSV::Writer writer("temp.csv", 9);
  writeHeaderToBackup(writer);

  do {
    Email email = readNextEmail(reader);
    if (email.getId() == id) {
      if (deleteMode) {
        continue;
      } else {
        writeEmailToBackup(writer, WindowData::getInstance().getEmail());
        continue;
      }
    }
    writeEmailToBackup(writer, email);
  } while (reader.next());
  reader.close();
  writer.flush();
  renameBackup("temp.csv", managedFile.toStdString().c_str());
}

void BackupWindow::writeEmailToProprietary(const long long id,
                                           const bool deleteMode) {
  std::ifstream reader{managedFile.toStdString(), std::ios::binary};
  std::ofstream writer{"temp.bin", std::ios::binary};

  while (!reader.eof()) {
    Email email;
    reader >> email;

    if (email.getId() == -1)
      break;

    if (email.getId() == id) {
      if (deleteMode) {
        continue;
      } else {
        writer << WindowData::getInstance().getEmail();
        continue;
      }
    }
    writer << email;
  }
  reader.close();
  writer.close();
  renameBackup("temp.bin", managedFile.toStdString().c_str());
}

void BackupWindow::renameBackup(const char* oldName, const char* newName) {
  remove(newName);
  rename(oldName, newName);
}

void BackupWindow::openEmail(long long id,
                             const WindowData::Operation operation) {
  WindowData& data = WindowData::getInstance();
  data.setOperation(operation);

  for (Email email : searchResults)
    if (email.getId() == id)
      data.setEmail(email);

  emit showPage(WindowData::EmailPage);
}

void BackupWindow::searchById(const long long id) {
  searchResults.clear();

  if (managedFile.right(3) == "csv")
    searchByIdCSV(id);
  else
    searchByIdProprietary(id);

  if (searchResults.empty()) {
    MessageBox::display("Couldn't find an email with that ID");
    clearEmailTable();
    return;
  }
  populateEmailTable();
}

void BackupWindow::searchByIdCSV(const long long id) {
  CSV::Reader reader(managedFile.toStdString());
  reader.skipHeaders();

  do {
    Email email = readNextEmail(reader);
    if (email.getId() == id) {
      searchResults.pushBack(email);
      break;
    }
  } while (reader.next());
}

void BackupWindow::searchByIdProprietary(const long long id) {
  std::ifstream file{managedFile.toStdString(), std::ios::binary};

  while (!file.eof()) {
    Email email;
    file >> email;

    if (email.getId() == -1)
      break;

    if (email.getId() == id)
      searchResults.pushBack(email);
  }
}

void BackupWindow::searchBySender(const char* sender) {
  searchResults.clear();

  if (managedFile.right(3) == "csv")
    searchBySenderCSV(sender);
  else
    searchBySenderProprietary(sender);

  if (searchResults.empty()) {
    MessageBox::display("Couldn't find an email with that sender");
    clearEmailTable();
    return;
  }
  populateEmailTable();
}

void BackupWindow::searchBySenderCSV(const char* sender) {
  CSV::Reader reader(managedFile.toStdString());
  reader.skipHeaders();

  do {
    Email email = readNextEmail(reader);
    if (Tools::equalEmails(email.getSender(), sender)) {
      searchResults.pushBack(email);
    }
  } while (reader.next());
}

void BackupWindow::searchBySenderProprietary(const char* sender) {
  std::ifstream file{managedFile.toStdString(), std::ios::binary};

  while (!file.eof()) {
    Email email;
    file >> email;

    if (email.getId() == -1)
      break;

    if (Tools::equalEmails(email.getSender(), sender))
      searchResults.pushBack(email);
  }
}

void BackupWindow::clearEmailTable() {
  ui->mailTableWidget->clearContents();
  ui->mailTableWidget->setRowCount(0);
}

void BackupWindow::populateEmailTable() {
  ui->mailTableWidget->clearContents();
  ui->mailTableWidget->setRowCount(searchResults.length());

  int row = 0;
  for (Email email : searchResults) {
    QWidget* widget = new QWidget();
    QTableWidgetItem* id = new QTableWidgetItem(QString::number(email.getId()));
    QTableWidgetItem* sender = new QTableWidgetItem(email.getSender());
    QTableWidgetItem* subject = new QTableWidgetItem(email.getSubject());

    QPushButton* readButton = new QPushButton();
    readButton->setIcon(QIcon("://icons/eye.png"));
    readButton->setMinimumSize(25, 25);
    readButton->setIconSize(QSize(20, 20));
    readButton->setStyleSheet("border-radius: 12px;");
    readButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    connect(readButton, &QPushButton::clicked, this,
            &BackupWindow::onReadButtonClicked);

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
            &BackupWindow::onEditButtonClicked);

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
            &BackupWindow::onDeleteButtonClicked);

    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(readButton);
    layout->addWidget(editButton);
    layout->addWidget(deleteButton);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(2, 0, 2, 0);
    widget->setLayout(layout);
    ui->mailTableWidget->setItem(row, 0, id);
    ui->mailTableWidget->setItem(row, 1, sender);
    ui->mailTableWidget->setItem(row, 2, subject);
    ui->mailTableWidget->setCellWidget(row, 3, widget);
    ++row;
  }

  ui->mailTableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
}

void BackupWindow::exportToCSV(const QString& fileName) {
  EmailDAO& db = EmailDAO::getInstance();
  CSV::Writer writer(fileName.toStdString(), 9);

  long long id = 0;
  Email email = db.read(id);
  writeHeaderToBackup(writer);
  while (db.next()) {
    if (email.getId() == id && Tools::isValidEmail(email.getSender())) {
      writeEmailToBackup(writer, email);
    }
    email = db.read(++id);
  }
}

void BackupWindow::exportToProprietary(const QString& fileName) {
  EmailDAO& db = EmailDAO::getInstance();
  std::ofstream file{fileName.toStdString(), std::ios::binary};

  long long id = 0;
  Email email = db.read(id);
  while (db.next()) {
    if (email.getId() == id && Tools::isValidEmail(email.getSender())) {
      file << email;
    }
    email = db.read(++id);
  }
}

void BackupWindow::importFromCSV(const QString& fileName) {
  CSV::Reader reader(fileName.toStdString());
  reader.skipHeaders();
  do {
    Email email = readNextEmail(reader);
    if (Tools::isIdInUse(email.getId())) {
      if (QDialog::Accepted ==
          MessageBox::question(
              "This email {ID: " + QString::number(email.getId()) +
              "} alredy exists, do you wish to overwrite it?")) {
        EmailDAO::getInstance().write(email);
        MessageBox::display("Email overwritten successfully");
      }
    } else {
      EmailDAO::getInstance().write(email);
    }
  } while (reader.next());
  MessageBox::display("Backup imported successfully");
}

void BackupWindow::importFromProprietary(const QString& fileName) {
  std::ifstream file{fileName.toStdString(), std::ios::binary};
  while (!file.eof()) {
    Email email;
    file >> email;

    if (email.getId() == -1)
      break;

    if (Tools::isIdInUse(email.getId())) {
      if (QDialog::Accepted ==
          MessageBox::question(
              "This email {ID: " + QString::number(email.getId()) +
              "} alredy exists, do you wish to overwrite it?")) {
        EmailDAO::getInstance().write(email);
        MessageBox::display("Email overwritten successfully");
      }
    } else {
      EmailDAO::getInstance().write(email);
    }
  }
  MessageBox::display("Backup imported successfully");
}

void BackupWindow::writeHeaderToBackup(CSV::Writer& writer) {
  writer << "ID"
         << "Date"
         << "Time"
         << "Subject"
         << "Sender"
         << "Receiver"
         << "CC"
         << "BCC"
         << "Content";
}

void BackupWindow::writeEmailToBackup(CSV::Writer& writer, Email email) {
  writer << email.getId() << email.getDate() << email.getTime()
         << email.getSubject() << email.getSender() << email.getReceiver()
         << email.getCC() << email.getBCC() << email.getContent();
}

Email BackupWindow::readNextEmail(CSV::Reader& reader) {
  Email email;

  if (reader.next()) {
    email.setId(std::stoll(reader.getNextField()));
    email.setDate(reader.getNextField().c_str());
    email.setTime(reader.getNextField().c_str());
    email.setSubject(reader.getNextField().c_str());
    email.setSender(reader.getNextField().c_str());
    email.setReceiver(reader.getNextField().c_str());
    email.setCC(reader.getNextField().c_str());
    email.setBCC(reader.getNextField().c_str());
    email.setContent(reader.getNextField().c_str());
  }

  return email;
}

// Pseudo Slots
void BackupWindow::onReadButtonClicked() {
  long long id = getClickedId();
  if (id == -1)
    return;

  openEmail(id, WindowData::Read);
}

void BackupWindow::onEditButtonClicked() {
  long long id = getClickedId();
  if (id == -1)
    return;

  openEmail(id, WindowData::Update);
}

void BackupWindow::onDeleteButtonClicked() {
  long long id = getClickedId();
  if (id == -1)
    return;
  writeEmail(id, true);
  MessageBox::display("Email deleted successfully");
}

long long BackupWindow::getClickedId() {
  QWidget* widget = qobject_cast<QWidget*>(sender()->parent());
  if (!widget)
    return -1;

  int row = ui->mailTableWidget->indexAt(widget->pos()).row();
  long long id = ui->mailTableWidget->item(row, 0)->text().toLongLong();
  return id;
}

void BackupWindow::on_exportButton_clicked() {
  QString fileName = QFileDialog::getSaveFileName(
      this, "Backup", "C:/",
      "CSV Backup Files (*.csv);; Proprietary Backup Files (*.bin)");

  if (fileName != "") {
    if (fileName.right(3) == "csv")
      exportToCSV(fileName);
    else
      exportToProprietary(fileName);
    MessageBox::display("Backup exported successfully");
  }
}

void BackupWindow::on_importButton_clicked() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "Backup", "C:/",
      "CSV Backup Files (*.csv);; Proprietary Backup Files (*.bin)");

  if (fileName != "") {
    if (fileName.right(3) == "csv")
      importFromCSV(fileName);
    else
      importFromProprietary(fileName);
  }
}

void BackupWindow::on_manageButton_clicked() {
  if (managedFile != "") {
    clearEmailTable();
    managedFile = "";
    ui->managedLabel->setText("No backup loaded");
    ui->manageButton->setText("Manage Backup");
    ui->manageButton->setStyleSheet("");
    return;
  }

  QString fileName = QFileDialog::getOpenFileName(
      this, "Backup", "C:/",
      "CSV Backup Files (*.csv);; Proprietary Backup Files (*.bin)");
  if (fileName != "") {
    managedFile = fileName;
    ui->managedLabel->setText(managedFile);
    ui->manageButton->setText("Stop Management");
    ui->manageButton->setStyleSheet(
        "#manageButton { background-color: rgb(242, 22, 43);}"
        "#manageButton:hover { background-color: rgb(203, 11, 29);}");
  }
}

void BackupWindow::on_searchButton_clicked() {
  if (managedFile == "") {
    MessageBox::display("No backup is being managed");
    return;
  }

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

void BackupWindow::on_encryptButton_clicked() {
  QString inputFileName = QFileDialog::getOpenFileName(
      this, "Backup", "C:/", "CSV Backup Files (*.csv)");
  QString outputFileName = QFileDialog::getSaveFileName(
      this, "Encrypted Backup", "C:/", "CSV Backup Files (*.csv);");
  QString privateKeyFileName = QFileDialog::getSaveFileName(
      this, "Encryption Key", "C:/", "Encryption Key Files (*.bin)");

  std::ifstream input{inputFileName.toStdString()};
  std::ofstream output{outputFileName.toStdString()};

  std::ofstream privateKey{privateKeyFileName.toStdString(), std::ios::binary};
  List<std::pair<int, int>> keys = RSA::generateKeys();
  privateKey.write(reinterpret_cast<char*>(&keys.back()),
                   sizeof(std::pair<int, int>));

  std::string password = ui->passwordLineEdit->text().toStdString();

  if (!password.empty())
    output << RSA::encrypt("requirePass=" + password, keys.front());

  std::string line;
  while (!input.eof()) {
    getline(input, line, '\r');
    output << RSA::encrypt(line, keys.front()) << "\r";
  }
}

void BackupWindow::on_decryptButton_clicked() {
  QString inputFileName = QFileDialog::getOpenFileName(
      this, "Encrypted Backup", "C:/", "CSV Backup Files (*.csv)");
  QString outputFileName = QFileDialog::getSaveFileName(
      this, "Backup", "C:/", "CSV Backup Files (*.csv);");
  QString privateKeyFileName = QFileDialog::getOpenFileName(
      this, "Encryption Key", "C:/", "Encryption Key Files (*.bin)");

  std::ifstream input{inputFileName.toStdString()};
  std::ofstream output{outputFileName.toStdString()};

  std::ifstream privateKey{privateKeyFileName.toStdString(), std::ios::binary};
  std::pair<int, int> key;
  privateKey.read(reinterpret_cast<char*>(&key), sizeof(std::pair<int, int>));

  std::string line;
  getline(input, line, '\r');
  line = RSA::decrypt(line, key);

  std::string password = ui->passwordLineEdit->text().toStdString();

  if (line.substr(0, 12) == "requirePass=") {
    if (password.empty()) {
      MessageBox::display("This file needs a password to be decrypted");
      return;
    }

    if (line.substr(12, password.size()) != password) {
      MessageBox::display("Wrong password for encrypted file");
      return;
    }

    output << line.substr(12 + password.size()) << '\r';
  } else
    output << line << '\r';

  while (!input.eof()) {
    getline(input, line, '\r');
    output << RSA::decrypt(line, key) << '\r';
  }
}
