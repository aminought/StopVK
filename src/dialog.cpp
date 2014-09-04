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

    //Initialization
    user = new User();
    delete_button = new QPushButton(this);
    auth_page = new QWebView(this);

    //Connection
    connect(auth_page,SIGNAL(urlChanged(QUrl)),this,SLOT(get_start(QUrl)));
    connect(delete_button,SIGNAL(clicked()),this,SLOT(delete_all()));

    //Check saved token
    settings = new QFile("settings.txt");
    QString token;
    if(settings->open(QFile::ReadWrite)) {
        QTextStream set_stream(settings);
        set_stream>>token;
        settings->close();
    }
    if(token=="") { //if no token
        //Construct get request
        QUrlQuery url("https://oauth.vk.com/authorize?");
        url.addQueryItem("client_id","4534170");
        url.addQueryItem("scope","friends,photos,audio,video,docs,notes,status,wall,groups,messages");
        url.addQueryItem("redirect_uri","http://oauth.vk.com/blank.html");
        url.addQueryItem("display","page");
        url.addQueryItem("response_type","token");
        //Load webView with authorization window
        auth_page->load(QUrl(url.toString()));
        auth_page->setGeometry(0,0,800,600);
        this->setFixedSize(800,600);
        auth_page->show();
    } else { //if token exists
        show_delete_button();
    }
}

Dialog::~Dialog()
{
    delete ui;
    delete user;
    delete settings;
    delete auth_page;
    delete delete_button;
}

void Dialog::get_start(QUrl token_url) {
    //Get http response
    QString url(token_url.toString());
    url.replace("#","&");
    QUrlQuery tok_url(url);
    QString token = tok_url.queryItemValue("access_token");
    if(token!="") { //if autorization is valid
        QString expire = tok_url.queryItemValue("expires_in");
        QString id = tok_url.queryItemValue("user_id");
        //Fill user info
        user->token = token;
        user->id = id;
        user->expire = expire;
        //Save token in file
        if(settings->open(QFile::ReadWrite)) {
            QTextStream set_stream(settings);
            set_stream<<token;
            settings->close();
        }
        show_delete_button();
    }
}

void Dialog::show_delete_button() {
    delete_button->setText("Delete VK account");
    delete_button->setGeometry(QRect(10,10,200,50));
    auth_page->hide();
    delete_button->setCursor(QCursor(Qt::PointingHandCursor));
    delete_button->show();
    this->setFixedSize(220,400);
}

void Dialog::delete_all() {
    qDebug()<<"Deleted";
}
