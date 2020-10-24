#include "messagebox.h"
#include "ui_messagebox.h"

MessageBox::MessageBox(QWidget* parent, const QString& msg, bool question)
    : QDialog{parent}, ui{new Ui::MessageBox} {
  ui->setupUi(this);
  this->setWindowFlag(Qt::FramelessWindowHint);
  ui->messageLabel->setText(msg);

  if (question) {
    ui->okButton->hide();
    connect(ui->yesButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->noButton, &QPushButton::clicked, this, &QDialog::reject);
  } else {
    ui->yesButton->hide();
    ui->noButton->hide();
    connect(ui->okButton, &QPushButton::clicked, this, &QDialog::accept);
  }
}

MessageBox::~MessageBox() {
  delete ui;
}

int MessageBox::display(const QString& msg) {
  MessageBox msgWindow{nullptr, msg};
  msgWindow.move(860, 470);
  return msgWindow.exec();
}

int MessageBox::question(const QString& msg) {
  MessageBox msgWindow{nullptr, msg, true};
  msgWindow.move(860, 470);
  return msgWindow.exec();
}
