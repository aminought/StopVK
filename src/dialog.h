#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QUrl>
#include <QFile>
#include <QWebView>
#include <QTextBrowser>
#include <QEventLoop>
#include <QTimer>
#include <QString>
#include <QUrlQuery>
#include <QSslSocket>
#include <QtNetwork>
#include <QMessageBox>
#include <QDataStream>
#include <QPushButton>
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

    QPushButton* delete_button;
    QWebView* auth_page;
    QTextBrowser* status;

    QByteArray GET(QUrlQuery);
    void show_delete_dialog();
    QVariantList bytearray_to_list(QByteArray array);

public slots:
    void get_start(QUrl url);
    void delete_all();
    void delete_friends();
    void delete_photos();
    void delete_audios();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
