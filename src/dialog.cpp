#include "dialog.h"
#include "ui_dialog.h"
#include <QUrlQuery>
#include <QSslSocket>
#include <QtNetwork>
#include <QMessageBox>
#include <QTextStream>
#include <QPushButton>
#include "user.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("Stop VK!");


    settings = new QFile("settings.txt");
    QString token;
    if(settings->open(QFile::ReadWrite)) {
        QTextStream set_stream(settings);
        set_stream>>token;
        settings->close();
    }
    if(token=="") {
        QUrlQuery url("https://oauth.vk.com/authorize?");
        url.addQueryItem("client_id","4534170");
        url.addQueryItem("scope","groups");
        url.addQueryItem("redirect_uri","http://oauth.vk.com/blank.html");
        url.addQueryItem("display","popup");
        url.addQueryItem("response_type","token");
        ui->webView->load(QUrl(url.toString()));
        ui->webView->show();
        connect(ui->webView,SIGNAL(urlChanged(QUrl)),this,SLOT(get_start(QUrl)));
    } else {
        show_delete_button();
    }
}

Dialog::~Dialog()
{
    delete ui;
    delete user;
    delete settings;
}

void Dialog::get_start(QUrl token_url) {
    if(token_url.toString()!="https://oauth.vk.com/authorize?&client_id=4534170&scope=groups&redirect_uri=http://oauth.vk.com/blank.html&display=popup&response_type=token") {
        QString url(token_url.toString());
        url.replace("#","&");
        QUrlQuery tok_url(url);
        QString token = tok_url.queryItemValue("access_token");
        QString expire = tok_url.queryItemValue("expires_in");
        QString id = tok_url.queryItemValue("user_id");
        user = new User(token,id,expire);
        if(settings->open(QFile::ReadWrite)) {
            QTextStream set_stream(settings);
            set_stream<<token;
            settings->close();
        }
        show_delete_button();
    }
}

void Dialog::show_delete_button() {
    QPushButton* delete_button = new QPushButton(this);
    delete_button->setText("Удалить аккаунт ВКонтакте");
    delete_button->setGeometry(QRect(100,100,200,50));
    ui->webView->hide();
    delete_button->show();
    this->resize(400,250);
}
