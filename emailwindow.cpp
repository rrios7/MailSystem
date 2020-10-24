#include "emailwindow.h"
#include "ui_emailwindow.h"

#include <QDate>
#include <QTime>
#include "messagebox.h"
#include "tools.h"
#include "windowdata.h"

EmailWindow::EmailWindow(QWidget* parent)
    : QWidget{parent}, ui{new Ui::EmailWindow}, timer{new QTimer} {
  ui->setupUi(this);
  connect(timer, &QTimer::timeout, this, &EmailWindow::onTimeout);
}

EmailWindow::~EmailWindow()
{
  delete ui;
}

// Class Methods
void EmailWindow::startTimer() {
  timer->start(1000);
}

void EmailWindow::stopTimer() {
  timer->stop();
}

void EmailWindow::setControls() {
  WindowData& data = WindowData::getInstance();

  if (data.getOperation() == WindowData::Read) {
    ui->idSpinBox->setEnabled(false);
    ui->subjectLineEdit->setEnabled(false);
    ui->senderLineEdit->setEnabled(false);
    ui->receiverLineEdit->setEnabled(false);
    ui->ccLineEdit->setEnabled(false);
    ui->bccLineEdit->setEnabled(false);
    ui->contentLineEdit->setEnabled(false);
    ui->writeButton->hide();
    return;
  }

  ui->idSpinBox->setEnabled(true);
  ui->subjectLineEdit->setEnabled(true);
  ui->senderLineEdit->setEnabled(true);
  ui->receiverLineEdit->setEnabled(true);
  ui->ccLineEdit->setEnabled(true);
  ui->bccLineEdit->setEnabled(true);
  ui->contentLineEdit->setEnabled(true);
  ui->writeButton->show();

  if (data.getOperation() == WindowData::Write) {
    startTimer();
    ui->writeButton->setIcon(QIcon("://icons/send.png"));
    ui->writeButton->setText("Send email");
    return;
  }

  if (data.getPreviousPage() != WindowData::BackupPage)
    ui->idSpinBox->setEnabled(false);

  ui->writeButton->setIcon(QIcon("://icons/save.png"));
  ui->writeButton->setText("Save email");
}

void EmailWindow::setFields() {
  WindowData& data = WindowData::getInstance();
  Email& email = data.getEmail();

  ui->idSpinBox->setStyleSheet("");
  ui->senderLineEdit->setStyleSheet("");
  ui->receiverLineEdit->setStyleSheet("");
  ui->ccLineEdit->setStyleSheet("");
  ui->bccLineEdit->setStyleSheet("");

  if (data.getOperation() == WindowData::Write) {
    ui->dateLabel->setText("Date:");
    ui->timeLabel->setText("Time:");
    ui->idSpinBox->setValue(0);
    ui->subjectLineEdit->setText("");
    ui->senderLineEdit->setText("");
    ui->receiverLineEdit->setText("");
    ui->ccLineEdit->setText("");
    ui->bccLineEdit->setText("");
    ui->contentLineEdit->setPlainText("");
    return;
  }

  ui->dateLabel->setText("Date: " + QString::fromStdString(email.getDate()));
  ui->timeLabel->setText("Time: " + QString::fromStdString(email.getTime()));
  ui->idSpinBox->setValue(email.getId());
  ui->subjectLineEdit->setText(email.getSubject());
  ui->senderLineEdit->setText(email.getSender());
  ui->receiverLineEdit->setText(email.getReceiver());
  ui->ccLineEdit->setText(email.getCC());
  ui->bccLineEdit->setText(email.getBCC());
  ui->contentLineEdit->setPlainText(email.getContent());
}

