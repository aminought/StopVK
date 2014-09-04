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
    delete_button->hide();
    auth_page = new QWebView(this);
    status = new QTextBrowser(this);
    status->hide();
    net_access_get_friends = new QNetworkAccessManager(this);

    //Connection
    connect(auth_page,SIGNAL(urlChanged(QUrl)),this,SLOT(get_start(QUrl)));
    connect(delete_button,SIGNAL(clicked()),this,SLOT(delete_all()));
    connect(net_access_get_friends,SIGNAL(finished(QNetworkReply*)),this,SLOT(get_friends(QNetworkReply*)));

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
        show_delete_dialog();
    }
}

Dialog::~Dialog()
{
    delete ui;
    delete user;
    delete settings;
    delete auth_page;
    delete delete_button;
    delete net_access_get_friends;
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
        show_delete_dialog();
    }
}

void Dialog::show_delete_dialog() {
    delete_button->setText("Delete VK account");
    delete_button->setGeometry(QRect(10,10,200,50));
    auth_page->hide();
    delete_button->setCursor(QCursor(Qt::PointingHandCursor));
    delete_button->show();
    this->setFixedSize(220,400);
    status->setGeometry(QRect(10,70,200,320));
    status->show();
}

void Dialog::delete_all() {
    status->append("Removing began...");

    QEventLoop* wait_delete_friends = new QEventLoop(this);
    delete_friends();
    wait_delete_friends->exec();
    delete wait_delete_friends;

    status->append("Deleted");
}

void Dialog::delete_friends() {
    status->append("Removing friends...");
    QUrlQuery* request = new QUrlQuery("https://api.vk.com/method/friends.get?");
    request->addQueryItem("user_id",user->id);
    request->addQueryItem("order","hints");
    //Get friends
    QEventLoop* wait_get_friends = new QEventLoop(this);
    net_access_get_friends->get(QNetworkRequest(QUrl(request->toString())));
    wait_get_friends->exec();
    delete wait_get_friends;

    status->append("Ok");
}

void Dialog::get_friends(QNetworkReply* reply) {
    QVariantList friends_list = reply_to_list(reply);
    for(int i=0;i<friends_list.size();++i) {
        user->friends_vec.push_back(friends_list[i].toInt());
    }
}

QVariantList Dialog::reply_to_list(QNetworkReply *net_reply) {
    //Get JSON and return list
    QString rep = (QString)net_reply->readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(rep.toUtf8());
    QJsonObject json_obj = json_doc.object();
    QJsonArray json_arr = json_obj["response"].toArray();
    QVariantList var_list = json_arr.toVariantList();
    return var_list;
}
