/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *pbt_switch;
    QPushButton *pbt_search;
    QPushButton *pbt_searchnear;
    QPushButton *pbt_antfind;
    QLineEdit *le_search;
    QLineEdit *le_fp_begin;
    QLineEdit *le_fp_end;
    QLabel *lb_time;
    QSpinBox *sb_hour;
    QSpinBox *sb_min;
    QSpinBox *sb_sec;
    QSpinBox *sb_wday;
    QPushButton *pbt_settime;
    QLabel *lb_fp_begin;
    QLabel *lb_fp_end;
    QFrame *line;
    QFrame *line_2;
    QComboBox *cb_velocity;
    QPushButton *pbt_findpath;
    QFrame *line_3;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        pbt_switch = new QPushButton(centralwidget);
        pbt_switch->setObjectName(QStringLiteral("pbt_switch"));
        pbt_switch->setGeometry(QRect(410, 10, 80, 24));
        pbt_search = new QPushButton(centralwidget);
        pbt_search->setObjectName(QStringLiteral("pbt_search"));
        pbt_search->setGeometry(QRect(699, 13, 61, 21));
        pbt_searchnear = new QPushButton(centralwidget);
        pbt_searchnear->setObjectName(QStringLiteral("pbt_searchnear"));
        pbt_searchnear->setGeometry(QRect(620, 180, 141, 24));
        pbt_antfind = new QPushButton(centralwidget);
        pbt_antfind->setObjectName(QStringLiteral("pbt_antfind"));
        pbt_antfind->setGeometry(QRect(620, 230, 141, 24));
        le_search = new QLineEdit(centralwidget);
        le_search->setObjectName(QStringLiteral("le_search"));
        le_search->setGeometry(QRect(610, 10, 81, 24));
        le_fp_begin = new QLineEdit(centralwidget);
        le_fp_begin->setObjectName(QStringLiteral("le_fp_begin"));
        le_fp_begin->setGeometry(QRect(650, 60, 113, 24));
        le_fp_end = new QLineEdit(centralwidget);
        le_fp_end->setObjectName(QStringLiteral("le_fp_end"));
        le_fp_end->setGeometry(QRect(650, 100, 113, 24));
        lb_time = new QLabel(centralwidget);
        lb_time->setObjectName(QStringLiteral("lb_time"));
        lb_time->setGeometry(QRect(20, 10, 60, 20));
        sb_hour = new QSpinBox(centralwidget);
        sb_hour->setObjectName(QStringLiteral("sb_hour"));
        sb_hour->setGeometry(QRect(100, 10, 42, 25));
        sb_hour->setMaximum(23);
        sb_min = new QSpinBox(centralwidget);
        sb_min->setObjectName(QStringLiteral("sb_min"));
        sb_min->setGeometry(QRect(150, 10, 42, 25));
        sb_min->setMaximum(59);
        sb_sec = new QSpinBox(centralwidget);
        sb_sec->setObjectName(QStringLiteral("sb_sec"));
        sb_sec->setGeometry(QRect(200, 10, 42, 25));
        sb_sec->setMaximum(59);
        sb_wday = new QSpinBox(centralwidget);
        sb_wday->setObjectName(QStringLiteral("sb_wday"));
        sb_wday->setGeometry(QRect(250, 10, 42, 25));
        sb_wday->setMaximum(6);
        pbt_settime = new QPushButton(centralwidget);
        pbt_settime->setObjectName(QStringLiteral("pbt_settime"));
        pbt_settime->setGeometry(QRect(310, 10, 80, 24));
        lb_fp_begin = new QLabel(centralwidget);
        lb_fp_begin->setObjectName(QStringLiteral("lb_fp_begin"));
        lb_fp_begin->setGeometry(QRect(610, 60, 60, 16));
        lb_fp_end = new QLabel(centralwidget);
        lb_fp_end->setObjectName(QStringLiteral("lb_fp_end"));
        lb_fp_end->setGeometry(QRect(610, 100, 60, 16));
        line = new QFrame(centralwidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(610, 40, 161, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(centralwidget);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setGeometry(QRect(610, 160, 151, 20));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        cb_velocity = new QComboBox(centralwidget);
        cb_velocity->setObjectName(QStringLiteral("cb_velocity"));
        cb_velocity->setGeometry(QRect(610, 130, 61, 24));
        pbt_findpath = new QPushButton(centralwidget);
        pbt_findpath->setObjectName(QStringLiteral("pbt_findpath"));
        pbt_findpath->setGeometry(QRect(690, 130, 71, 24));
        line_3 = new QFrame(centralwidget);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setGeometry(QRect(610, 210, 151, 16));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        pbt_switch->setText(QApplication::translate("MainWindow", "\345\210\207\346\215\242\346\240\241\345\214\272", Q_NULLPTR));
        pbt_search->setText(QApplication::translate("MainWindow", "\346\220\234\347\264\242", Q_NULLPTR));
        pbt_searchnear->setText(QApplication::translate("MainWindow", "\346\220\234\347\264\242\351\231\204\350\277\221", Q_NULLPTR));
        pbt_antfind->setText(QApplication::translate("MainWindow", "\345\244\232\347\202\271\345\257\273\350\267\257", Q_NULLPTR));
        lb_time->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
        pbt_settime->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256\346\227\266\351\227\264", Q_NULLPTR));
        lb_fp_begin->setText(QApplication::translate("MainWindow", "\350\265\267\347\202\271", Q_NULLPTR));
        lb_fp_end->setText(QApplication::translate("MainWindow", "\347\273\210\347\202\271", Q_NULLPTR));
        cb_velocity->clear();
        cb_velocity->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "\346\255\245\350\241\214", Q_NULLPTR)
         << QApplication::translate("MainWindow", "\351\252\221\350\241\214", Q_NULLPTR)
        );
        cb_velocity->setCurrentText(QApplication::translate("MainWindow", "\346\255\245\350\241\214", Q_NULLPTR));
        pbt_findpath->setText(QApplication::translate("MainWindow", "\346\220\234\347\264\242\350\267\257\347\272\277", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
