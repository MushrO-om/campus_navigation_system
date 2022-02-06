#ifndef LOG_H
#define LOG_H

#include <QVector>
#include <QString>
#include <fstream>
#include <qdebug.h>
#include <string>

#include "constants.h"
#include "clock.h"

class Clock;

class Log
{
public:
    Log();
    ~Log();

    void readLog();
    void writeLog();

    void addItem(Clock clk, QString op, QString inf);
    void popItem();

    //搜索一定时间范围内的所有操作
    QVector<QString> search(Clock clk_high, Clock clk_low, const QString &ope) const;

private:
    typedef struct ITEM {
        Clock clk;
        QString ope;
        QString info;
    } item;

    QVector<item> log_list;
};

#endif // LOG_H
