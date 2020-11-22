#include "advancedsearch.h"
#include "ui_advancedsearch.h"

#include "messagebox.h"
#include "tools.h"

AdvancedSearch::AdvancedSearch(QString& sender,
                               QString& receiver,
                               int& searchType,
                               QWidget* parent)
    : sender{sender},
      receiver{receiver},
      searchType{searchType},
      QDialog{parent},
      ui{new Ui::AdvancedSearch} {
  ui->setupUi(this);
  setWindowFlag(Qt::FramelessWindowHint);
}

AdvancedSearch::~AdvancedSearch()
{
  delete ui;
}

void AdvancedSearch::on_searchButton_clicked() {
  QString sender = ui->senderLineEdit->text();
  QString receiver = ui->receiverLineEdit->text();

  if (sender.isEmpty() && receiver.isEmpty()) {
    MessageBox::display("Fill at least one of the email fields");
    return;
  }

  if (!sender.isEmpty() && !receiver.isEmpty() &&
      ui->normalSearchRadioButton->isChecked()) {
    MessageBox::display(
        "Special search must be activated to search using two emails");
    return;
  }

  if (!sender.isEmpty() && !Tools::validateEmail(ui->senderLineEdit)) {
    MessageBox::display("Invalid sender email");
    return;
  }

  if (!receiver.isEmpty() && !Tools::validateEmail(ui->receiverLineEdit)) {
    MessageBox::display("Invalid receiver email");
    return;
  }

  this->sender = sender;
  this->receiver = receiver;
  this->searchType = (ui->normalSearchRadioButton->isChecked()
                          ? 0
                          : ui->andSearchRadioButton->isChecked() ? 1 : 2);
  this->close();
}

void AdvancedSearch::on_closeButton_clicked() {
  this->close();
}
