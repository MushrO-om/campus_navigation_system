#include "clock.h"

Clock::Clock() {
    //初始化为系统当前时间
    time_t now = time(0);
    tm *ltm = localtime(&now);
    sec = ltm->tm_sec;
    min = ltm->tm_min;
    hour = ltm->tm_hour;
    wday = ltm->tm_wday;  //tm_wday范围是0～6
}

//更新时间
void Clock::update_clock() {
    sec++;
    if(sec==60) {
        sec = 0;
        min++;
        if(min==60) {
            min = 0;
            hour++;
            if(hour==24) {
                hour = 0;
                wday++;
                if(wday==7) wday = 0;
            }
        }
    }
}

bool Clock::operator>=(const Clock &clock) {
    if(this->wday>clock.wday) return true;
    else if(this->get_wday()==clock.wday) {
        if(this->get_hour()>clock.hour) return true;
        else if(this->get_hour()==clock.hour) {
            if(this->get_min()>clock.min) return true;
            else if(this->get_min()==clock.min) {
                if(this->get_sec()>=clock.sec) return true;
            }
        }
    }
    return false;
}

Clock Clock::operator-(int ho) {
    int w,h;
    h = this->hour-ho;
    w = this->wday;
    if(h<0) {
        h = h%24;
        w = this->wday-1;
        //不能减到上一周
        if(w<0) {
            w = 0;
            h = 0;
        }
    }
    Clock res(0,0,h,w);
    return res;
}

//this-clock的秒数
int Clock::difSec(const Clock &clock) {
    int total_sec = wday*24*3600 + hour*3600 + min*60 +sec;
    int clock_total_sec = clock.wday*24*3600 + clock.hour*3600 + clock.min*60 + clock.sec;
    return total_sec-clock_total_sec;
}

QString Clock::get_clock_str() {
    return "星期"+QString::number(wday)+":"+QString::number(hour)+":"+QString::number(min)+":"+QString::number(sec);
}
