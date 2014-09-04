#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QUrl>
#include <QFile>
#include "user.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    User* user;
    QFile* settings;

    void show_delete_button();

public slots:
    void get_start(QUrl url);

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
