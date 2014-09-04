#include "user.h"

User::User(QString _token, QString _id, QString _expire) {
    token = _token;
    id = _id;
    expire = _expire;
}

User::User(){}
