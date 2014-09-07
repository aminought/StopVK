#ifndef USER_H
#define USER_H

#include <QString>
#include <QVector>

class User {

public:
    QString token;
    QString id;
    QString expire;
public:
    User(QString _token,QString _id,QString _expire);
    User();
    void load_settings();
    void save_settings(QString token);
    bool is_token_valid();
};

#endif // USER_H