bool EmailWindow::validateId() {
  WindowData& data = WindowData::getInstance();

  if (data.getPreviousPage() == WindowData::BackupPage)
    return true;

  long long id = ui->idSpinBox->value();
  if (data.getOperation() == WindowData::Write && Tools::isIdInUse(id)) {
    ui->idSpinBox->setStyleSheet("border: 2px solid rgb(253, 1, 94);");
    return false;
  }

  ui->idSpinBox->setStyleSheet("border: 2px solid rgb(200, 200, 200);");
  return true;
}

bool EmailWindow::validateEmail(QLineEdit* lineEdit) {
  std::string email = lineEdit->text().toStdString();

  if (!Tools::isValidEmail(email.c_str())) {
    lineEdit->setStyleSheet("border: 2px solid rgb(253, 1, 94);");
    return false;
  }

  lineEdit->setStyleSheet("border: 2px solid rgb(200, 200, 200);");
  return true;
}

bool EmailWindow::validateEmailList(QLineEdit* lineEdit) {
  QStringList emailList = lineEdit->text().split(';');

  for (QString email : emailList) {
    if (email.length() > 0 &&
        !Tools::isValidEmail(email.toStdString().c_str())) {
      lineEdit->setStyleSheet("border: 2px solid rgb(253, 1, 94);");
      return false;
    }
  }

  lineEdit->setStyleSheet("border: 2px solid rgb(200, 200, 200);");
  return true;
}

bool EmailWindow::validateFields() {
  if (!validateId()) {
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
    MessageBox::display("One or more non valid emails on BCC");
    return false;
  }

  if (ui->contentLineEdit->toPlainText().length() > 1024) {
    MessageBox::display("Email content max length is 1024 characters long");
    return false;
  }
  return true;
}

// Pseudo-Slots
void EmailWindow::onTimeout() {
  ui->dateLabel->setText("Date: " +
                         QDate::currentDate().toString("dd/MM/yyyy"));
  ui->timeLabel->setText("Time: " + QTime::currentTime().toString("hh:mm"));
}

// Slots

void EmailWindow::on_idSpinBox_editingFinished() {
  validateId();
}

void EmailWindow::on_senderLineEdit_editingFinished() {
  validateEmail(ui->senderLineEdit);
}

void EmailWindow::on_receiverLineEdit_editingFinished() {
  validateEmail(ui->receiverLineEdit);
}

void EmailWindow::on_ccLineEdit_editingFinished() {
  validateEmailList(ui->ccLineEdit);
}

void EmailWindow::on_bccLineEdit_editingFinished() {
  validateEmailList(ui->bccLineEdit);
}

void EmailWindow::on_writeButton_clicked() {
  WindowData& data = WindowData::getInstance();
  Email& email = data.getEmail();

  if (!validateFields())
    return;

  long long id = ui->idSpinBox->value();
  email.setId(id);

  if (data.getOperation() == WindowData::Write) {
    std::string date =
        QDate::currentDate().toString("dd/MM/yyyy").toStdString();
    std::string time = QTime::currentTime().toString("hh:mm").toStdString();
    email.setDate(date.c_str());
    email.setTime(time.c_str());
  }

  std::string subject = ui->subjectLineEdit->text().toStdString();
  email.setSubject(subject.c_str());
  std::string sender = ui->senderLineEdit->text().toStdString();
  email.setSender(sender.c_str());
  std::string receiver = ui->receiverLineEdit->text().toStdString();
  email.setReceiver(receiver.c_str());
  std::string cc = ui->ccLineEdit->text().toStdString();
  email.setCC(cc.c_str());
  std::string bcc = ui->bccLineEdit->text().toStdString();
  email.setBCC(bcc.c_str());
  std::string content = ui->contentLineEdit->toPlainText().toStdString();
  email.setContent(content.c_str());

  if (data.getOperation() == WindowData::Write) {
    MessageBox::display("Email written successfully");
  } else if (data.getOperation() == WindowData::Update) {
    MessageBox::display("Email updated successfully");
  }

  if (data.getPreviousPage() == WindowData::BackupPage)
    emit writeBackupEmail();
  else
    emit writeEmail();
}
