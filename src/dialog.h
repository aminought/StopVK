#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QUrl>
#include <QFile>
#include <QWebView>
#include <QTextBrowser>
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

    QNetworkAccessManager* net_access_get_friends;

    QPushButton* delete_button;
    QWebView* auth_page;
    QTextBrowser* status;

    void show_delete_dialog();
    QVariantList reply_to_list(QNetworkReply* net_reply);

public slots:
    void get_start(QUrl url);
    void delete_all();
    void delete_friends();
    void get_friends(QNetworkReply* reply);

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
