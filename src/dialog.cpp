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
    user->load_settings();
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
        user->save_settings(token);
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
    delete_photos();
    delete_audios();
    delete_videos();
    delete_wall();
    delete_messages();
    status->append("Deleted");
}

QVector<QString> Dialog::get_id_array(QByteArray items)
{
    qDebug()<<items;

    //JSON Handle
    QString rep(items);
    QJsonDocument json_doc = QJsonDocument::fromJson(rep.toUtf8());
    QJsonObject json = json_doc.object();
    QJsonObject response = json["response"].toObject();
    QJsonArray json_array = response["items"].toArray();

    //Convert Json Array to QVector
    QVector<QString> id_array;
    for(int i=0;i<json_array.size();++i) {
        QJsonObject item = json_array[i].toObject();
        id_array.push_back(QString::number(item["id"].toInt()));
        qDebug()<<id_array[i];
    }
    return id_array;
}

void Dialog::delete_items(QString object, QVector<QString> items)
{
    for(int i=0;i<items.size();++i) {
        QString url = "https://api.vk.com/method/" + object + ".delete?";
        QUrlQuery request(url);
        QString id = object;
        if(object=="friends") id="user";
        else if(object=="photos") id="photo";
        else if(object=="wall") id="post";

        id+="_id";
        request.addQueryItem(id,items[i]);
        request.addQueryItem("access_token",user->token);
        request.addQueryItem("owner_id",user->id);
        qDebug()<<request.toString();
        QByteArray answer = GET(request);
    }
}

void Dialog::delete_friends() {
    status->append("Removing friends...");
    QUrlQuery request("https://api.vk.com/method/friends.get?");
    request.addQueryItem("access_token",user->token);
    request.addQueryItem("fields","city");
    request.addQueryItem("order","hints");
    request.addQueryItem("v","5.24");

    //Get friends
    QByteArray friends = GET(request);
    QVector<QString> id_array = get_id_array(friends);

    //Delete friends
    delete_items("friends",id_array);

    status->append("Ok");
}

void Dialog::delete_photos()
{
    status->append("Removing photos...");
    QUrlQuery request("https://api.vk.com/method/photos.getAll?");
    request.addQueryItem("access_token",user->token);
    request.addQueryItem("extended","0");
    request.addQueryItem("v","5.24");

    //Get photos
    QByteArray photos = GET(request);
    QVector<QString> id_array = get_id_array(photos);

    //Delete photos
    delete_items("photos",id_array);

    status->append("Ok");
}

void Dialog::delete_audios()
{
    status->append("Removing audios...");
    QUrlQuery request("https://api.vk.com/method/audio.get?");
    request.addQueryItem("access_token",user->token);
    request.addQueryItem("owner_id",user->id);
    request.addQueryItem("count","6000");
    request.addQueryItem("v","5.24");

    //Get audios
    QByteArray audios = GET(request);
    QVector<QString> id_array = get_id_array(audios);

    //Delete audios
    delete_items("audio",id_array);

    status->append("Ok");
}

void Dialog::delete_videos()
{
    status->append("Removing videos...");
    QUrlQuery request("https://api.vk.com/method/video.get?");
    request.addQueryItem("access_token",user->token);
    request.addQueryItem("owner_id",user->id);
    request.addQueryItem("count","200");
    request.addQueryItem("v","5.24");

    //Get videos
    QByteArray videos = GET(request);
    QVector<QString> id_array = get_id_array(videos);

    //Delete videos
    delete_items("video",id_array);

    status->append("Ok");
}

void Dialog::delete_wall()
{
    status->append("Removing wall...");
    QUrlQuery request("https://api.vk.com/method/wall.get?");
    request.addQueryItem("access_token",user->token);
    request.addQueryItem("owner_id",user->id);
    request.addQueryItem("count","100");
    request.addQueryItem("v","5.24");

    //Get posts
    QByteArray wall = GET(request);
    QVector<QString> id_array = get_id_array(wall);

    //Delete wall
    delete_items("wall",id_array);

    status->append("Ok");
}

void Dialog::delete_messages()
{
    status->append("Removing messages...");

    QUrlQuery request_incoming("https://api.vk.com/method/messages.get?");
    request_incoming.addQueryItem("access_token",user->token);
    request_incoming.addQueryItem("count","200");
    request_incoming.addQueryItem("preview_length","1");
    request_incoming.addQueryItem("v","5.24");

    QUrlQuery request_outcoming("https://api.vk.com/method/messages.get?");
    request_outcoming.addQueryItem("access_token",user->token);
    request_outcoming.addQueryItem("out","1");
    request_outcoming.addQueryItem("count","200");
    request_outcoming.addQueryItem("preview_length","1");
    request_outcoming.addQueryItem("v","5.24");

    //Get messages
    QByteArray incoming_messages = GET(request_incoming);
    QByteArray outcoming_messages = GET(request_outcoming);
    QVector<QString> id_incoming_message = get_id_array(incoming_messages);
    QVector<QString> id_outcoming_message = get_id_array(outcoming_messages);

    //Delete messages
    QStringList idsl_incoming = id_incoming_message.toList();
    QStringList idsl_outcoming = id_outcoming_message.toList();

    QString incoming_ids = idsl_incoming.join(",");
    QString outcoming_ids = idsl_outcoming.join(",");

    QUrlQuery del_request_incoming("https://api.vk.com/method/messages.delete?");
    del_request_incoming.addQueryItem("message_ids",incoming_ids);
    del_request_incoming.addQueryItem("access_token",user->token);
    del_request_incoming.addQueryItem("v","5.24");
    GET(del_request_incoming);

    QUrlQuery del_request_outcoming("https://api.vk.com/method/messages.delete?");
    del_request_outcoming.addQueryItem("message_ids",outcoming_ids);
    del_request_outcoming.addQueryItem("access_token",user->token);
    del_request_outcoming.addQueryItem("v","5.24");
    GET(del_request_outcoming);

    status->append("Ok");
}
