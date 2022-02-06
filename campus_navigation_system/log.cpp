
#include "log.h"

using namespace std;

Log::Log()
{
    readLog();
}

Log::~Log() {
    writeLog();
}

void Log::readLog() {
    ifstream fin;
    fin.open(LOG_PATH,ios::in);
    if(!fin.is_open()) {
        qDebug()<<"file open error"<<endl;
        exit(1);
    }
    int wday,hour,min,sec;
    string op,inf;
    while(fin>>wday>>hour>>min>>sec>>op>>inf) {
        Clock tmp(sec,min,hour,wday);
        addItem(tmp,QString::fromStdString(op),QString::fromStdString(inf));
    }
    fin.close();
    qDebug()<<"log file load successfully"<<endl;
}

void Log::writeLog() {
    ofstream fout;
    fout.open(LOG_PATH,ios::out|ios::trunc);
    if(!fout.is_open()) {
        qDebug()<<"file open error"<<endl;
        exit(1);
    }
    for(auto it : log_list) {
        fout<<it.clk.get_wday()<<" "<<it.clk.get_hour()<<" "<<it.clk.get_min()<<" "<<
              it.clk.get_sec()<<" "<<it.ope.toStdString()<<" "<<it.info.toStdString()<<endl;
    }
    fout.close();
    qDebug()<<"log file write successfully"<<endl;
}

void Log::addItem(Clock clk, QString op, QString inf) {
    item tmp = {clk,op,inf};
    log_list.append(tmp);
}

void Log::popItem() {
    log_list.pop_back();
}

QVector<QString> Log::search(Clock clk_high, Clock clk_low, const QString &ope) const {
    QVector<QString> res;
    for(auto it : log_list) {
        if(it.clk>=clk_low && clk_high>=it.clk && (ope==it.ope || ope=="全部")) {
            res.append(it.clk.get_clock_str()+"\t"+it.ope+"\t\t"+it.info);
        }
    }
    return res;
}
