#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QVector>
#include <fstream>

#include "constants.h"
#include "course.h"

class Course;

class Account
{
public:
    Account();
    Account(QString id, QString pwd, QVector<Course> courses);

    void set_id(const QString &id) {this->id = id;}
    void set_password(const QString &pwd) {password = pwd;}
    void set_courses(const QVector<Course> &courses) {this->courses = courses;}

    QString get_id() const {return id;}
    QString get_password() const {return password;}
    QVector<Course> get_courses() const {return courses;}

    //对应当前时间找到教室，返回顶点编号，没课在图书馆，晚上在宿舍
    Course gotoclass(Clock clock) const;

private:
    QString id;
    QString password;
    QVector<Course> courses;    //要按照课程结束时间升序排列
};

#endif // ACCOUNT_H
