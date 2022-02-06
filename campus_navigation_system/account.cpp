#include "account.h"

using namespace std;

Account::Account(){}

Account::Account(QString id, QString pwd, QVector<Course> courses):
    id(id),password(pwd),courses(courses)
{

}

Course Account::gotoclass(Clock clock) const {
    for(auto course : courses) {
        if(course.get_end_time()>=clock && clock>=course.get_begin_time())
            return course;
    }
    Course tmp;
    tmp.set_classroom(-1);
    return tmp;
}
