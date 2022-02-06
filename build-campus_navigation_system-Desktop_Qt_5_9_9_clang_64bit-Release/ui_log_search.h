/********************************************************************************
** Form generated from reading UI file 'log_search.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOG_SEARCH_H
#define UI_LOG_SEARCH_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_Log_search
{
public:
    QComboBox *comboBox;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *le_begin_w;
    QLineEdit *le_begin_h;
    QLineEdit *le_begin_m;
    QLineEdit *le_end_m;
    QLineEdit *le_end_h;
    QLineEdit *le_end_w;
    QLabel *label_3;
    QLabel *label_4;
    QTextBrowser *textBrowser;
    QPushButton *pushButton;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;

    void setupUi(QDialog *Log_search)
    {
        if (Log_search->objectName().isEmpty())
            Log_search->setObjectName(QStringLiteral("Log_search"));
        Log_search->resize(719, 540);
        comboBox = new QComboBox(Log_search);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(160, 40, 151, 31));
        label = new QLabel(Log_search);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(40, 40, 101, 31));
        label_2 = new QLabel(Log_search);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(40, 90, 101, 31));
        le_begin_w = new QLineEdit(Log_search);
        le_begin_w->setObjectName(QStringLiteral("le_begin_w"));
        le_begin_w->setGeometry(QRect(290, 100, 51, 21));
        le_begin_h = new QLineEdit(Log_search);
        le_begin_h->setObjectName(QStringLiteral("le_begin_h"));
        le_begin_h->setGeometry(QRect(370, 100, 51, 21));
        le_begin_m = new QLineEdit(Log_search);
        le_begin_m->setObjectName(QStringLiteral("le_begin_m"));
        le_begin_m->setGeometry(QRect(450, 100, 51, 21));
        le_end_m = new QLineEdit(Log_search);
        le_end_m->setObjectName(QStringLiteral("le_end_m"));
        le_end_m->setGeometry(QRect(450, 140, 51, 21));
        le_end_h = new QLineEdit(Log_search);
        le_end_h->setObjectName(QStringLiteral("le_end_h"));
        le_end_h->setGeometry(QRect(370, 140, 51, 21));
        le_end_w = new QLineEdit(Log_search);
        le_end_w->setObjectName(QStringLiteral("le_end_w"));
        le_end_w->setGeometry(QRect(290, 140, 51, 21));
        label_3 = new QLabel(Log_search);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(160, 100, 60, 16));
        label_4 = new QLabel(Log_search);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(160, 140, 60, 16));
        textBrowser = new QTextBrowser(Log_search);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(40, 190, 641, 331));
        pushButton = new QPushButton(Log_search);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(550, 40, 131, 121));
        label_5 = new QLabel(Log_search);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(250, 100, 31, 21));
        label_6 = new QLabel(Log_search);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(250, 140, 31, 21));
        label_7 = new QLabel(Log_search);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(430, 100, 21, 21));
        label_8 = new QLabel(Log_search);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(430, 140, 21, 21));
        label_9 = new QLabel(Log_search);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(510, 100, 21, 21));
        label_10 = new QLabel(Log_search);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(510, 140, 21, 21));

        retranslateUi(Log_search);

        QMetaObject::connectSlotsByName(Log_search);
    } // setupUi

    void retranslateUi(QDialog *Log_search)
    {
        Log_search->setWindowTitle(QApplication::translate("Log_search", "Dialog", Q_NULLPTR));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("Log_search", "\345\205\250\351\203\250", Q_NULLPTR)
         << QApplication::translate("Log_search", "\346\220\234\347\264\242\345\234\260\347\202\271", Q_NULLPTR)
         << QApplication::translate("Log_search", "\346\220\234\347\264\242\351\231\204\350\277\221", Q_NULLPTR)
         << QApplication::translate("Log_search", "\344\270\244\347\202\271\345\257\273\350\267\257", Q_NULLPTR)
         << QApplication::translate("Log_search", "\345\244\232\347\202\271\345\257\273\350\267\257", Q_NULLPTR)
         << QApplication::translate("Log_search", "\345\210\207\346\215\242\346\262\231\346\262\263\345\234\260\345\233\276", Q_NULLPTR)
         << QApplication::translate("Log_search", "\345\210\207\346\215\242\350\267\250\346\240\241\345\214\272\345\234\260\345\233\276", Q_NULLPTR)
         << QApplication::translate("Log_search", "\345\210\207\346\215\242\346\234\254\351\203\250\345\234\260\345\233\276", Q_NULLPTR)
         << QApplication::translate("Log_search", "\350\256\276\347\275\256\346\227\266\351\227\264", Q_NULLPTR)
         << QApplication::translate("Log_search", "\350\256\276\347\275\256\351\200\237\345\272\246", Q_NULLPTR)
         << QApplication::translate("Log_search", "\345\211\215\345\276\200\346\225\231\345\256\244", Q_NULLPTR)
         << QApplication::translate("Log_search", "\346\237\245\347\234\213\345\234\260\347\202\271", Q_NULLPTR)
         << QApplication::translate("Log_search", "\346\250\241\347\263\212\346\220\234\347\264\242", Q_NULLPTR)
         << QString()
        );
        label->setText(QApplication::translate("Log_search", "\346\240\271\346\215\256\346\223\215\344\275\234\346\237\245\350\257\242", Q_NULLPTR));
        label_2->setText(QApplication::translate("Log_search", "\346\240\271\346\215\256\346\227\266\351\227\264\346\237\245\350\257\242", Q_NULLPTR));
        label_3->setText(QApplication::translate("Log_search", "\350\265\267\345\247\213\346\227\266\351\227\264", Q_NULLPTR));
        label_4->setText(QApplication::translate("Log_search", "\347\273\210\346\255\242\346\227\266\351\227\264", Q_NULLPTR));
        pushButton->setText(QApplication::translate("Log_search", "\346\237\245\350\257\242", Q_NULLPTR));
        label_5->setText(QApplication::translate("Log_search", "\346\230\237\346\234\237", Q_NULLPTR));
        label_6->setText(QApplication::translate("Log_search", "\346\230\237\346\234\237", Q_NULLPTR));
        label_7->setText(QApplication::translate("Log_search", "\346\227\266", Q_NULLPTR));
        label_8->setText(QApplication::translate("Log_search", "\346\227\266", Q_NULLPTR));
        label_9->setText(QApplication::translate("Log_search", "\345\210\206", Q_NULLPTR));
        label_10->setText(QApplication::translate("Log_search", "\345\210\206", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Log_search: public Ui_Log_search {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOG_SEARCH_H
