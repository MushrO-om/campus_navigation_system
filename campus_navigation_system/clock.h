#ifndef CLOCK_H
#define CLOCK_H

#include <QString>
#include <ctime>

class Clock
{
public:
    Clock();
    Clock(int sec,int min,int hour,int wday):sec(sec),min(min),hour(hour),wday(wday){}

    int get_sec() {return sec;}
    int get_min() {return min;}
    int get_hour() {return hour;}
    int get_wday() {return wday;}

    //用于手动更改时间
    void set_sec(int s) {sec = s;}
    void set_min(int m) {min = m;}
    void set_hour(int h) {hour = h;}
    void set_wday(int d) {wday = d;}

    QString get_clock_str();

    //时钟更新，+1s
    void update_clock();

    int difSec(const Clock &clock);
    bool operator>=(const Clock &clock);
    Clock operator-(int h);

private:
    int sec;
    int min;
    int hour;
    int wday;    //星期几,0~6，0表示星期日
};

#endif // CLOCK_H
