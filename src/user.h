#ifndef USER_H
#define USER_H

#include <QString>
#include <QVector>

class User {

public:
    QString token;
    QString id;
    QString expire;
    QVector<int> friends_vec;
public:
    User(QString _token,QString _id,QString _expire);
    User();
};

#endif // USER_H
