#include "mainwindow.h"
#include <QDate>
#include <QDebug>
#include <QTime>
#include "emailtools.h"
#include "messagebox.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow{parent},
      ui{new Ui::MainWindow},
      timer{new QTimer},
      emailData{nullptr} {
  ui->setupUi(this);

  // UI Cofiguration
  setWindowFlag(Qt::FramelessWindowHint);
  ui->mailTableWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
}

MainWindow::~MainWindow() {
  delete ui;
}

// Pseudo slots
void MainWindow::onTimeout() {
  ui->dateLabel->setText("Date: " +
                         QDate::currentDate().toString("dd/MM/yyyy"));
  ui->timeLabel->setText("Time: " + QTime::currentTime().toString("hh:mm"));
}

void MainWindow::onReadButtonClicked() {
  long long id = getClickedId();
  if (id == -1)
    return;

  openEmail(id, true);
}

void MainWindow::onEditButtonClicked() {
  long long id = getClickedId();
  if (id == -1)
    return;

  openEmail(id, false);
}

void MainWindow::onDeleteButtonClicked() {
  long long id = getClickedId();
  if (id == -1)
    return;

  Email email{id};
  EmailTools::writeEmail(email);
  MessageBox::display("Email deleted successfully");
  openInbox();
}

// Class Methods
long long MainWindow::getClickedId() {
  QWidget* widget = qobject_cast<QWidget*>(sender()->parent());
  if (!widget)
    return -1;

  int row = ui->mailTableWidget->indexAt(widget->pos()).row();
  long long id = ui->mailTableWidget->item(row, 0)->text().toLongLong();
  return id;
}

void MainWindow::populateEmailTable(List<Email> emailList) {
  ui->mailTableWidget->clearContents();
  ui->mailTableWidget->setRowCount(emailList.length());

  int row = 0;
  for (Email email : emailList) {
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
            &MainWindow::onReadButtonClicked);

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
            &MainWindow::onEditButtonClicked);

    QPushButton* deleteButton = new QPushButton();
    deleteButton->setIcon(QIcon("://icons/delete.png"));
    deleteButton->setMinimumSize(25, 25);
    deleteButton->setIconSize(QSize(20, 20));
    deleteButton->setStyleSheet(
        "QPushButton {border-radius: 12px; background-color: rgb(242, 22, 43);}"
        "QPushButton:hover { background-color: rgb(203, 11, 29);}");
    deleteButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    connect(deleteButton, &QPushButton::clicked, this,
            &MainWindow::onDeleteButtonClicked);

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

void MainWindow::clearEmailTable() {
  ui->mailTableWidget->clearContents();
  ui->mailTableWidget->setRowCount(0);
}

void MainWindow::connectTimer() {
  connect(timer, &QTimer::timeout, this, &MainWindow::onTimeout);
  timer->start(1000);
}

void MainWindow::disconnectTimer() {
  timer->stop();
}

void MainWindow::searchById(const long long id) {
  Email email = EmailTools::readEmail(id);
  bool emailExists = EmailTools::isValidEmail(email.getSender());

  if (!emailExists) {
    MessageBox::display("Couldn't find an email with that ID");
    clearEmailTable();
    return;
  }

  populateEmailTable(List<Email>{email});
}

void MainWindow::searchBySender(const char* sender) {
  List<Email> inbox;
  EmailTools::readInbox(inbox);

  List<Email> emailList;

  for (Email email : inbox) {
    if (EmailTools::equalEmails(email.getSender(), sender)) {
      emailList.pushBack(email);
    }
  }

  if (emailList.empty()) {
    MessageBox::display("Couldn't find an email with that sender");
    clearEmailTable();
    return;
  }
  populateEmailTable(emailList);
}

void MainWindow::openInbox() {
  ui->content->setCurrentIndex(0);
  disconnectTimer();
  clearEmailData();
  resetEmailControls();
  clearEmailTable();
}

void MainWindow::openEmail(const long long id, const bool readOnly) {
  loadEmailData(EmailTools::readEmail(id));
  setEmailControls(readOnly);
  displayEmailData();
  ui->content->setCurrentIndex(1);
}

