#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QUrl>
#include <QFile>
#include <QWebView>
#include "user.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    User* user;
    QFile* settings;

    QPushButton* delete_button;
    QWebView* auth_page;

    void show_delete_button();
    void show_status_text(QString);

public slots:
    void get_start(QUrl url);
    void delete_all();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
