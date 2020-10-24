#include "inboxwindow.h"
#include "ui_inboxwindow.h"

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
}

void InboxWindow::openEmail(long long id,
                            const WindowData::Operation operation) {
  WindowData& data = WindowData::getInstance();
  data.setOperation(operation);
  data.getEmail() = EmailDAO::getInstance().read(id);
  emit showPage(WindowData::EmailPage);
}

void InboxWindow::searchById(const long long id) {
  Email email = EmailDAO::getInstance().read(id);
  bool emailExists = Tools::isValidEmail(email.getSender());

  if (!emailExists) {
    MessageBox::display("Couldn't find an email with that ID");
    clearEmailTable();
    return;
  }

  populateEmailTable(List<Email>{email});
}

void InboxWindow::searchBySender(const char* sender) {
  EmailDAO& db = EmailDAO::getInstance();
  List<Email> emailList;

  long long id = 0;
  Email email = db.read(id);

  while (db.next()) {
    if (email.getId() == id && Tools::equalEmails(email.getSender(), sender)) {
      emailList.pushBack(email);
    }
    email = db.read(++id);
  }

  if (emailList.empty()) {
    MessageBox::display("Couldn't find an email with that sender");
    clearEmailTable();
    return;
  }
  populateEmailTable(emailList);
}

void InboxWindow::clearEmailTable() {
  ui->mailTableWidget->clearContents();
  ui->mailTableWidget->setRowCount(0);
}

void InboxWindow::populateEmailTable(List<Email> emailList) {
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
        "QPushButton {border-radius: 12px; background-color: rgb(242, 22, 43);}"
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
    ui->mailTableWidget->setItem(row, 2, subject);
    ui->mailTableWidget->setCellWidget(row, 3, widget);
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
