#ifndef BACKUPWINDOW_H
#define BACKUPWINDOW_H

#include <QWidget>
#include "csv.h"
#include "list.h"
#include "windowdata.h"

namespace Ui {
class BackupWindow;
}

class BackupWindow : public QWidget
{
  Q_OBJECT

 public:
  explicit BackupWindow(QWidget *parent = nullptr);
  ~BackupWindow();

 public:
  void writeEmail();

  // Class Methods
 private:
  void writeEmail(const long long id, const bool deleteMode);
  void writeEmailToCSV(const long long id, const bool deleteMode);
  void writeEmailToProprietary(const long long id, const bool deleteMode);

  void renameBackup(const char* oldName, const char* newName);

  void openEmail(const long long id, const WindowData::Operation operation);

  void searchById(const long long id);
  void searchByIdCSV(const long long id);
  void searchByIdProprietary(const long long id);

  void searchBySender(const char* sender);
  void searchBySenderCSV(const char* sender);
  void searchBySenderProprietary(const char* sender);

  void clearEmailTable();
  void populateEmailTable();

  void exportToCSV(const QString& fileName);
  void exportToProprietary(const QString& fileName);

  void importFromCSV(const QString& fileName);
  void importFromProprietary(const QString& fileName);

  Email readNextEmail(CSV::Reader& reader);
  void writeHeaderToBackup(CSV::Writer& writer);
  void writeEmailToBackup(CSV::Writer& writer, Email email);

  // Pseudo Slots
 private:
  void onReadButtonClicked();
  void onEditButtonClicked();
  void onDeleteButtonClicked();
  long long getClickedId();

 private slots:
  void on_exportButton_clicked();

  void on_importButton_clicked();

  void on_manageButton_clicked();

  void on_searchButton_clicked();

 signals:
  void showPage(const WindowData::Page page);

 private:
  Ui::BackupWindow* ui;

  List<Email> searchResults;

  QString managedFile;
};

#endif // BACKUPWINDOW_H
