#ifndef COURSE_H
#define COURSE_H

#include <QString>
#include <QVector>

#include "vertex.h"
#include "clock.h"

class Vertex;
class Clock;

class Course
{
public:
    Course();
    Course(QString name, Clock begin, Clock end, int room):course_name(name),begin_time(begin),end_time(end),classroom(room){}

    QString get_name(){return course_name;}
    Clock get_begin_time(){return begin_time;}
    Clock get_end_time(){return end_time;}
    int get_classroom(){return classroom;}

    void set_name(QString name) {course_name = name;}
    void set_begin_time(Clock time) {begin_time = time;}
    void set_end_time(Clock time) {end_time = time;}
    void set_classroom(int classroom) {this->classroom = classroom;}

private:
    QString course_name;
    Clock begin_time;
    Clock end_time;
    int classroom;          //教室编号对应vertex
};

#endif // COURSE_H
