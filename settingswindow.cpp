#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "windowdata.h"

SettingsWindow::SettingsWindow(QWidget* parent)
    : QWidget(parent), ui(new Ui::SettingsWindow) {
  ui->setupUi(this);
}

SettingsWindow::~SettingsWindow() {
  delete ui;
}

void SettingsWindow::on_primaryIndexAVLButton_clicked() {
  WindowData& data = WindowData::getInstance();
  if (ui->primaryIndexAVLButton->isChecked())
    data.setIndexTree(WindowData::AVL);
  else
    data.setIndexTree(WindowData::NoTree);
}

void SettingsWindow::on_memorySearchCheckBox_clicked() {
  WindowData& data = WindowData::getInstance();
  if (ui->memorySearchCheckBox->isChecked())
    data.setMemorySearch(true);
  else
    data.setMemorySearch(false);
}
