#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QMap>
#include <QMessageBox>
#include <fstream>
#include <qdebug.h>
#include <QString>

#include "account.h"

class Account;

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

    //读入用户账户及课表信息
    void readAccounts();

    bool checkLogin(const QString &id, const QString &pwd);

    Account* get_online_account() {return online_account;}

private slots:
    void on_pbt_login_clicked();

    void on_pbt_exit_clicked();

private:
    Ui::Login *ui;
    QMap<QString,Account*> accounts;            //用户账户(id到账户的映射）
    Account *online_account;
};

#endif // LOGIN_H
