#include "dialog.h"
#include "ui_dialog.h"
#include <QUrlQuery>
#include <QSslSocket>
#include <QtNetwork>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("Stop VK!");
    QUrlQuery url("https://oauth.vk.com/authorize?");
    url.addQueryItem("client_id","4534170");
    url.addQueryItem("scope","262144");
    url.addQueryItem("redirect_uri","http://oauth.vk.com/blank.html");
    url.addQueryItem("display","popup");
    url.addQueryItem("response_type","token");
    ui->webView->load(QUrl(url.toString()));
    ui->url_label->setText(url.toString());
    ui->webView->show();
}

Dialog::~Dialog()
{
    delete ui;
}
