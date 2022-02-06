/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Login
{
public:
    QLineEdit *le_id;
    QLabel *label_id;
    QLabel *label_pwd;
    QPushButton *pbt_login;
    QPushButton *pbt_exit;
    QLineEdit *le_pwd;

    void setupUi(QDialog *Login)
    {
        if (Login->objectName().isEmpty())
            Login->setObjectName(QStringLiteral("Login"));
        Login->resize(647, 439);
        le_id = new QLineEdit(Login);
        le_id->setObjectName(QStringLiteral("le_id"));
        le_id->setGeometry(QRect(230, 120, 321, 41));
        label_id = new QLabel(Login);
        label_id->setObjectName(QStringLiteral("label_id"));
        label_id->setGeometry(QRect(90, 120, 111, 41));
        label_id->setTextFormat(Qt::AutoText);
        label_id->setAlignment(Qt::AlignCenter);
        label_pwd = new QLabel(Login);
        label_pwd->setObjectName(QStringLiteral("label_pwd"));
        label_pwd->setGeometry(QRect(90, 200, 111, 41));
        label_pwd->setAlignment(Qt::AlignCenter);
        pbt_login = new QPushButton(Login);
        pbt_login->setObjectName(QStringLiteral("pbt_login"));
        pbt_login->setGeometry(QRect(130, 300, 161, 41));
        pbt_exit = new QPushButton(Login);
        pbt_exit->setObjectName(QStringLiteral("pbt_exit"));
        pbt_exit->setGeometry(QRect(360, 300, 151, 41));
        le_pwd = new QLineEdit(Login);
        le_pwd->setObjectName(QStringLiteral("le_pwd"));
        le_pwd->setGeometry(QRect(230, 200, 321, 41));
        le_pwd->setEchoMode(QLineEdit::Password);

        retranslateUi(Login);
        QObject::connect(pbt_exit, SIGNAL(clicked()), Login, SLOT(reject()));

        QMetaObject::connectSlotsByName(Login);
    } // setupUi

    void retranslateUi(QDialog *Login)
    {
        Login->setWindowTitle(QApplication::translate("Login", "Dialog", Q_NULLPTR));
        le_id->setPlaceholderText(QApplication::translate("Login", "\350\257\267\345\234\250\346\255\244\345\244\204\350\276\223\345\205\245\350\264\246\345\217\267", Q_NULLPTR));
        label_id->setText(QApplication::translate("Login", "\350\264\246\345\217\267", Q_NULLPTR));
        label_pwd->setText(QApplication::translate("Login", "\345\257\206\347\240\201", Q_NULLPTR));
        pbt_login->setText(QApplication::translate("Login", "\347\231\273\351\231\206", Q_NULLPTR));
        pbt_exit->setText(QApplication::translate("Login", "\351\200\200\345\207\272", Q_NULLPTR));
        le_pwd->setPlaceholderText(QApplication::translate("Login", "\350\257\267\345\234\250\346\255\244\345\244\204\350\276\223\345\205\245\345\257\206\347\240\201", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Login: public Ui_Login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
