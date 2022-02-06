#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QVector>
#include <QPair>
#include <QMap>
#include <QTimer>
#include <QString>
#include <QMainWindow>
#include <qdebug.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QMouseEvent>
#include <QMessageBox>
#include <fstream>
#include <math.h>

#include "log_search.h"
#include "log.h"
#include "current.h"
#include "edge.h"
#include "login.h"
#include "vertex.h"
#include "dijkstra.h"
#include "ant.h"
#include "constants.h"
#include "account.h"
#include "clock.h"

class Vertex;
class Edge;
class current;
class Log;
class Log_search;
class Login;
class Dijkstra;
class Ant;
class Account;
class Clock;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Account *online_account, QWidget *parent = nullptr);
    ~MainWindow();

    bool eventFilter(QObject *watched, QEvent *event);

    void initAction();
    void initToolBar();
    void initTimer();

    //地图的读写
    void readAdj();

    //顶点信息的读写
    void readVer();

    //读入逻辑名到物理名的映射
    void readL2P();

    //读入班车时刻表
    void readShuttleSchedule();

    void currentOutput();

    bool inShahe(int spot);
    bool inBenbu(int spot);
    bool inBetween(int spot);

    void searchNear();
    void searchSpot();
    void findPath();
    void antFind();
    void switchShahe();
    void switchBetween();
    void switchBenbu();
    void setTime();

    void drawPath(const QVector<Vertex*> &path, bool currentIn);
    void move();
    void clear();

    //用于开始导航时的current设置
    void setCurrent(const QVector<Vertex*> &path, qreal dis, qreal time, Priority pr);
    //用于把当前位置修改到指定顶点
    void setCurrentToVer(int no);
    void updateTime();
    int getFirstShuttleTime(qreal t);
    int getFirstBusTime(qreal t);

    void fuzzySearch(const QString &str);

protected:
    void mouseDoubleClickEvent (QMouseEvent *e);

private slots:
    void setVelocity(int x);
    void clock_slot();
    void cancel();
    void reenter();
    void choose_findpath_time();
    void choose_findpath_dis();
    void choose_searchnear(int i);
    void choose_search_fzs(int i);
    void choose_fp_begin_fzs(int i);
    void choose_fp_end_fzs(int i);
    void choose_ant_fzs(int i);
    void antfindNext();
    void antfindStart();
    void antfindStartDis();
    void antfindStartTime();
    void stopTime();
    void logSearch();
    void gotoClass();

    void search_fzs(const QString &str);
    void findpath_begin_fzs(const QString &str);
    void findpath_end_fzs(const QString &str);
    void antfind_fzs(const QString &str);

