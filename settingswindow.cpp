#include "settingswindow.h"
#include "messagebox.h"
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
  data.setIndexType(
      static_cast<WindowData::Index>(data.getIndexType() ^ WindowData::AVL));
}

void SettingsWindow::on_memorySearchCheckBox_clicked() {
  WindowData& data = WindowData::getInstance();
  if (ui->memorySearchCheckBox->isChecked())
    data.setMemorySearch(true);
  else
    data.setMemorySearch(false);
}

void SettingsWindow::on_paginatedIndexCheckBox_clicked() {
  WindowData& data = WindowData::getInstance();
  data.setIndexType(static_cast<WindowData::Index>(data.getIndexType() ^
                                                   WindowData::Paginated));
}

void SettingsWindow::on_hashTableCheckBox_clicked() {
  WindowData& data = WindowData::getInstance();
  data.setIndexType(static_cast<WindowData::Index>(data.getIndexType() ^
                                                   WindowData::HashTable));

  if (data.getIndexType() & WindowData::HashTable) {
    data.loadHashTable();
  } else
    data.senderHashTable.clear();
}

void SettingsWindow::on_serializeButton_clicked() {
  WindowData& data = WindowData::getInstance();
  if (data.senderHashTable.empty()) {
    MessageBox::display("Can't serialize an empty Hash Table");
    return;
  }
  data.senderHashTable.serialize();
}

void SettingsWindow::on_deserializeButton_clicked() {
  WindowData& data = WindowData::getInstance();
  if (!data.senderHashTable.deserialize()) {
    MessageBox::display("No serialized Hash Table found");
    return;
  }
  data.setIndexType(static_cast<WindowData::Index>(data.getIndexType() |
                                                   WindowData::HashTable));
  ui->hashTableCheckBox->setChecked(true);

  qDebug() << data.senderHashTable.count();
}
