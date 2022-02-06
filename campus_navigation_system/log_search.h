#ifndef LOG_SEARCH_H
#define LOG_SEARCH_H

#include <QDialog>
#include <QString>
#include "clock.h"
#include "log.h"

class Log;
class Clock;

namespace Ui {
class Log_search;
}

class Log_search : public QDialog
{
    Q_OBJECT

public:
    explicit Log_search(QWidget *parent = nullptr);
    explicit Log_search(Log* log, QWidget *parent = nullptr);

    void sendData(Clock cur);

    ~Log_search();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Log_search *ui;

    //关闭窗口该变量不会被释放
    Log* log;
    Clock begin;
    Clock end;
    QString ope;

    QVector<QString> res_list;
};

#endif // LOG_SEARCH_H