private:
    Ui::MainWindow *ui;

    QTimer *tmr_clock;

    int goal_sec;    //时间要加速的秒速，直到0

    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsPixmapItem *map_item;
    //当前点，此item并非绑定在scene中
    QGraphicsEllipseItem *current_point;
    //用于推进导航的进度，任何时候只能有一个导航在进行(该item是绑定在scene中的)
    QGraphicsPathItem *main_path_item;
    //根据current所在校区更改对应的painter_path，切换校区时切换path_item的painter_path
    //当变更导航时，先delete再new
    QPainterPath *shahe_painter_path;
    QPainterPath *benbu_painter_path;
    QPainterPath *between_painter_path;

    QPixmap shahe;
    QPixmap benbu;
    QPixmap between;

    int offset_x = 23;
    int offset_y = 72;

    QToolBar *toolbar;
    QAction *findpath;
    QAction *search_spot;
    QAction *search_near;
    QAction *ant_find;
    QAction *switch_shahe;
    QAction *switch_between;
    QAction *switch_benbu;
    QAction *log_search;
    QAction *set_time;
    QAction *set_velocity;
    QAction *go_to_class;

    QLabel *lb_fuzzysearch; //用于提示没有模糊搜索的匹配项
    QLabel *lb_current;
    QLabel *lb_time1;
    QLabel *lb_time2;
    QLabel *lb_search;
    QLabel *lb_searchnear;
    QLabel *lb_begin;
    QLabel *lb_end;
    QLabel *lb_antfind;
    QLabel *lb_velocity;
    QLabel *lb_wday;
    QLabel *lb_swc_shahe;
    QLabel *lb_swc_benbu;
    QLabel *lb_swc_between;
    QLabel *lb_class;
    QLineEdit *le_begin;
    QLineEdit *le_end;
    QLineEdit *le_search;       //输入地点
    QLineEdit *le_searchnear;   //输入半径
    QLineEdit *le_hour;
    QLineEdit *le_min;
    QLineEdit *le_sec;
    QLineEdit *le_wday;
    QComboBox *cb_velocity;

    QWidget *main_widget;
    QGridLayout *glayout;
    QPushButton *pbt_cancel;

    //用于显示搜索附近的所有结果（动态创建）
    QList<QPushButton*> pbtlist_searchnear;
    QList<QLabel*> lblist_searchnear;
    //存储list下标到该终点编号的映射
    QMap<int,int> list_to_no;

    //用于显示两点寻路的优先选择（动态创建）
    QLabel *lb_findpath_time;
    QLabel *lb_findpath_dis;
    QPushButton *pbt_findpath_time;
    QPushButton *pbt_findpath_dis;


    //用于显示双击的顶点信息
    QLabel *lb_click_des;

    //用于提示antfind的输入标签及lineedit(共用cancel按钮作为取消键)
    QLineEdit *le_antfind;
    QLabel *lb_antfind_hint;    //输入上方的描述性文字
    QPushButton *pbt_antfind_next;
    QPushButton *pbt_antfind_start;
    QLabel *lb_antfind_dis;
    QLabel *lb_antfind_time;
    QPushButton *pbt_antfind_dis;
    QPushButton *pbt_antfind_time;

    //用于显示模糊搜索的所有结果（动态创建）
    QList<QPushButton*> pbtlist_fuzzysearch;
    QList<QLabel*> lblist_fuzzysearch;
    QPushButton* pbt_fuzzysearch_reenter;

    //存储模糊搜索的结果数组(内容为顶点编号)
    QVector<int> fuzzysearch_vec;
    //-1表示清空lineedit重新输入
    int fuzzysearch_res;

    Log_search *log_dialog;                     //日志操作窗口
    Log *log;                                   //日志
    QVector<QMap<int,Edge*>> adj_map;           //地图，邻接表
    QVector<Vertex*> vertices;                  //保存所有顶点，对应编号
    Current *current;                           //当前状态
    QMap<QString,QVector<int>> logic2phy;       //逻辑名到物理名（编号）的映射
    Clock *clock;                               //模拟时钟
    QVector<Clock> shuttle_schedule;            //班车时刻表
    Account *online_account;                    //登陆账号
    Campus campus;      //指示当前mainwindow显示的校区
    qreal velocity;

    Dijkstra dij_searchnear;
    Dijkstra dij_findpath_time;
    Dijkstra dij_findpath_dis;

    //跨校区分段寻路
    Dijkstra dij_first_dis_shuttle;
    Dijkstra dij_first_time_shuttle;
    Dijkstra dij_first_dis_bus;
    Dijkstra dij_first_time_bus;
    Dijkstra dij_second_shuttle;
    Dijkstra dij_second_bus;
    Dijkstra dij_third_dis_shuttle;
    Dijkstra dij_third_time_shuttle;
    Dijkstra dij_third_dis_bus;
    Dijkstra dij_third_time_bus;

    //记录跨校区寻路的最终结果（步行距离短/时间短）
    Dijkstra dij_dis_first;
    Dijkstra dij_dis_second;
    Dijkstra dij_dis_third;
    Dijkstra dij_time_first;
    Dijkstra dij_time_second;
    Dijkstra dij_time_third;
    //标志，1表示要跨校区，0表示不用
    bool cross;
    //shuttle/bus
    qreal cross_dis_v;  //以距离优先跨校区时between的交通方式（速度）
    qreal cross_time_v; //以时间优先跨校区时between的交通方式（速度）
    int wait_sec;       //等车时间

    //设定好的中间线路
    QVector<Vertex*> s2b_bus_path;
    QVector<Vertex*> s2b_shuttle_path;
    QVector<Vertex*> b2s_bus_path;
    QVector<Vertex*> b2s_shuttle_path;

    //设定好的中间线路的总距离和时间
    qreal bus_total_dis;
    qreal shuttle_total_dis;
    qreal bus_total_time;
    qreal shuttle_total_time;

    //起点为current则-1，未设置则为-2，终点未设置也是-2（初始化都是-2）
    //每次reenter或输入为空或搜索后都要设为-2
    //只要起终点有一个是-2，就有错误输出
    int findpath_begin;
    int findpath_end;

    //存储搜索顶点的编号，未设置为-2（初始化为-2）不能为current
    int search_end;

    //用于多点寻路（不可以是当前位置）
    //用于不跨校区的多点寻路，同时记录跨校区多点寻路的最终结果（bus/shuttle）
    Ant ant_dis;
    Ant ant_time;

    //跨校区多点寻路
    Ant ant_dis_bus;
    Ant ant_time_bus;
    Ant ant_dis_shuttle;
    Ant ant_time_shuttle;

    QVector<Vertex*> ant_path_shahe_dis_bus;
    QVector<Vertex*> ant_path_shahe_time_bus;
    QVector<Vertex*> ant_path_shahe_dis_shuttle;
    QVector<Vertex*> ant_path_shahe_time_shuttle;
    QVector<Vertex*> ant_path_benbu_dis_bus;
    QVector<Vertex*> ant_path_benbu_time_bus;
    QVector<Vertex*> ant_path_benbu_dis_shuttle;
    QVector<Vertex*> ant_path_benbu_time_shuttle;

    //跨校区不同优先度的最终结果
    QVector<Vertex*> ant_path_dis;
    QVector<Vertex*> ant_path_time;
    QVector<Vertex*> ant_path_shahe_dis;
    QVector<Vertex*> ant_path_shahe_time;
    QVector<Vertex*> ant_path_benbu_dis;
    QVector<Vertex*> ant_path_benbu_time;
    qreal ant_shahe_d_dis;                  //shahe校区距离优先的路线距离
    qreal ant_shahe_t_dis;                  //shahe校区距离优先的路线时间
    qreal ant_shahe_d_time;                 //shahe校区时间优先的路线距离
    qreal ant_shahe_t_time;
    qreal ant_benbu_d_dis;
    qreal ant_benbu_t_dis;
    qreal ant_benbu_d_time;
    qreal ant_benbu_t_time;

    //存储时间和距离
    qreal ant_shahe_d_dis_bus;
    qreal ant_shahe_t_dis_bus;
    qreal ant_shahe_d_time_bus;
    qreal ant_shahe_t_time_bus;
    qreal ant_shahe_d_dis_shuttle;
    qreal ant_shahe_t_dis_shuttle;
    qreal ant_shahe_d_time_shuttle;
    qreal ant_shahe_t_time_shuttle;
    qreal ant_benbu_d_dis_bus;
    qreal ant_benbu_t_dis_bus;
    qreal ant_benbu_d_time_bus;
    qreal ant_benbu_t_time_bus;
    qreal ant_benbu_d_dis_shuttle;
    qreal ant_benbu_t_dis_shuttle;
    qreal ant_benbu_d_time_shuttle;
    qreal ant_benbu_t_time_shuttle;

    //最终导航的路线(中间路段借用dij_second)
    QVector<Vertex*> ant_path;
    QVector<Vertex*> ant_path_shahe;
    QVector<Vertex*> ant_path_benbu;
    qreal ant_shahe_d;
    qreal ant_shahe_t;
    qreal ant_benbu_d;
    qreal ant_benbu_t;

    int ant_spot;   //用于存储每次模糊搜索的结果，-2表示未设置
    QVector<int> ant_spots;
    QVector<int> ant_shahe_spots_bus;
    QVector<int> ant_shahe_spots_shuttle;
    QVector<int> ant_benbu_spots_bus;
    QVector<int> ant_benbu_spots_shuttle;
    QVector<int> ant_spots_bus;             //用于合并
    QVector<int> ant_spots_shuttle;

    bool have_shahe;
    bool have_benbu;
    bool is_anting;

    //根据spots生成的完全图(Dijkstra)
    QMap<int,QMap<int,Dijkstra>> abmap_dis;
    QMap<int,QMap<int,Dijkstra>> abmap_time;

    QMap<int,QMap<int,Dijkstra>> abmap_dis_bus;
    QMap<int,QMap<int,Dijkstra>> abmap_time_bus;
    QMap<int,QMap<int,Dijkstra>> abmap_dis_shuttle;
    QMap<int,QMap<int,Dijkstra>> abmap_time_shuttle;

    qreal before_v;
};

#endif // MAINWINDOW_H