void MainWindow::setEmailControls(const bool readOnly) {
  ui->idSpinBox->setEnabled(false);

  if (readOnly) {
    ui->subjectLineEdit->setEnabled(false);
    ui->senderLineEdit->setEnabled(false);
    ui->receiverLineEdit->setEnabled(false);
    ui->ccLineEdit->setEnabled(false);
    ui->bccLineEdit->setEnabled(false);
    ui->contentLineEdit->setEnabled(false);
    ui->writeButton->hide();
    return;
  }

  ui->writeButton->setIcon(QIcon("://icons/save.png"));
  ui->writeButton->setText("Save");
}

void MainWindow::resetEmailControls() {
  ui->idSpinBox->setEnabled(true);
  ui->subjectLineEdit->setEnabled(true);
  ui->senderLineEdit->setEnabled(true);
  ui->receiverLineEdit->setEnabled(true);
  ui->ccLineEdit->setEnabled(true);
  ui->bccLineEdit->setEnabled(true);
  ui->contentLineEdit->setEnabled(true);
  ui->writeButton->setIcon(QIcon("://icons/send.png"));
  ui->writeButton->setText("Send email");
  ui->writeButton->show();
}

void MainWindow::loadEmailData(const Email& email) {
  if (emailData != nullptr) {
    delete emailData;
  }

  emailData = new Email(email);
}

void MainWindow::clearEmailData() {
  emailData = nullptr;
  ui->dateLabel->setText("Date:");
  ui->timeLabel->setText("Time:");
  ui->idSpinBox->setValue(0);
  ui->idSpinBox->setStyleSheet("");
  ui->subjectLineEdit->setText("");
  ui->subjectLineEdit->setStyleSheet("");
  ui->senderLineEdit->setText("");
  ui->senderLineEdit->setStyleSheet("");
  ui->receiverLineEdit->setText("");
  ui->receiverLineEdit->setStyleSheet("");
  ui->ccLineEdit->setText("");
  ui->ccLineEdit->setStyleSheet("");
  ui->bccLineEdit->setText("");
  ui->bccLineEdit->setStyleSheet("");
  ui->contentLineEdit->setPlainText("");
}

void MainWindow::displayEmailData() {
  ui->dateLabel->setText("Date: " +
                         QString::fromStdString(emailData->getDate()));
  ui->timeLabel->setText("Time: " +
                         QString::fromStdString(emailData->getTime()));
  ui->idSpinBox->setValue(emailData->getId());
  ui->subjectLineEdit->setText(emailData->getSubject());
  ui->senderLineEdit->setText(emailData->getSender());
  ui->receiverLineEdit->setText(emailData->getReceiver());
  ui->ccLineEdit->setText(emailData->getCC());
  ui->bccLineEdit->setText(emailData->getBCC());
  ui->contentLineEdit->setPlainText(emailData->getContent());
}

bool MainWindow::validateEmailId() {
  long long id = ui->idSpinBox->value();
  if (emailData == nullptr && EmailTools::isIdInUse(id)) {
    ui->idSpinBox->setStyleSheet("border: 2px solid rgb(253, 1, 94);");
    return false;
  }

  ui->idSpinBox->setStyleSheet("border: 2px solid rgb(200, 200, 200);");
  return true;
}

bool MainWindow::validateEmail(QLineEdit* lineEdit) {
  std::string email = lineEdit->text().toStdString();

  if (!EmailTools::isValidEmail(email.c_str())) {
    lineEdit->setStyleSheet("border: 2px solid rgb(253, 1, 94);");
    return false;
  }

  lineEdit->setStyleSheet("border: 2px solid rgb(200, 200, 200);");
  return true;
}

bool MainWindow::validateEmailList(QLineEdit* lineEdit) {
  QStringList emailList = lineEdit->text().split(';');

  for (QString email : emailList) {
    if (email.length() > 0 &&
        !EmailTools::isValidEmail(email.toStdString().c_str())) {
      lineEdit->setStyleSheet("border: 2px solid rgb(253, 1, 94);");
      return false;
    }
  }

  lineEdit->setStyleSheet("border: 2px solid rgb(200, 200, 200);");
  return true;
}

