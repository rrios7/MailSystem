#include "messagebox.h"
#include "ui_messagebox.h"

MessageBox::MessageBox(QWidget* parent, const QString& msg)
    : QDialog{parent}, ui{new Ui::MessageBox} {
  ui->setupUi(this);
  this->setWindowFlag(Qt::FramelessWindowHint);
  ui->messageLabel->setText(msg);
}

MessageBox::~MessageBox() {
  delete ui;
}

void MessageBox::display(const QString& msg) {
  MessageBox msgWindow{nullptr, msg};
  msgWindow.move(860, 470);
  msgWindow.exec();
}

void MessageBox::on_okButton_clicked() {
  this->close();
}
