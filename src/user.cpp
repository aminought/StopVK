#include "user.h"
#include <QFile>
#include <QDataStream>

User::User(QString _token, QString _id, QString _expire) {
    token = _token;
    id = _id;
    expire = _expire;
}

User::User(){
    token="No token";
}

void User::load_token()
{
    QFile settings("settings.api");
    if(settings.open(QFile::ReadWrite)) {
        QDataStream set_stream(&settings);
        set_stream>>token;
        qDebug("Настройки загружены");
    } else {
        qDebug("Невозможно открыть файл для загрузки настроек");
    }

    settings.close();
}

void User::save_token(QString token)
{
    QFile settings("settings.api");
    if(settings.open(QFile::ReadWrite)) {
        QDataStream set_stream(&settings);
        set_stream<<token;
        qDebug("Настройки сохранены");
    } else {
        qDebug("Невозможно открыть файл для сохранения настроек");
    }

    settings.close();
}

bool User::is_token_valid()
{
    return token!="No token";
}
