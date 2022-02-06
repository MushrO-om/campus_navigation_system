#ifndef VERTEX_H
#define VERTEX_H

#include <QString>

class Vertex
{
public:
    Vertex();
    Vertex(int no, qreal x, qreal y, QString des, bool spot);

    void set_no(int n) {no = n;}
    void set_x(qreal x) {this->x = x;}
    void set_y(qreal y) {this->y = y;}
    void set_description(QString des) {description = des;}
    void set_isSpot(bool isSpot) {this->isSpot = isSpot;}

    int get_no() {return no;}
    qreal get_x() {return x;}
    qreal get_y() {return y;}
    QString get_description() {return description;}
    bool get_isSpot() {return isSpot;}

private:
    int no;
    qreal x;
    qreal y;
    QString description;
    bool isSpot;            //区分有用没有
};

#endif // VERTEX_H
