#ifndef USER_H
#define USER_H

#include <QString>

class User {

public:
    QString token;
    QString id;
    QString expire;
public:
    User(QString _token,QString _id,QString _expire);
    User();
};

#endif // USER_H
