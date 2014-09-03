#ifndef USER_H
#define USER_H

#include <QString>

class User {

public:
    QString token;
    QString id;
public:
    User(QString _token,QString _id);
    User();
};

#endif // USER_H
