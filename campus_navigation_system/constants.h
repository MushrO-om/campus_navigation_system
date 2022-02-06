#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QVector>

//文件路径
extern const char *ADJMAP_PATH;
extern const char *VERTICES_PATH;
extern const char *L2P_PATH;
extern const char *SHUTTLE_SCHEDULE_PATH;
extern const char *ACCOUNTS_PATH;
//日志文件
extern const char *LOG_PATH;

//日志文件各种操作
extern const char *SWC_SHAHE;
extern const char *SWC_BETWEEN;
extern const char *SWC_BENBU;
extern const char *FINDPATH;
extern const char *ANTFIND;
extern const char *SEARCH;
extern const char *SEARCH_NEAR;
extern const char *SET_TIME;
extern const char *SET_V;
extern const char *DOUBLE_CLICK;
extern const char *FUZZY_SEARCH;
extern const char *REENTER;
extern const char *GOTO_CLASS;

//bus的运营时间
extern const int BUS_BEGIN_SEC;
extern const int BUS_END_SEC;
extern const int BUS_INTERVAL_SEC;

//current的速度
extern const qreal STOP;
extern const qreal FOOT;
extern const qreal BIKE;
extern const qreal SHUTTLE;
extern const qreal BUS;

extern const int GRID_ROWS;

//沙河、本部中节点的最大序号
extern const int SHAHE_MAX_NO;
extern const int BETWEEN_MIN_NO;
extern const int BETWEEN_MAX_NO;
extern const int BENBU_MAX_NO;

//两个校区的班车点、公交点的编号
//bus点就是校区的校门点
extern const int SHAHE_BUS_VER_NO;
extern const int BENBU_BUS_VER_NO;
extern const int SHAHE_SHUTTLE_VER_NO;
extern const int BENBU_SHUTTLE_VER_NO;

//中间线路的编号表示形式
//中间段按设定好的路线走即可
extern const QVector<int> S2B_BUS_PATH_NO;
extern const QVector<int> S2B_SHUTTLE_PATH_NO;

enum Priority {
    Pr_Distance,
    Pr_Time
};

enum Campus {
    Cp_Shahe,
    Cp_Between,
    Cp_Benbu
};

#endif // CONSTANTS_H