bool MainWindow::validateFields() {
  if (!validateEmailId()) {
    MessageBox::display("Email ID alredy in use");
    return false;
  }

  if (!validateEmail(ui->senderLineEdit)) {
    MessageBox::display("Not a valid sender email");
    return false;
  }

  if (!validateEmail(ui->receiverLineEdit)) {
    MessageBox::display("Not a valid receiver email");
    return false;
  }

  if (!validateEmailList(ui->ccLineEdit)) {
    MessageBox::display("One or more non valid emails on CC");
    return false;
  }

  if (!validateEmailList(ui->bccLineEdit)) {
    MessageBox::display("One or more non valid emails on CC");
    return false;
  }

  if (ui->contentLineEdit->toPlainText().length() > 1024) {
    MessageBox::display("Email content max length is 1024 characters long");
    return false;
  }
  return true;
}

// Sidebar slots

void MainWindow::on_inboxButton_clicked() {
  openInbox();
}

void MainWindow::on_backupButton_clicked() {
  ui->content->setCurrentIndex(2);
}

// Inbox slots

void MainWindow::on_searchButton_clicked() {
  QString query = ui->searchLineEdit->text();

  if (query.isEmpty()) {
    MessageBox::display("Please enter a valid ID or Sender");
    clearEmailTable();
    return;
  }

  bool successfulConversion;
  long long id = query.toLongLong(&successfulConversion);

  if (!successfulConversion && !validateEmail(ui->searchLineEdit)) {
    MessageBox::display("Please enter a valid email");
    clearEmailTable();
    return;
  }

  on_inboxButton_clicked();

  if (successfulConversion)
    searchById(id);
  else
    searchBySender(query.toStdString().c_str());
}

void MainWindow::on_newEmailButton_clicked() {
  ui->content->setCurrentIndex(1);
  connectTimer();
}

// Email slots
void MainWindow::on_idSpinBox_editingFinished() {
  validateEmailId();
}

void MainWindow::on_senderLineEdit_editingFinished() {
  validateEmail(ui->senderLineEdit);
}

void MainWindow::on_receiverLineEdit_editingFinished() {
  validateEmail(ui->receiverLineEdit);
}

void MainWindow::on_ccLineEdit_editingFinished() {
  validateEmailList(ui->ccLineEdit);
}

void MainWindow::on_bccLineEdit_editingFinished() {
  validateEmailList(ui->bccLineEdit);
}

void MainWindow::on_writeButton_clicked() {
  if (!validateFields())
    return;

  Email newEmail;
  long long id = ui->idSpinBox->value();
  newEmail.setId(id);

  if (this->emailData == nullptr) {
    std::string date =
        QDate::currentDate().toString("dd/MM/yyyy").toStdString();
    std::string time = QTime::currentTime().toString("hh:mm").toStdString();
    newEmail.setDate(date.c_str());
    newEmail.setTime(time.c_str());
  } else {
    newEmail.setDate(this->emailData->getDate());
    newEmail.setTime(this->emailData->getTime());
  }

  std::string subject = ui->subjectLineEdit->text().toStdString();
  newEmail.setSubject(subject.c_str());
  std::string sender = ui->senderLineEdit->text().toStdString();
  newEmail.setSender(sender.c_str());
  std::string receiver = ui->receiverLineEdit->text().toStdString();
  newEmail.setReceiver(receiver.c_str());
  std::string cc = ui->ccLineEdit->text().toStdString();
  newEmail.setCC(cc.c_str());
  std::string bcc = ui->bccLineEdit->text().toStdString();
  newEmail.setBCC(bcc.c_str());
  std::string content = ui->contentLineEdit->toPlainText().toStdString();
  newEmail.setContent(content.c_str());

  EmailTools::writeEmail(newEmail);
  MessageBox::display("Email written successfully");
  clearEmailData();
  openInbox();
}

void MainWindow::on_createBackupButton_clicked() {
  EmailTools::createBackup();
  MessageBox::display("Backup created successfully");
}
