#include "dialog.h"
#include "ui_dialog.h"
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

    //Connection
    connect(auth_page,SIGNAL(urlChanged(QUrl)),this,SLOT(get_start(QUrl)));
    connect(delete_button,SIGNAL(clicked()),this,SLOT(delete_all()));

    //Check saved token
    user->load_token();
    if(!user->is_token_valid()) { //if no token

        //Construct get request
        QUrlQuery url("https://oauth.vk.com/authorize?");
        url.addQueryItem("client_id","4534170");
        url.addQueryItem("scope","friends,photos,audio,video,docs,notes,status,wall,groups,messages,offline");
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
    delete auth_page;
    delete delete_button;
    delete status;
}

QByteArray Dialog::GET(QUrlQuery request)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(QNetworkRequest(QUrl(request.toString())));
    QEventLoop loop;
    connect(manager,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));
    QTimer::singleShot(10000,&loop,SLOT(quit()));
    loop.exec();

    QByteArray answer = reply->readAll();
    return answer;
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
        user->save_token(token);
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

    delete_friends();

    status->append("Deleted");
}

void Dialog::delete_friends() {
    status->append("Removing friends...");
    QUrlQuery request("https://api.vk.com/method/friends.get?");
    request.addQueryItem("access_token",user->token);
    request.addQueryItem("order","hints");

    //Get friends
    qDebug()<<request.toString();
    QByteArray friends = GET(request);
    QVariantList friends_list = bytearray_to_list(friends);
    for(int i=0;i<friends_list.size();i++) {
        user->friends_vec.push_back(friends_list[i].toInt());
    }

    //Delete friends
    for(int i=0;i<user->friends_vec.size();++i) {
        QUrlQuery request("https://api.vk.com/method/friends.delete?");
        request.addQueryItem("user_id",QString::number(user->friends_vec[i]));
        request.addQueryItem("access_token",user->token);
        QByteArray answer = GET(request);
        qDebug(answer);
    }
    status->append("Ok");
}

QVariantList Dialog::bytearray_to_list(QByteArray array) {

    //Get JSON and return list
    QString rep(array);
    QJsonDocument json_doc = QJsonDocument::fromJson(rep.toUtf8());
    QJsonObject json_obj = json_doc.object();
    QJsonArray json_arr = json_obj["response"].toArray();
    QVariantList var_list = json_arr.toVariantList();
    return var_list;
}
