#include "dialog.h"
#include "ui_dialog.h"
#include <QUrlQuery>
#include <QSslSocket>
#include <QtNetwork>
#include <QMessageBox>
#include "user.h"

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
    ui->webView->show();
    connect(ui->webView,SIGNAL(urlChanged(QUrl)),this,SLOT(get_start(QUrl)));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::get_start(QUrl token_url) {
    QString url(token_url.toString());
    url.replace("#","&");
    QUrlQuery tok_url(url);
    QString token = tok_url.queryItemValue("access_token");
    QString id = tok_url.queryItemValue("user_id");
    user = new User(token,id);
}
