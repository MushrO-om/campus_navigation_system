
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(Account *online_account, QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), goal_sec(0), online_account(online_account), campus(Cp_Shahe), velocity(0), cross(false),
    wait_sec(0), bus_total_dis(0), shuttle_total_dis(0), bus_total_time(0), shuttle_total_time(0),
    findpath_begin(-2), findpath_end(-2), search_end(-2), ant_spot(-2), is_anting(false), before_v(0)
{
    ui->setupUi(this);
    readAdj();
    readL2P();
    readVer();
    readShuttleSchedule();

    current = new Current(adj_map,vertices);
    clock = new Clock;
    log = new Log;
    log_dialog = new Log_search(log);
    qDebug()<<"Log file init down"<<endl;

    initTimer();
    initAction();
    initToolBar();

    setWindowTitle("北邮校园导航系统");
    scene = new QGraphicsScene;
    scene->setSceneRect(0,0,990,640);
    shahe.load(":/images/shahe");
    benbu.load(":images/benbu");
    between.load(":/images/between");
    view = new QGraphicsView(scene);
//    view->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    map_item = new QGraphicsPixmapItem(shahe);
    scene->addItem(map_item);
    view->setFixedSize (1010, 650);
    qDebug()<<"images init done"<<endl;

    main_widget = new QWidget;
    glayout = new QGridLayout(main_widget);
    glayout->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    setCentralWidget(main_widget);
    glayout->addWidget(view,0,0,GRID_ROWS,1);
    lb_current = new QLabel;
    glayout->addWidget(lb_current,GRID_ROWS+1,0);
    qDebug()<<"glayout init done"<<endl;

    //初始化当前圆点
    current_point = new QGraphicsEllipseItem(QRect(0,0,10,10));
    current_point->setPen(Qt::NoPen);
    current_point->setBrush(Qt::cyan);
    current_point->setFlag(QGraphicsItem::ItemIsPanel);
    scene->addItem(current_point);
    current_point->setPos(current->get_x(), current->get_y());
    qDebug()<<"current point init down"<<endl;

    //初始化导航pathItem
    main_path_item = new QGraphicsPathItem;
    shahe_painter_path = new QPainterPath;
    benbu_painter_path = new QPainterPath;
    between_painter_path = new QPainterPath;
    QPen pen;
    pen.setWidth (4);
    pen.setColor (Qt::red);
    main_path_item->setPen (pen);
    main_path_item->setFlag (QGraphicsItem::ItemIsPanel);
    main_path_item->setPath(*shahe_painter_path);
    scene->addItem (main_path_item);
    qDebug()<<"painter path init done"<<endl;

    view->show();
    qDebug()<<"view show"<<endl;

    lb_class = new QLabel;

    pbt_cancel = new QPushButton("取消");
    connect(pbt_cancel, &QPushButton::clicked, this, &MainWindow::cancel);

    lb_click_des = new QLabel;
    glayout->addWidget(lb_click_des,GRID_ROWS+2,0);

    //findpath的一堆按钮和标签（和searchspot共用）
    lb_findpath_dis = new QLabel;
    lb_findpath_time = new QLabel;

    pbt_findpath_dis = new QPushButton("选择");
    pbt_findpath_time = new QPushButton("选择");
    connect(pbt_findpath_dis, &QPushButton::clicked, this, &MainWindow::choose_findpath_dis);
    connect(pbt_findpath_time, &QPushButton::clicked, this, &MainWindow::choose_findpath_time);
    pbt_findpath_dis->installEventFilter(this);
    pbt_findpath_time->installEventFilter(this);

    //antfind的一堆标签和按钮(共用cancel作为取消)
    lb_antfind_hint = new QLabel("请输入下一个要经过的地点");
    lb_antfind_dis = new QLabel;
    lb_antfind_time = new QLabel;
    le_antfind = new QLineEdit;
    le_antfind->setPlaceholderText("输入地点");
    pbt_antfind_next = new QPushButton("下一个");
    pbt_antfind_start = new QPushButton("开始导航");
    pbt_antfind_dis = new QPushButton("距离优先");
    pbt_antfind_time = new QPushButton("时间优先");
    pbt_antfind_dis->installEventFilter(this);
    pbt_antfind_time->installEventFilter(this);
    connect(le_antfind, &QLineEdit::returnPressed, this, [=](){antfind_fzs(le_antfind->text());});
    connect(pbt_antfind_next, &QPushButton::clicked, this, &MainWindow::antfindNext);
    connect(pbt_antfind_start, &QPushButton::clicked, this, &MainWindow::antfindStart);
    connect(pbt_antfind_dis, &QPushButton::clicked, this, &MainWindow::antfindStartDis);
    connect(pbt_antfind_time, &QPushButton::clicked, this, &MainWindow::antfindStartTime);

    //模糊搜索的标签和按钮
    lb_fuzzysearch = new QLabel;
    pbt_fuzzysearch_reenter = new QPushButton("重新输入");
    connect(pbt_fuzzysearch_reenter, &QPushButton::clicked, this, &MainWindow::reenter);

    qDebug()<<"pbt init done"<<endl;

    //初始化中间线路
    for(int i = 0; i<S2B_BUS_PATH_NO.size(); i++) {
        s2b_bus_path.append(vertices[S2B_BUS_PATH_NO[i]]);
        b2s_bus_path.prepend(vertices[S2B_BUS_PATH_NO[i]]);
        if(i>0) {
            bus_total_dis += adj_map[S2B_BUS_PATH_NO[i-1]][S2B_BUS_PATH_NO[i]]->get_distance();
            bus_total_time += adj_map[S2B_BUS_PATH_NO[i-1]][S2B_BUS_PATH_NO[i]]->get_distance()/(BUS*adj_map[S2B_BUS_PATH_NO[i-1]][S2B_BUS_PATH_NO[i]]->get_crowdness());
        }
    }
    for(int i = 0; i<S2B_SHUTTLE_PATH_NO.size(); i++) {
        s2b_shuttle_path.append(vertices[S2B_SHUTTLE_PATH_NO[i]]);
        b2s_shuttle_path.prepend(vertices[S2B_SHUTTLE_PATH_NO[i]]);
        if(i>0) {
            shuttle_total_dis += adj_map[S2B_SHUTTLE_PATH_NO[i-1]][S2B_SHUTTLE_PATH_NO[i]]->get_distance();
            shuttle_total_time += adj_map[S2B_SHUTTLE_PATH_NO[i-1]][S2B_SHUTTLE_PATH_NO[i]]->get_distance()/(SHUTTLE*adj_map[S2B_SHUTTLE_PATH_NO[i-1]][S2B_SHUTTLE_PATH_NO[i]]->get_crowdness());
        }
    }
    qDebug()<<"between path init down"<<endl;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete tmr_clock;
    delete log;
    delete current;
    delete clock;
    delete online_account;
    delete shahe_painter_path;
    delete benbu_painter_path;
    delete between_painter_path;
    scene->addItem(current_point);
    glayout->addWidget(pbt_cancel);

    glayout->addWidget(pbt_findpath_dis);
    glayout->addWidget(pbt_findpath_time);
    glayout->addWidget(lb_findpath_dis);
    glayout->addWidget(lb_findpath_time);

    glayout->addWidget(lb_class);

    glayout->addWidget(pbt_fuzzysearch_reenter);
    glayout->addWidget(lb_fuzzysearch);

    glayout->addWidget(lb_antfind_hint);
    glayout->addWidget(lb_antfind_dis);
    glayout->addWidget(lb_antfind_time);
    glayout->addWidget(pbt_antfind_dis);
    glayout->addWidget(pbt_antfind_time);
    glayout->addWidget(pbt_antfind_next);
    glayout->addWidget(pbt_antfind_start);
    glayout->addWidget(le_antfind);

}

void MainWindow::mouseDoubleClickEvent (QMouseEvent *e) {
    int x = e->x()-offset_x;
    int y = e->y()-offset_y;
    for(auto ver : vertices) {
        if(ver->get_isSpot()==false) continue;
        //如果该点属于当前地图
        if((campus==Cp_Shahe && inShahe(ver->get_no())) ||
                (campus==Cp_Benbu && inBenbu(ver->get_no())) ||
                (campus==Cp_Between && inBetween(ver->get_no()))) {
            //如果点击处在该顶点范围内
            if((ver->get_x()-x)*(ver->get_x()-x) + (ver->get_y()-y)*(ver->get_y()-y)<=400) {
                //显示该顶点信息
                lb_click_des->setText("编号"+QString::number(ver->get_no())+" : "+ver->get_description());

                break;
            }
        }
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    //对于findpath的按钮
    if(pbt_findpath_dis==watched) {
        if(event->type() == QEvent::Enter) {
            //如果是searchspot
            if(findpath_begin == -2) {
                if(cross==false) drawPath(dij_findpath_dis.get_dij_path(),true);
                else drawPath(dij_dis_first.get_dij_path(),true);
            }
            //如果从当前点出发（画线要从当前点出发）
            else if(findpath_begin == -1) {
                //如果没有跨校区
                if(cross==false) drawPath(dij_findpath_dis.get_dij_path(),true);
                else drawPath(dij_dis_first.get_dij_path(),true);
            }
            //如果不是从当前点出发
            else {
                if(cross==false) drawPath(dij_findpath_dis.get_dij_path(),false);
                else drawPath(dij_dis_first.get_dij_path(),false);
            }
            return true;
        }
        if(event->type() == QEvent::Leave) {
            if(!cross && ((!inShahe(dij_findpath_dis.get_dij_path()[0]->get_no()) && campus==Cp_Shahe) ||
                    (!inBenbu(dij_findpath_dis.get_dij_path()[0]->get_no()) && campus==Cp_Benbu) ||
                    (!inBetween(dij_findpath_dis.get_dij_path()[0]->get_no()) && campus==Cp_Between)))
                return true;
            if(cross && ((!inShahe(dij_dis_first.get_dij_path()[0]->get_no()) && campus==Cp_Shahe) ||
                    (!inBenbu(dij_dis_first.get_dij_path()[0]->get_no()) && campus==Cp_Benbu) ||
                    (!inBetween(dij_dis_first.get_dij_path()[0]->get_no()) && campus==Cp_Between)))
                return true;
            QList<QGraphicsItem*> listItem = scene->items();
            scene->removeItem(listItem.front());
            delete listItem.front();
            listItem.removeAt(0);
            return true;
        }
    }
    if(pbt_findpath_time==watched) {
        if(event->type() == QEvent::Enter) {
            if(findpath_begin==-2) {
                //如果没有跨校区
                if(cross==false) drawPath(dij_findpath_time.get_dij_path(),true);
                else drawPath(dij_time_first.get_dij_path(),true);
            }
            //如果从当前点出发（画线要从当前点出发）
            else if(findpath_begin==-1) {
                //如果没有跨校区
                if(cross==false) drawPath(dij_findpath_time.get_dij_path(),true);
                else drawPath(dij_time_first.get_dij_path(),true);
            }
            //如果不是从当前点出发
            else {
                if(cross==false) drawPath(dij_findpath_time.get_dij_path(),false);
                else drawPath(dij_time_first.get_dij_path(),false);
            }
            return true;
        }
        if(event->type() == QEvent::Leave) {
            if(!cross && ((!inShahe(dij_findpath_time.get_dij_path()[0]->get_no()) && campus==Cp_Shahe) ||
                    (!inBenbu(dij_findpath_time.get_dij_path()[0]->get_no()) && campus==Cp_Benbu) ||
                    (!inBetween(dij_findpath_time.get_dij_path()[0]->get_no()) && campus==Cp_Between)))
                return true;
            if(cross && ((!inShahe(dij_time_first.get_dij_path()[0]->get_no()) && campus==Cp_Shahe) ||
                    (!inBenbu(dij_time_first.get_dij_path()[0]->get_no()) && campus==Cp_Benbu) ||
                    (!inBetween(dij_time_first.get_dij_path()[0]->get_no()) && campus==Cp_Between)))
                return true;
            QList<QGraphicsItem*> listItem = scene->items();
            scene->removeItem(listItem.front());
            delete listItem.front();
            listItem.removeAt(0);
            return true;
        }
    }

    //对于antfind的按钮
    if(pbt_antfind_dis==watched) {
        if(event->type() == QEvent::Enter) {
            if(!cross) drawPath(ant_path_dis,false);
            else drawPath(ant_path_shahe_dis,false);
            return true;
        }
        if(event->type() == QEvent::Leave) {
            if((!cross && ((inShahe(ant_path_dis.front()->get_no()) && campus==Cp_Shahe) ||
                          (inBenbu(ant_path_dis.front()->get_no()) && campus==Cp_Benbu))) ||
                    (cross && campus==Cp_Shahe)) {
                QList<QGraphicsItem*> listItem = scene->items();
                scene->removeItem(listItem.front());
                delete listItem.front();
                listItem.removeAt(0);
            }
            return true;
        }
    }
    if(pbt_antfind_time==watched) {
        if(event->type() == QEvent::Enter) {
            if(!cross) drawPath(ant_path_time,false);

            else drawPath(ant_path_shahe_time,false);
            return true;
        }
        if(event->type() == QEvent::Leave) {
            if((!cross && ((inShahe(ant_path_time.front()->get_no()) && campus==Cp_Shahe) ||
                          (inBenbu(ant_path_time.front()->get_no()) && campus==Cp_Benbu))) ||
                    (cross && campus==Cp_Shahe)) {
                QList<QGraphicsItem*> listItem = scene->items();
                scene->removeItem(listItem.front());
                delete listItem.front();
                listItem.removeAt(0);
            }
            return true;
        }
    }

    //对于searchnear的按钮
    for(int i = 0; i<pbtlist_searchnear.size(); i++) {
        if(pbtlist_searchnear[i]==watched) {
            int no = list_to_no[i];
            if(event->type() == QEvent::Enter) {
                drawPath(dij_searchnear.get_paths()[no],true);
                return true;
            }
            if(event->type() == QEvent::Leave) {
                if((!inShahe(dij_searchnear.get_paths()[no][0]->get_no()) && campus==Cp_Shahe) ||
                        (!inBenbu(dij_searchnear.get_paths()[no][0]->get_no()) && campus==Cp_Benbu) ||
                        (!inBetween(dij_searchnear.get_paths()[no][0]->get_no()) && campus==Cp_Between))
                    return true;
                QList<QGraphicsItem*> listItem = scene->items();
                scene->removeItem(listItem.front());
                delete listItem.front();
                listItem.removeAt(0);
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched,event);
}

//读取邻接表信息，一行可以有多个相邻节点，故每行长度不定，需要双重循环
void MainWindow::readAdj() {
    ifstream fin;
    fin.open(ADJMAP_PATH,ios::in);
    if(!fin.is_open()) {
        qDebug()<<"file open error"<<endl;
        exit(1);
    }
    int no;
    qreal dis;
    qreal crd;
    qreal v;
    char ch;
    while (fin>>no>>dis>>crd>>v) {
        ch = fin.get();
        QMap<int,Edge*> temp;
        while(fin.peek()!=EOF&&ch!='\r'&&ch!='\n') {
            Edge *edge = new Edge(dis,crd,v);
            temp[no] = edge;
            fin>>no>>dis>>crd>>v;
            ch = fin.get();
        }
        Edge *edge = new Edge(dis,crd,v);
        temp[no] = edge;
        adj_map.push_back(temp);
    }
    fin.close();
    qDebug()<<"map load success"<<endl;
    dij_searchnear.set_adj_map(adj_map);
    dij_findpath_time.set_adj_map(adj_map);
    dij_findpath_dis.set_adj_map(adj_map);
    dij_first_dis_bus.set_adj_map(adj_map);
    dij_first_time_bus.set_adj_map(adj_map);
    dij_first_dis_shuttle.set_adj_map(adj_map);
    dij_first_time_shuttle.set_adj_map(adj_map);
    dij_second_bus.set_adj_map(adj_map);
    dij_second_shuttle.set_adj_map(adj_map);
    dij_third_dis_bus.set_adj_map(adj_map);
    dij_third_time_bus.set_adj_map(adj_map);
    dij_third_dis_shuttle.set_adj_map(adj_map);
    dij_third_time_shuttle.set_adj_map(adj_map);
}

//读入顶点信息，一行是一个顶点的信息
void MainWindow::readVer() {
    ifstream fin;
    fin.open(VERTICES_PATH,ios::in);
    if(!fin.is_open()) {
        qDebug()<<"file open error"<<endl;
        exit(1);
    }
    int no;
    qreal x;
    qreal y;
    string description;
    bool isSpot;
    while (fin>>no>>x>>y>>description>>isSpot) {
        Vertex *ver = new Vertex(no,x,y,QString::fromStdString(description),isSpot);
        vertices.push_back(ver);
    }
    fin.close();
    qDebug()<<"vertices load success"<<endl;
    dij_searchnear.set_vertices(vertices);
    dij_findpath_dis.set_vertices(vertices);
    dij_findpath_time.set_vertices(vertices);
    dij_first_dis_bus.set_vertices(vertices);
    dij_first_time_bus.set_vertices(vertices);
    dij_first_dis_shuttle.set_vertices(vertices);
    dij_first_time_shuttle.set_vertices(vertices);
    dij_second_bus.set_vertices(vertices);
    dij_second_shuttle.set_vertices(vertices);
    dij_third_dis_bus.set_vertices(vertices);
    dij_third_time_bus.set_vertices(vertices);
    dij_third_dis_shuttle.set_vertices(vertices);
    dij_third_time_shuttle.set_vertices(vertices);
}

//逻辑名到物理名的映射，一个逻辑名可对应多个物理名，故需要双重while
void MainWindow::readL2P() {
    ifstream fin;
    fin.open(L2P_PATH, ios::in);
    if(!fin.is_open()) {
        qDebug()<<"file open error"<<endl;
        exit(1);
    }
    string logic;
    int no;
    char ch;
    while (fin>>logic>>no) {
        ch = fin.get();
        QVector<int> temp;
        while(fin.peek()!=EOF&&ch!='\r'&&ch!='\n') {
            temp.push_back(no);
            fin>>no;
            ch = fin.get();
        }
        temp.push_back(no);
        logic2phy[QString::fromStdString(logic)] = temp;
    }
    fin.close();
    qDebug()<<"L2P load success"<<endl;
}

//班车时刻表，每行是一班车的发车时间
void MainWindow::readShuttleSchedule() {
    ifstream fin;
    fin.open(SHUTTLE_SCHEDULE_PATH, ios::in);
    if(!fin.is_open()) {
        qDebug()<<"file open error"<<endl;
        exit(1);
    }
    int min,hour,wday;
    while (fin>>wday>>hour>>min) {
        Clock tmp(0,min,hour,wday);
        shuttle_schedule.append(tmp);
    }
    fin.close();
    qDebug()<<"shuttle schedule load success"<<endl;
}

void MainWindow::initAction() {
    findpath = new QAction(QIcon(":/images/search"),"搜索路径", this);
    findpath->setStatusTip ("搜索路线");
    connect(findpath, &QAction::triggered, this, &MainWindow::findPath);

    search_spot = new QAction(QIcon(":/images/search"),"搜索地点", this);
    search_spot->setStatusTip ("搜索地点");
    connect(search_spot, &QAction::triggered, this, &MainWindow::searchSpot);

    search_near = new QAction(QIcon(":/images/search"),"搜索附近", this);
    search_near->setStatusTip ("搜索附近");
    connect(search_near, &QAction::triggered, this, &MainWindow::searchNear);

    ant_find = new QAction(QIcon(":/images/search"),"多点寻路", this);
    ant_find->setStatusTip ("多点寻路");
    connect(ant_find, &QAction::triggered, this, &MainWindow::antFind);

    switch_shahe = new QAction(QIcon(":/images/switch_shahe"),"切换到沙河", this);
    switch_shahe->setStatusTip ("切换到沙河");
    connect(switch_shahe, &QAction::triggered, this, &MainWindow::switchShahe);

    switch_between = new QAction(QIcon(":/images/switch_between"),"切换到校外", this);
    switch_between->setStatusTip ("切换到校外");
    connect(switch_between, &QAction::triggered, this, &MainWindow::switchBetween);

    switch_benbu = new QAction(QIcon(":/images/switch_benbu"),"切换到本部", this);
    switch_benbu->setStatusTip ("切换到本部");
    connect(switch_benbu, &QAction::triggered, this, &MainWindow::switchBenbu);

    set_time = new QAction(QIcon(":/images/set_time"),"设置时间", this);
    set_time->setStatusTip ("设置时间");
    connect(set_time, &QAction::triggered, this, &MainWindow::setTime);

    log_search = new QAction(QIcon(":/images/log"),"日志搜索", this);
    log_search->setStatusTip("日志搜索");
    connect(log_search, &QAction::triggered, this, &MainWindow::logSearch);

    go_to_class = new QAction(QIcon(":/images/class"),"导航去教室", this);
    go_to_class->setStatusTip("导航去教室");
    connect(go_to_class, &QAction::triggered, this, &MainWindow::gotoClass);

    qDebug()<<"init action down"<<endl;
}

void MainWindow::initToolBar() {
    toolbar = addToolBar("tool");
    le_search = new QLineEdit;
    le_begin = new QLineEdit;
    le_end = new QLineEdit;
    le_searchnear = new QLineEdit;
    le_search->setPlaceholderText("输入地点");
    connect(le_search, &QLineEdit::returnPressed, this, [=](){search_fzs(le_search->text());});
    le_begin->setPlaceholderText("输入起点");
    connect(le_begin, &QLineEdit::returnPressed, this, [=](){findpath_begin_fzs(le_begin->text());});
    le_end->setPlaceholderText("输入终点");
    connect(le_end, &QLineEdit::returnPressed, this, [=](){findpath_end_fzs(le_end->text());});
    le_searchnear->setPlaceholderText("输入范围");
    lb_search = new QLabel("搜索地点");
    lb_searchnear = new QLabel("搜索范围");
    lb_begin = new QLabel("起点");
    lb_end = new QLabel("终点");
    lb_antfind = new QLabel("多点寻路");
    lb_velocity = new QLabel("交通工具");
    lb_time1 = new QLabel(":");
    lb_time2 = new QLabel(":");
    lb_wday = new QLabel("星期");
    le_hour = new QLineEdit(QString::number(clock->get_hour()));
    le_min = new QLineEdit(QString::number(clock->get_min()));
    le_sec = new QLineEdit(QString::number(clock->get_sec()));
    le_wday = new QLineEdit(QString::number(clock->get_wday()));
    cb_velocity = new QComboBox;
    cb_velocity->addItem("停止");
    cb_velocity->addItem("步行");
    cb_velocity->addItem("自行车");
    cb_velocity->setCurrentIndex(0);
    connect(cb_velocity, SIGNAL(currentIndexChanged(int)), this, SLOT(setVelocity(int)));
    connect(le_search, SIGNAL(textEdited(QString)), this, SLOT(stopTime()));
    connect(le_begin, SIGNAL(textEdited(QString)), this, SLOT(stopTime()));
    connect(le_end, SIGNAL(textEdited(QString)), this, SLOT(stopTime()));
    connect(le_searchnear, SIGNAL(textEdited(QString)), this, SLOT(stopTime()));
    connect(le_hour, SIGNAL(textEdited(QString)), this, SLOT(stopTime()));
    connect(le_min, SIGNAL(textEdited(QString)), this, SLOT(stopTime()));
    connect(le_sec, SIGNAL(textEdited(QString)), this, SLOT(stopTime()));
    connect(le_wday, SIGNAL(textEdited(QString)), this, SLOT(stopTime()));
    toolbar->addWidget(lb_search);
    toolbar->addWidget(le_search);
    toolbar->addAction(search_spot);
    toolbar->addSeparator();
    toolbar->addWidget(lb_begin);
    toolbar->addWidget(le_begin);
    toolbar->addWidget(lb_end);
    toolbar->addWidget(le_end);
    toolbar->addAction(findpath);
    toolbar->addSeparator();
    toolbar->addWidget(lb_searchnear);
    toolbar->addWidget(le_searchnear);
    toolbar->addAction(search_near);
    toolbar->addSeparator();
    toolbar->addWidget(lb_antfind);
    toolbar->addAction(ant_find);
    toolbar->addSeparator();
    toolbar->addWidget(lb_velocity);
    toolbar->addWidget(cb_velocity);
    toolbar->addSeparator();
    toolbar->addWidget(le_hour);
    toolbar->addWidget(lb_time1);
    toolbar->addWidget(le_min);
    toolbar->addWidget(lb_time2);
    toolbar->addWidget(le_sec);
    toolbar->addWidget(lb_wday);
    toolbar->addWidget(le_wday);
    toolbar->addAction(set_time);
    toolbar->addSeparator();
    toolbar->addAction(switch_shahe);
    toolbar->addSeparator();
    toolbar->addAction(switch_benbu);
    toolbar->addSeparator();
    toolbar->addAction(switch_between);
    toolbar->addSeparator();
    toolbar->addAction(go_to_class);
    toolbar->addSeparator();
    toolbar->addAction(log_search);

    qDebug()<<"init tool bar down"<<endl;
}

void MainWindow::initTimer() {
    tmr_clock = new QTimer;
    connect(tmr_clock, SIGNAL(timeout()), this, SLOT(clock_slot()));
    tmr_clock->start(167);

    qDebug()<<"init timer down"<<endl;
}

//每次超时系统时间更新,同时显示出新的时间
void MainWindow::updateTime() {
    clock->update_clock();
    le_hour->setText(QString::number(clock->get_hour()));
    le_min->setText(QString::number(clock->get_min()));
    le_sec->setText(QString::number(clock->get_sec()));
    le_wday->setText(QString::number(clock->get_wday()));
}

//每次进行操作时都会先将时钟暂停
void MainWindow::stopTime() {
    tmr_clock->stop();
}

//设置时间，加速时间流动直到达到设置的时间
//1ms==1s
void MainWindow::setTime() {
    //记录日志文件
    log->addItem(*clock,SET_TIME,"设置时间为：星期"+le_wday->text()+":"+le_hour->text()+":"+le_min->text()+":"+le_sec->text());

    Clock tmp(le_sec->text().toInt(),le_min->text().toInt(),le_hour->text().toInt(),le_wday->text().toInt());
    //求出时间差，这段差值加速通过
    goal_sec += tmp.difSec(*clock)%(24*3600*7);
    if(goal_sec>0) {
        tmr_clock->start(1);
        goal_sec--;
    } else {
        tmr_clock->start(167);
    }
}

//打开新窗口，搜索日志
void MainWindow::logSearch() {
    log_dialog->sendData(*clock);
    log_dialog->show();
}

//导航去当前教室
void MainWindow::gotoClass() {
    if(current->get_velocity()==SHUTTLE || current->get_velocity()==BUS) {
        QMessageBox::warning(this, "Warning!", "在车上无法更改导航，请下车后重试!", QMessageBox::Yes);
        return;
    }

    Course tmp = online_account->gotoclass(*clock);

    if(tmp.get_classroom()==-1) {
        QMessageBox::warning(this, "Warning!", "当前没课!", QMessageBox::Yes);
        return;
    }

    QString cname = tmp.get_name();
    Clock begin_t = tmp.get_begin_time();
    Clock end_t = tmp.get_end_time();
    search_end = tmp.get_classroom();
    lb_class->setText("当前正在上："+cname+"课\n开始时间："+begin_t.get_clock_str()+
                      "\n结束时间："+end_t.get_clock_str()+"\n地点为："+vertices[search_end]->get_description());
    glayout->addWidget(lb_class,0,1,1,2);

    //其中包含时间停止和设置按钮
    searchSpot();

    //弹出search_spot的日志记录，再加入新的gotoclass
    log->popItem();
    log->addItem(*clock,GOTO_CLASS,"前往"+vertices[search_end]->get_description());

    //借用完要清空
    search_end = -2;
}

void MainWindow::searchNear() {
    if(current->get_velocity()==SHUTTLE || current->get_velocity()==BUS) {
        QMessageBox::warning(this, "Warning!", "在车上无法更改导航，请下车后重试!", QMessageBox::Yes);
        return;
    }

    //记录日志
    log->addItem(*clock,SEARCH_NEAR,"搜索范围："+le_searchnear->text());

    //如果当前位置位于节点上
    if(current->get_current_pass()==0) {
        dij_searchnear.singleSearch(current->get_last_ver()->get_no(),
                          le_searchnear->text().toDouble(),velocity);
    } else {
        dij_searchnear.doubleSearch(current->get_last_ver()->get_no(),
                          current->get_next_ver()->get_no(), le_searchnear->text().toDouble(),velocity,
                          current->get_current_pass(), current->get_current_dis()-current->get_current_pass());
    }
    QMap<int,QVector<Vertex*>> paths = dij_searchnear.get_paths();
    QMap<int,qreal> dis = dij_searchnear.get_dis();
    QMap<int,qreal> time = dij_searchnear.get_time();
    QPushButton *pbt_temp;
    QLabel *lb_temp;
    list_to_no.clear();
    int i = 0;
    for(auto path : paths) {
        //如果当前节点无关紧要，跳过
        if(path.back()->get_isSpot()==false) {
            continue;
        }
        list_to_no[i] = path.back()->get_no();
        lb_temp = new QLabel("地点："+ path.back()->get_description()+
                             "，距离："+ QString::number(dis[path.back()->get_no()])+
                             "，时间："+ QString::number(time[path.back()->get_no()]));
        pbt_temp = new QPushButton("到这去");
        lblist_searchnear.append(lb_temp);
        pbtlist_searchnear.append(pbt_temp);
        pbt_temp->installEventFilter(this);
        connect(pbt_temp, &QPushButton::clicked, this, [=](){choose_searchnear(i);});
        glayout->addWidget(lb_temp,i,1);
        glayout->addWidget(pbt_temp,i,2);
        i++;
    }
    glayout->addWidget(pbt_cancel,i,1,1,2);

}

//输入名称搜索地点，自动调用fuzzysearch后，search_end存储目的顶点编号
//地点信息显示在右侧（名称，距离，时间）,类似findpath
void MainWindow::searchSpot() {
    if(search_end==-2) {
        QMessageBox::warning(this, "Warning!", "地点不能为空!", QMessageBox::Yes);
        return;
    }
    if(current->get_velocity()==SHUTTLE || current->get_velocity()==BUS) {
        QMessageBox::warning(this, "Warning!", "在车上无法更改导航，请下车后重试!", QMessageBox::Yes);
        return;
    }

    stopTime();

    //记录日志
    log->addItem(*clock,SEARCH,vertices[search_end]->get_description());

    //不用跨校区(current位置只可能在校园内，因为在between时是不允许操作的)
    if((current->inShahe() && inShahe(search_end)) ||
            (current->inBenbu() && inBenbu(search_end))) {
        cross = false;
        //在路中间
        if(current->get_current_pass()!=0) {
            dij_findpath_dis.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                        search_end,Pr_Distance,velocity,current->get_current_pass(),
                                        current->get_current_dis()-current->get_current_pass());
            dij_findpath_time.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                         search_end,Pr_Time,velocity,current->get_current_pass(),
                                         current->get_current_dis()-current->get_current_pass());
        }
        //在节点上
        else {
            dij_findpath_dis.singleFind(current->get_last_ver()->get_no(),search_end,Pr_Distance,velocity);
            dij_findpath_time.singleFind(current->get_last_ver()->get_no(),search_end,Pr_Time,velocity);
        }
        //两个标签、两个按钮选择优先度，还有cancel按钮
        lb_findpath_dis->setText("距离优先，总距离："+QString::number(dij_findpath_dis.get_total_dis())+",总时间："+QString::number(dij_findpath_dis.get_total_time()));
        lb_findpath_time->setText("时间优先，总距离："+QString::number(dij_findpath_time.get_total_dis())+",总时间："+QString::number(dij_findpath_time.get_total_time()));
    }
    //要跨校区
    else {
        cross = true;
        //在路中间
        if(current->get_current_pass()!=0) {
            //如果起点在沙河，第一段终点为沙河的上车点（bus/shuttle），否则是本部的
            dij_first_dis_shuttle.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                        current->inShahe()?SHAHE_SHUTTLE_VER_NO:BENBU_SHUTTLE_VER_NO,Pr_Distance,velocity,
                                        current->get_current_pass(),current->get_current_dis()-current->get_current_pass());
            dij_first_time_shuttle.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                        current->inShahe()?SHAHE_SHUTTLE_VER_NO:BENBU_SHUTTLE_VER_NO,Pr_Time,velocity,
                                        current->get_current_pass(),current->get_current_dis()-current->get_current_pass());
            dij_first_dis_bus.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                         current->inShahe()?SHAHE_BUS_VER_NO:BENBU_BUS_VER_NO,Pr_Distance,velocity,
                                         current->get_current_pass(),current->get_current_dis()-current->get_current_pass());
            dij_first_time_bus.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                         current->inShahe()?SHAHE_BUS_VER_NO:BENBU_BUS_VER_NO,Pr_Time,velocity,
                                         current->get_current_pass(),current->get_current_dis()-current->get_current_pass());
        }
        //在节点上
        else if(current->get_current_pass()==0) {
            dij_first_dis_shuttle.singleFind(current->get_last_ver()->get_no(),current->inShahe()?SHAHE_SHUTTLE_VER_NO:BENBU_SHUTTLE_VER_NO,Pr_Distance,velocity);
            dij_first_time_shuttle.singleFind(current->get_last_ver()->get_no(),current->inShahe()?SHAHE_SHUTTLE_VER_NO:BENBU_SHUTTLE_VER_NO,Pr_Time,velocity);
            dij_first_dis_bus.singleFind(current->get_last_ver()->get_no(),current->inShahe()?SHAHE_BUS_VER_NO:BENBU_BUS_VER_NO,Pr_Distance,velocity);
            dij_first_time_bus.singleFind(current->get_last_ver()->get_no(),current->inShahe()?SHAHE_BUS_VER_NO:BENBU_BUS_VER_NO,Pr_Time,velocity);
        }

        //设置第二段的路线(只需要path，dis，time)
        if(current->inShahe()) {
            dij_second_bus.set_dij_path(s2b_bus_path);
            dij_second_shuttle.set_dij_path(s2b_shuttle_path);
            dij_second_bus.set_begin(SHAHE_BUS_VER_NO);
            dij_second_shuttle.set_begin(SHAHE_SHUTTLE_VER_NO);
            dij_second_bus.set_end(BENBU_BUS_VER_NO);
            dij_second_shuttle.set_end(BENBU_SHUTTLE_VER_NO);
        } else if(current->inBenbu()) {
            dij_second_bus.set_dij_path(b2s_bus_path);
            dij_second_shuttle.set_dij_path(b2s_shuttle_path);
            dij_second_bus.set_begin(BENBU_BUS_VER_NO);
            dij_second_shuttle.set_begin(BENBU_SHUTTLE_VER_NO);
            dij_second_bus.set_end(SHAHE_BUS_VER_NO);
            dij_second_shuttle.set_end(SHAHE_SHUTTLE_VER_NO);
        }
        dij_second_bus.set_total_dis(bus_total_dis);
        dij_second_shuttle.set_total_dis(shuttle_total_dis);
        dij_second_bus.set_total_time(bus_total_time);
        dij_second_shuttle.set_total_time(shuttle_total_time);

        //设置后两段的寻路
        dij_third_dis_shuttle.singleFind(current->inShahe()?BENBU_SHUTTLE_VER_NO:SHAHE_SHUTTLE_VER_NO,search_end,Pr_Distance,velocity);
        dij_third_time_shuttle.singleFind(current->inShahe()?BENBU_SHUTTLE_VER_NO:SHAHE_SHUTTLE_VER_NO,search_end,Pr_Time,velocity);
        dij_third_dis_bus.singleFind(current->inShahe()?BENBU_BUS_VER_NO:SHAHE_BUS_VER_NO,search_end,Pr_Distance,velocity);
        dij_third_time_bus.singleFind(current->inShahe()?BENBU_BUS_VER_NO:SHAHE_BUS_VER_NO,search_end,Pr_Time,velocity);

        //距离优先（步行距离优先）（不考虑second段）
        if(dij_first_dis_bus.get_total_dis()+dij_third_dis_bus.get_total_dis()<dij_first_dis_shuttle.get_total_dis()+dij_third_dis_shuttle.get_total_dis()) {
            dij_dis_first = dij_first_dis_bus;
            dij_dis_second = dij_second_bus;
            dij_dis_third = dij_third_dis_bus;
            cross_dis_v = BUS;
        } else {
            dij_dis_first = dij_first_dis_shuttle;
            dij_dis_second = dij_second_shuttle;
            dij_dis_third = dij_third_dis_shuttle;
            cross_dis_v = SHUTTLE;
        }

        //时间优先，要考虑车上时间以及等车时间
        if(dij_first_time_bus.get_total_time()+dij_second_bus.get_total_time()+dij_third_time_bus.get_total_time()+
                getFirstBusTime(dij_first_time_bus.get_total_time())<
                dij_first_time_shuttle.get_total_time()+dij_second_shuttle.get_total_time()+dij_third_time_shuttle.get_total_time()+
                getFirstShuttleTime(dij_first_time_shuttle.get_total_time())) {
            //bus时间更短
            dij_time_first = dij_first_time_bus;
            dij_time_second = dij_second_bus;
            dij_time_third = dij_third_time_bus;
            cross_time_v = BUS;
            wait_sec = getFirstBusTime(dij_first_time_bus.get_total_time());
        } else {
            dij_time_first = dij_first_time_shuttle;
            dij_time_second = dij_second_shuttle;
            dij_time_third = dij_third_time_shuttle;
            cross_time_v = SHUTTLE;
            wait_sec = getFirstShuttleTime(dij_first_time_shuttle.get_total_time());
        }

        //设置标签文字
        lb_findpath_dis->setText("距离优先，总距离："+QString::number(dij_dis_first.get_total_dis()+dij_dis_second.get_total_dis()+dij_dis_third.get_total_dis())+
                                 ",总时间："+QString::number(dij_dis_first.get_total_time()+dij_dis_third.get_total_time()+dij_dis_third.get_total_time()));
        lb_findpath_time->setText("时间优先，总距离："+QString::number(dij_time_first.get_total_dis()+dij_time_second.get_total_dis()+dij_time_third.get_total_dis())+
                                  ",总时间："+QString::number(dij_time_first.get_total_time()+dij_time_third.get_total_time()+dij_time_third.get_total_time()));
    }

    glayout->addWidget(lb_findpath_dis,1,1);
    glayout->addWidget(lb_findpath_time,2,1);
    glayout->addWidget(pbt_findpath_dis,1,2);
    glayout->addWidget(pbt_findpath_time,2,2);
}

//两点寻路槽函数
void MainWindow::findPath() {    
    //有不明确的起点或终点
    if(findpath_begin==-2 || findpath_end==-2) {
        //输出warning
        QMessageBox::warning(this, "Warning!", "地点不能为空!", QMessageBox::Yes);
        return;
    }
    if(current->get_velocity()==SHUTTLE || current->get_velocity()==BUS) {
        QMessageBox::warning(this, "Warning!", "在车上无法更改导航，请下车后重试!", QMessageBox::Yes);
        return;
    }

    stopTime();

    //记录日志
    if(findpath_begin==-1) log->addItem(*clock, FINDPATH, "起点：当前位置，终点："+vertices[findpath_end]->get_description());
    else log->addItem(*clock, FINDPATH, "起点："+vertices[findpath_begin]->get_description()+"，终点："+vertices[findpath_end]->get_description());

    //如果不用跨校区（不用考虑当前位置在校区间的情况）
    if((inShahe(findpath_begin) && inShahe(findpath_end)) ||
            (inBenbu(findpath_begin) && inBenbu(findpath_end))||
            (findpath_begin==-1 && current->inShahe() && inShahe(findpath_end))||
            (findpath_begin==-1 && current->inBenbu() && inBenbu(findpath_end))) {
        //设置标志为不用跨校区false
        cross = false;
        //在当前位置，在路中间
        if(findpath_begin==-1 && current->get_current_pass()!=0) {
            dij_findpath_dis.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                        findpath_end,Pr_Distance,velocity,
                                        current->get_current_pass(),current->get_current_dis()-current->get_current_pass());
            dij_findpath_time.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                         findpath_end,Pr_Time,velocity,
                                         current->get_current_pass(),current->get_current_dis()-current->get_current_pass());
        }
        //在当前位置的节点上
        else if(findpath_begin==-1 && current->get_current_pass()==0) {
            dij_findpath_dis.singleFind(current->get_last_ver()->get_no(),findpath_end,Pr_Distance,velocity);
            dij_findpath_time.singleFind(current->get_last_ver()->get_no(),findpath_end,Pr_Time,velocity);
        }
        //从另外的节点出发
        else {
            dij_findpath_dis.singleFind(findpath_begin,findpath_end,Pr_Distance,velocity);
            dij_findpath_time.singleFind(findpath_begin,findpath_end,Pr_Time,velocity);
            //修改当前位置到起点
            setCurrentToVer(findpath_begin);
        }
        //两个标签、两个按钮选择优先度，还有cancel按钮
        lb_findpath_dis->setText("距离优先，总距离："+QString::number(dij_findpath_dis.get_total_dis())+",总时间："+QString::number(dij_findpath_dis.get_total_time()));
        lb_findpath_time->setText("时间优先，总距离："+QString::number(dij_findpath_time.get_total_dis())+",总时间："+QString::number(dij_findpath_time.get_total_time()));
    }
    //要跨校区
    else {
        cross = true;
        //在当前位置，在路中间
        if(findpath_begin==-1 && current->get_current_pass()!=0) {
            //如果起点在沙河，第一段终点为沙河的上车点（bus/shuttle），否则是本部的
            dij_first_dis_shuttle.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                        current->inShahe()?SHAHE_SHUTTLE_VER_NO:BENBU_SHUTTLE_VER_NO,Pr_Distance,velocity,
                                        current->get_current_pass(),current->get_current_dis()-current->get_current_pass());
            dij_first_time_shuttle.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                        current->inShahe()?SHAHE_SHUTTLE_VER_NO:BENBU_SHUTTLE_VER_NO,Pr_Time,velocity,
                                        current->get_current_pass(),current->get_current_dis()-current->get_current_pass());
            dij_first_dis_bus.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                         current->inShahe()?SHAHE_BUS_VER_NO:BENBU_BUS_VER_NO,Pr_Distance,velocity,
                                         current->get_current_pass(),current->get_current_dis()-current->get_current_pass());
            dij_first_time_bus.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                         current->inShahe()?SHAHE_BUS_VER_NO:BENBU_BUS_VER_NO,Pr_Time,velocity,
                                         current->get_current_pass(),current->get_current_dis()-current->get_current_pass());
        }
        //在当前位置的节点上
        else if(findpath_begin==-1 && current->get_current_pass()==0) {
            dij_first_dis_shuttle.singleFind(current->get_last_ver()->get_no(),current->inShahe()?SHAHE_SHUTTLE_VER_NO:BENBU_SHUTTLE_VER_NO,Pr_Distance,velocity);
            dij_first_time_shuttle.singleFind(current->get_last_ver()->get_no(),current->inShahe()?SHAHE_SHUTTLE_VER_NO:BENBU_SHUTTLE_VER_NO,Pr_Time,velocity);
            dij_first_dis_bus.singleFind(current->get_last_ver()->get_no(),current->inShahe()?SHAHE_BUS_VER_NO:BENBU_BUS_VER_NO,Pr_Distance,velocity);
            dij_first_time_bus.singleFind(current->get_last_ver()->get_no(),current->inShahe()?SHAHE_BUS_VER_NO:BENBU_BUS_VER_NO,Pr_Time,velocity);
        }
        //从另外的节点出发
        else {
            dij_first_dis_shuttle.singleFind(findpath_begin,findpath_begin<=SHAHE_MAX_NO?SHAHE_SHUTTLE_VER_NO:BENBU_SHUTTLE_VER_NO,Pr_Distance,velocity);
            dij_first_time_shuttle.singleFind(findpath_begin,findpath_begin<=SHAHE_MAX_NO?SHAHE_SHUTTLE_VER_NO:BENBU_SHUTTLE_VER_NO,Pr_Time,velocity);
            dij_first_dis_bus.singleFind(findpath_begin,findpath_begin<=SHAHE_MAX_NO?SHAHE_BUS_VER_NO:BENBU_BUS_VER_NO,Pr_Distance,velocity);
            dij_first_time_bus.singleFind(findpath_begin,findpath_begin<=SHAHE_MAX_NO?SHAHE_BUS_VER_NO:BENBU_BUS_VER_NO,Pr_Time,velocity);
            //修改当前位置到起点
            setCurrentToVer(findpath_begin);
        }
        //此时当前位置即为起点

        //设置第二段的路线(只需要path，dis，time)
        if(current->inShahe()) {
            dij_second_bus.set_dij_path(s2b_bus_path);
            dij_second_shuttle.set_dij_path(s2b_shuttle_path);
            dij_second_bus.set_begin(SHAHE_BUS_VER_NO);
            dij_second_shuttle.set_begin(SHAHE_SHUTTLE_VER_NO);
            dij_second_bus.set_end(BENBU_BUS_VER_NO);
            dij_second_shuttle.set_end(BENBU_SHUTTLE_VER_NO);
        } else if(current->inBenbu()) {
            dij_second_bus.set_dij_path(b2s_bus_path);
            dij_second_shuttle.set_dij_path(b2s_shuttle_path);
            dij_second_bus.set_begin(BENBU_BUS_VER_NO);
            dij_second_shuttle.set_begin(BENBU_SHUTTLE_VER_NO);
            dij_second_bus.set_end(SHAHE_BUS_VER_NO);
            dij_second_shuttle.set_end(SHAHE_SHUTTLE_VER_NO);
        }
        dij_second_bus.set_total_dis(bus_total_dis);
        dij_second_shuttle.set_total_dis(shuttle_total_dis);
        dij_second_bus.set_total_time(bus_total_time);
        dij_second_shuttle.set_total_time(shuttle_total_time);

        //设置最后一段的寻路
        dij_third_dis_shuttle.singleFind(current->inShahe()?BENBU_SHUTTLE_VER_NO:SHAHE_SHUTTLE_VER_NO,findpath_end,Pr_Distance,velocity);
        dij_third_time_shuttle.singleFind(current->inShahe()?BENBU_SHUTTLE_VER_NO:SHAHE_SHUTTLE_VER_NO,findpath_end,Pr_Time,velocity);
        dij_third_dis_bus.singleFind(current->inShahe()?BENBU_BUS_VER_NO:SHAHE_BUS_VER_NO,findpath_end,Pr_Distance,velocity);
        dij_third_time_bus.singleFind(current->inShahe()?BENBU_BUS_VER_NO:SHAHE_BUS_VER_NO,findpath_end,Pr_Time,velocity);

        //决定距离优先时是用bus还是shuttle（步行距离优先）（不考虑second段）
        //如果bus更优
        if(dij_first_dis_bus.get_total_dis()+dij_third_dis_bus.get_total_dis()<dij_first_dis_shuttle.get_total_dis()+dij_third_dis_shuttle.get_total_dis()) {
            dij_dis_first = dij_first_dis_bus;
            dij_dis_second = dij_second_bus;
            dij_dis_third = dij_third_dis_bus;
            cross_dis_v = BUS;
        } else {
            dij_dis_first = dij_first_dis_shuttle;
            dij_dis_second = dij_second_shuttle;
            dij_dis_third = dij_third_dis_shuttle;
            cross_dis_v = SHUTTLE;
        }
        //时间优先，要考虑车上时间以及等车时间
        if(dij_first_time_bus.get_total_time()+dij_second_bus.get_total_time()+dij_third_time_bus.get_total_time()+
                getFirstBusTime(dij_first_time_bus.get_total_time())<
                dij_first_time_shuttle.get_total_time()+dij_second_shuttle.get_total_time()+dij_third_time_shuttle.get_total_time()+
                getFirstShuttleTime(dij_first_time_shuttle.get_total_time())) {
            //bus时间更短
            dij_time_first = dij_first_time_bus;
            dij_time_second = dij_second_bus;
            dij_time_third = dij_third_time_bus;
            cross_time_v = BUS;
            wait_sec = getFirstBusTime(dij_first_time_bus.get_total_time());
        } else {
            dij_time_first = dij_first_time_shuttle;
            dij_time_second = dij_second_shuttle;
            dij_time_third = dij_third_time_shuttle;
            cross_time_v = SHUTTLE;
            wait_sec = getFirstShuttleTime(dij_first_time_shuttle.get_total_time());
        }
        //设置标签文字（跨校区不考虑公交距离）
        lb_findpath_dis->setText("步行距离优先，总步行距离："+QString::number(dij_dis_first.get_total_dis()+dij_dis_third.get_total_dis())+
                                 ",总时间："+QString::number(dij_dis_first.get_total_time()+dij_dis_third.get_total_time()+dij_dis_third.get_total_time()));
        lb_findpath_time->setText("时间优先，总步行距离："+QString::number(dij_time_first.get_total_dis()+dij_time_third.get_total_dis())+
                                  ",总时间："+QString::number(dij_time_first.get_total_time()+dij_time_third.get_total_time()+dij_time_third.get_total_time()));
    }
    //添加标签、按钮
    glayout->addWidget(lb_findpath_dis,0,1);
    glayout->addWidget(lb_findpath_time,1,1);
    glayout->addWidget(pbt_findpath_dis,0,2);
    glayout->addWidget(pbt_findpath_time,1,2);

}

//返回从到达上车点至这班车发车还需要的时间（秒数），10000代表没车
int MainWindow::getFirstShuttleTime(qreal t) {
    for(int i = 0; i<shuttle_schedule.size(); i++) {
        //赶得上的第一班车
        if(shuttle_schedule[i].difSec(*clock)>=t) {
            return shuttle_schedule[i].difSec(*clock)-t;
        }
    }
    //如果没有车了,返回很大的数
    return 1000000;
}

//7分钟一班，从6到23点多一点，返回等待时间
int MainWindow::getFirstBusTime(qreal t) {
    int current_sec = clock->get_hour()*3600 + clock->get_min()*60 + clock->get_sec();
    if(current_sec+t<BUS_BEGIN_SEC) return BUS_BEGIN_SEC-t-current_sec;
    else if(current_sec+t>BUS_END_SEC) return BUS_BEGIN_SEC+24*3600-t-current_sec;
    else return BUS_INTERVAL_SEC-(current_sec+(int)ceil(t)-BUS_BEGIN_SEC)%BUS_INTERVAL_SEC;
}

//把current位置移到某个节点上（findpath或antfind起点可能是其他点）
void MainWindow::setCurrentToVer(int no) {
    current->set_x(vertices[no]->get_x());
    current->set_y(vertices[no]->get_y());
    current->set_last_ver(vertices[no]);
    current->set_next_ver_index(-1);
    current->set_current_pass(0);
    current_point->setPos(current->get_x(), current->get_y());
}

//多点寻路
//首先点击工具栏多点寻路按钮，右侧按钮区会出现提醒输入和lineedit，可以选择继续输入、开始导航或退出
//输入完一个地点会出现模糊搜索的选择，选择完即可进行下一步操作
void MainWindow::antFind() {
    if(current->get_velocity()==SHUTTLE || current->get_velocity()==BUS) {
        QMessageBox::warning(this, "Warning!", "在车上无法更改导航，请下车后重试!", QMessageBox::Yes);
        return;
    }

    //记录日志文件
    log->addItem(*clock, ANTFIND, "开始输入地点");

    //把标签和输入框加入布局
    glayout->addWidget(lb_antfind_hint,0,1,1,2);
    glayout->addWidget(le_antfind,1,1,1,2);
    glayout->addWidget(pbt_antfind_next,2,1);
    glayout->addWidget(pbt_cancel,2,2);
    glayout->addWidget(pbt_antfind_start,3,1,1,2);

    //初始化
    ant_spot = -2;
    ant_spots.clear();
    ant_shahe_spots_bus.clear();
    ant_shahe_spots_shuttle.clear();
    ant_benbu_spots_bus.clear();
    ant_benbu_spots_shuttle.clear();
    have_shahe = false;
    have_benbu = false;

    //时间恢复流动
    tmr_clock->start();
}

void MainWindow::switchShahe() {
    //记录日志文件
    if(campus==Cp_Shahe) log->addItem(*clock, SWC_SHAHE, "从沙河切换");
    else if(campus==Cp_Between) log->addItem(*clock, SWC_SHAHE, "从校外切换");
    else if(campus==Cp_Benbu) log->addItem(*clock, SWC_SHAHE, "从本部切换");

    if(campus==Cp_Shahe) return;
    campus = Cp_Shahe;
    clear();
    main_path_item->setPath(*shahe_painter_path);
    if(current->inShahe()) {
        scene->addItem(current_point);
    } else {
        scene->removeItem(current_point);
    }
    map_item->setPixmap(shahe);

    tmr_clock->start();
}

void MainWindow::switchBetween() {
    //记录日志文件
    if(campus==Cp_Shahe) log->addItem(*clock, SWC_BETWEEN, "从沙河切换");
    else if(campus==Cp_Between) log->addItem(*clock, SWC_BETWEEN, "从校外切换");
    else if(campus==Cp_Benbu) log->addItem(*clock, SWC_BETWEEN, "从本部切换");

    if(campus==Cp_Between) return;
    campus = Cp_Between;
    clear();
    main_path_item->setPath(*between_painter_path);
    if(current->inBetween()) {
        scene->addItem(current_point);
    } else {
        scene->removeItem(current_point);
    }
    map_item->setPixmap(between);

    tmr_clock->start();
}

void MainWindow::switchBenbu() {
    //记录日志文件
    if(campus==Cp_Shahe) log->addItem(*clock, SWC_BENBU, "从沙河切换");
    else if(campus==Cp_Between) log->addItem(*clock, SWC_BENBU, "从校外切换");
    else if(campus==Cp_Benbu) log->addItem(*clock, SWC_BENBU, "从本部切换");

    if(campus==Cp_Benbu) return;
    campus = Cp_Benbu;
    //clear不会删去当前点（前提它在地图中）
    clear();
    main_path_item->setPath(*benbu_painter_path);
    //如果当前点在本部
    if(current->inBenbu()) {
        scene->addItem(current_point);
    } else {
        scene->removeItem(current_point);
    }
    map_item->setPixmap(benbu);

    tmr_clock->start();
}

//currentIndexChanged只有选择与上次不同才会触发
//速度改变导航也要重新规划
void MainWindow::setVelocity(int x) {
    if(current->get_velocity()==BUS||current->get_velocity()==SHUTTLE) {
        QMessageBox::warning(this, "Warning!", "在车上无法更改速度，请下车后重试!", QMessageBox::Yes);
        if(velocity==STOP) cb_velocity->setCurrentIndex(0);
        else if(velocity==FOOT) cb_velocity->setCurrentIndex(1);
        else if(velocity==BIKE) cb_velocity->setCurrentIndex(2);
        return;
    }
    if(is_anting) {
        QMessageBox::warning(this, "Warning!", "在多点寻路时无法更改速度，请稍后重试!", QMessageBox::Yes);
        if(velocity==STOP) cb_velocity->setCurrentIndex(0);
        else if(velocity==FOOT) cb_velocity->setCurrentIndex(1);
        else if(velocity==BIKE) cb_velocity->setCurrentIndex(2);
        return;
    }
    switch(x) {
        case 0:
            //记录日志文件
            log->addItem(*clock, SET_V, "设置速度为："+QString::number(STOP));

            //记录停止之前的速度
            before_v = velocity;
            velocity = STOP;
            //强行停下current
            current->set_velocity(velocity);
            return;
        case 1:
            //记录日志文件
            log->addItem(*clock, SET_V, "设置速度为："+QString::number(FOOT));

            //更改mw的速度，但只有在有导航的时候才会应用到current
            velocity = FOOT;
            break;
        case 2:
            //记录日志文件
            log->addItem(*clock, SET_V, "设置速度为："+QString::number(BIKE));

            velocity = BIKE;
            break;
    }

    //如果在行进中（导航中），要重新规划
    //以当前位置为起点
    if(current->get_next_ver()!=nullptr) {
        //如果回到之前的速度，直接更改速度继续走就行
        if(current->get_velocity()==STOP && before_v==velocity) {
            current->set_velocity(before_v);
            return;
        }

        //否则都得重新规划
        findpath_end = current->get_path().back()->get_no();
        //在路中间
        if(current->get_current_pass()!=0) {
            dij_findpath_dis.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                        findpath_end,Pr_Distance,velocity,
                                        current->get_current_pass(),current->get_current_dis()-current->get_current_pass());
            dij_findpath_time.doubleFind(current->get_last_ver()->get_no(),current->get_next_ver()->get_no(),
                                        findpath_end,Pr_Time,velocity,
                                        current->get_current_pass(),current->get_current_dis()-current->get_current_pass());
        }
        //在当前位置的节点上
        else if(findpath_begin==-1 && current->get_current_pass()==0) {
            dij_findpath_dis.singleFind(current->get_last_ver()->get_no(),findpath_end,Pr_Distance,velocity);
            dij_findpath_time.singleFind(current->get_last_ver()->get_no(),findpath_end,Pr_Time,velocity);
        }

        //更改当前状态，开始导航
        if(current->get_priority()==Pr_Distance) {
            setCurrent(dij_findpath_dis.get_dij_path(),dij_findpath_dis.get_total_dis(),dij_findpath_dis.get_total_time(),Pr_Distance);
        } else {
            setCurrent(dij_findpath_time.get_dij_path(),dij_findpath_time.get_total_dis(),dij_findpath_dis.get_total_time(),Pr_Distance);
        }
    }
}

//直接画出完整的路径
void MainWindow::drawPath(const QVector<Vertex*> &path, bool currentIn) {
    if(path.isEmpty()) return;
    if((!inShahe(path[0]->get_no()) && campus==Cp_Shahe) ||
            (!inBenbu(path[0]->get_no()) && campus==Cp_Benbu) ||
            (!inBetween(path[0]->get_no()) && campus==Cp_Between))
        return;
    //新创建一个item
    QGraphicsPathItem *path_item = new QGraphicsPathItem;
    QPen pen;
    pen.setWidth (4);
    pen.setColor (Qt::red);
    path_item->setPen (pen);
    path_item->setFlag (QGraphicsItem::ItemIsPanel);
    //加入场景
    scene->addItem (path_item);

    QPainterPath painter_path;
    //设置路径的起点
    if(currentIn) painter_path.moveTo(current->get_x(), current->get_y());
    else painter_path.moveTo (path[0]->get_x(), path[0]->get_y());
    //将路径用坐标存入到路径
    for (int i = 0; i < path.size() ; i++) {
        //下一个点的坐标
        painter_path.lineTo(path[i]->get_x(), path[i]->get_y());
    }
    path_item->setPath(painter_path);
}

//即使当前显示地图不匹配，后台也照样在推进该地图上的路线，只是不显示罢了
//当前点和导航路线点都要绑定current
void MainWindow::move() {
    //没目标时不移动
    if(current->get_velocity()==STOP) return;
    //等车时不移动
    if(!current->get_path().isEmpty() && (current->get_path()==dij_second_bus.get_dij_path()||
        current->get_path()==dij_second_shuttle.get_dij_path()) && wait_sec>0) {
        wait_sec--;
        return;
    }
    //当前速度 = 速度 * 拥挤度
    qreal left_dis = current->get_velocity() * adj_map[current->get_last_ver()->get_no()][current->get_next_ver()->get_no()]->get_crowdness();
    qreal v = current->get_velocity() * adj_map[current->get_last_ver()->get_no()][current->get_next_ver()->get_no()]->get_crowdness();
    qreal t = 1;

    //越过(或刚好到达)下一节点
    while(current->get_current_pass()+left_dis >= current->get_current_dis() && current->get_velocity()!=STOP) {
        //下一节点是终点，并且能到达(不用继续走了)
        if(current->get_next_ver_index()==current->get_path().size()-1) {
            //如果多点寻路全程结束
            if(is_anting && current->get_path()==ant_path_benbu) {
                is_anting = false;
                cross = false;
            }
            //如果两点寻路跨校区全程结束
            if(cross && ((current->get_path()==dij_dis_third.get_dij_path() && current->get_priority()==Pr_Distance) ||
                    (current->get_path()==dij_time_third.get_dij_path() && current->get_priority()==Pr_Time))) {
                cross = false;
            }

            //来到沙河（此时current属于between：inBetween==true）
            if(current->get_current_dis()==0 && current->get_next_ver()
                    !=nullptr && current->get_next_ver()->get_no()==SHAHE_BUS_VER_NO) {
                //切换地图（不保留当前点）
                switchShahe();
                //加回当前点
                scene->addItem(current_point);
                //初始化shahe的path导航线路起点
                shahe_painter_path->moveTo(vertices[SHAHE_BUS_VER_NO]->get_x(),vertices[SHAHE_BUS_VER_NO]->get_y());
            }
            //来到本部（此时current属于between：inBetween==true）
            else if(current->get_current_dis()==0 && current->get_next_ver()
                    !=nullptr && current->get_next_ver()->get_no()==BENBU_BUS_VER_NO) {
                //切换地图（不保留当前点）
                switchBenbu();
                //加回当前点
                scene->addItem(current_point);
                //初始化benbu的path导航线路起点
                benbu_painter_path->moveTo(vertices[BENBU_BUS_VER_NO]->get_x(),vertices[BENBU_BUS_VER_NO]->get_y());
            }

            current->set_last_ver(current->get_next_ver());
            current->set_next_ver_index(-1);
            current->set_velocity(STOP);
            current->set_total_pass(current->get_total_dis());
            current->set_total_pass_time(current->get_total_time());
            current->set_current_pass(0);
            current->set_current_dis(0);
            current->set_x(current->get_last_ver()->get_x());
            current->set_y(current->get_last_ver()->get_y());

            //清除路线
            delete shahe_painter_path;
            delete between_painter_path;
            delete benbu_painter_path;
            shahe_painter_path = new QPainterPath;
            between_painter_path = new QPainterPath;
            benbu_painter_path = new QPainterPath;
            //自动切换校园地图，初始化导航路线
            if(current->inShahe()) {
                shahe_painter_path->moveTo(current->get_x(),current->get_y());
            } else if(current->inBenbu()) {
                benbu_painter_path->moveTo(current->get_x(),current->get_y());
            } else {
                between_painter_path->moveTo(current->get_x(),current->get_y());
            }
        }
        //没到终点
        else {
            //如果current_dis==0表示正在跨地图（特别处理）
            if(current->get_current_dis()==0) {
                //离开沙河（此时current属于between：inBetween==true）
                if(current->get_last_ver()->get_no()==SHAHE_BUS_VER_NO) {
                    //切换地图（会保留当前点）
                    switchBetween();
                    //初始化between的path导航线路起点
                    between_painter_path->moveTo(vertices[BETWEEN_MIN_NO]->get_x(),vertices[BETWEEN_MIN_NO]->get_y());
                }
                //离开本部（此时current属于between：inBetween==true）
                else if(current->get_last_ver()->get_no()==BENBU_BUS_VER_NO) {
                    //切换地图（会保留当前点）
                    switchBetween();
                    //初始化between的path导航线路起点
                    between_painter_path->moveTo(vertices[BETWEEN_MAX_NO]->get_x(),vertices[BETWEEN_MAX_NO]->get_y());
                }
                //来到沙河（此时current属于between：inBetween==true）
                else if(current->get_next_ver()!=nullptr && current->get_next_ver()->get_no()==SHAHE_BUS_VER_NO) {
                    //切换地图（不保留当前点）
                    switchShahe();
                    //加回当前点
                    scene->addItem(current_point);
                    //初始化shahe的path导航线路起点
                    shahe_painter_path->moveTo(vertices[SHAHE_BUS_VER_NO]->get_x(),vertices[SHAHE_BUS_VER_NO]->get_y());
                }
                //来到本部（此时current属于between：inBetween==true）
                else if(current->get_next_ver()!=nullptr && current->get_next_ver()->get_no()==BENBU_BUS_VER_NO) {
                    //切换地图（不保留当前点）
                    switchBenbu();
                    //加回当前点
                    scene->addItem(current_point);
                    //初始化benbu的path导航线路起点
                    benbu_painter_path->moveTo(vertices[BENBU_BUS_VER_NO]->get_x(),vertices[BENBU_BUS_VER_NO]->get_y());
                }

            }   //dis==0的特别处理

            //t表示剩余的时间
            t -= (current->get_current_dis()-current->get_current_pass())/v;

            //修改当前节点（向前推进到下一节点）
            current->set_total_pass(current->get_total_pass() + current->get_current_dis()-current->get_current_pass());
            current->set_total_pass_time(current->get_total_pass_time()+(current->get_current_dis()-current->get_current_pass())/v);
            current->set_last_ver(current->get_next_ver());
            //不会有数组越界的情况，因为这个elseif的前提是next_ver_index<size-1
            current->set_next_ver_index(current->get_next_ver_index()+1);
            current->set_current_dis(adj_map[current->get_last_ver()->get_no()][current->get_next_ver()->get_no()]->get_distance());
            current->set_current_pass(0);

            current->set_x(current->get_last_ver()->get_x());
            current->set_y(current->get_last_ver()->get_y());

            //更新在新段的v的速度
            v = current->get_velocity() * adj_map[current->get_last_ver()->get_no()][current->get_next_ver()->get_no()]->get_crowdness();
            //left_dis表示按新段的拥挤度和剩余时间能走的距离
            left_dis = t * v;

            //有折线，连到此时的上一节点（如果刚切换完校区，则等于无操作）
            if(current->get_last_ver()->get_no()<=SHAHE_MAX_NO) {
                shahe_painter_path->lineTo(current->get_last_ver()->get_x(),current->get_last_ver()->get_y());
//                shahe_painter_path->moveTo(current->get_last_ver()->get_x(),current->get_last_ver()->get_y());
            } else if(current->get_last_ver()->get_no()<=BENBU_MAX_NO && current->get_last_ver()->get_no()>=BENBU_BUS_VER_NO) {
                benbu_painter_path->lineTo(current->get_last_ver()->get_x(),current->get_last_ver()->get_y());
//                benbu_painter_path->moveTo(current->get_last_ver()->get_x(),current->get_last_ver()->get_y());
            } else {
                between_painter_path->lineTo(current->get_last_ver()->get_x(),current->get_last_ver()->get_y());
//                between_painter_path->moveTo(current->get_last_ver()->get_x(),current->get_last_ver()->get_y());
            }
        }
    }//while

    //如果到终点了就不用执行
    if(current->get_next_ver()!=nullptr) {
        //最后推进当前位置(不用到下一段路)
        current->set_total_pass(current->get_total_pass() + left_dis);
        current->set_total_pass_time(current->get_total_pass_time() + t);
        current->set_current_pass(current->get_current_pass() + left_dis);

        current->set_x(current->get_last_ver()->get_x() + (current->get_next_ver()->get_x()-current->get_last_ver()->get_x())*(current->get_current_pass()/current->get_current_dis()));
        current->set_y(current->get_last_ver()->get_y() + (current->get_next_ver()->get_y()-current->get_last_ver()->get_y())*(current->get_current_pass()/current->get_current_dis()));
    }

    //无折线，直接连线
    if(current->inShahe()) {
        shahe_painter_path->lineTo(current->get_x(),current->get_y());
//        shahe_painter_path->moveTo(current->get_x(),current->get_y());
    } else if(current->inBenbu()) {
        benbu_painter_path->lineTo(current->get_x(),current->get_y());
//        benbu_painter_path->moveTo(current->get_x(),current->get_y());
    } else {
        between_painter_path->lineTo(current->get_x(),current->get_y());
//        between_painter_path->moveTo(current->get_x(),current->get_y());
    }
}

void MainWindow::clock_slot() {
    currentOutput();
    updateTime();
    //如果current是STOP的，不会move
    move();
    //current_point跟随current移动，校区是否匹配只决定其是否显示
    current_point->setPos(current->get_x(), current->get_y());

    if(campus==Cp_Shahe) main_path_item->setPath(*shahe_painter_path);
    else if(campus==Cp_Between) main_path_item->setPath(*between_painter_path);
    else main_path_item->setPath(*benbu_painter_path);

    //如果当前在跨校区且在分段路径的终点，要更新当前状态（分段寻路）
    if(cross && !current->get_path().isEmpty() && current->get_next_ver()==nullptr) {
        //距离优先
        if(current->get_priority()==Pr_Distance) {
            //如果当前不是多点寻路（即为二点寻路）
            if(is_anting==false) {
                //判断下一段应该是哪一段
                //当前走完第一段，则下一段是第二段（校区间）
                if(current->get_path()==dij_dis_first.get_dij_path()) {
                    setCurrent(dij_dis_second.get_dij_path(),dij_dis_second.get_total_dis(),dij_dis_second.get_total_time(),Pr_Distance);
                    //setCurrent会先设速度为velocity，之后需要覆盖它（改为bus/shuttle）
                    //current的velocity和mainwindow的可能不同，current的速度才是行进的速度
                    current->set_velocity(cross_dis_v);
                }
                //当前走完第二段，则下一段应该是第三段
                else if(current->get_path()==dij_dis_second.get_dij_path()) {
                    setCurrent(dij_dis_third.get_dij_path(),dij_dis_third.get_total_dis(),dij_dis_third.get_total_time(),Pr_Distance);
                }
                //当前走完第三段，即全部走完了（啥也不干）
            }
            //如果是多点寻路
            else {
                //当前是第一段
                if(current->get_path()==ant_path_shahe) {
                    setCurrent(dij_dis_second.get_dij_path(),dij_dis_second.get_total_dis(),dij_dis_second.get_total_time(),Pr_Distance);
                    current->set_velocity(cross_dis_v);
                } else if(current->get_path()==dij_dis_second.get_dij_path()) {
                    setCurrent(ant_path_benbu, ant_benbu_d, ant_benbu_t, Pr_Distance);
                }
            }
        }
        //时间优先
        else {
            if(is_anting==false) {
                //判断下一段应该是哪一段
                //当前走完第一段，则下一段是第二段（校区间）
                if(current->get_path()==dij_time_first.get_dij_path()) {
                    setCurrent(dij_time_second.get_dij_path(),dij_time_second.get_total_dis(),dij_time_second.get_total_time(),Pr_Time);
                    //setCurrent会先设速度为velocity，之后需要覆盖它（改为bus/shuttle）
                    current->set_velocity(cross_time_v);
                }
                //当前走完第二段，则下一段应该是第三段
                else if(current->get_path()==dij_time_second.get_dij_path()) {
                    setCurrent(dij_time_third.get_dij_path(),dij_time_third.get_total_dis(),dij_time_third.get_total_time(),Pr_Time);
                }
                //当前走完第三段，即全部走完了（啥也不干）
            } else {
                //当前是第一段
                if(current->get_path()==ant_path_shahe) {
                    setCurrent(dij_time_second.get_dij_path(),dij_time_second.get_total_dis(),dij_time_second.get_total_time(),Pr_Time);
                    current->set_velocity(cross_dis_v);
                } else if(current->get_path()==dij_time_second.get_dij_path()) {
                    setCurrent(ant_path_benbu, ant_benbu_d, ant_benbu_t, Pr_Time);
                }
            }
        }
    }
    //需要加速的时间减少
    if(goal_sec>0) {
        goal_sec--;
    } else if(goal_sec==0) {
        tmr_clock->start(167);
    }
}

//清空地图上的所有路线，会保留地图、当前点（如果有）、路线item
void MainWindow::clear() {
    QList<QGraphicsItem*> listItem = scene->items();
    //清除所有图元
    for(int i = 0; i<listItem.size(); i++)
    {
        if(listItem[i]==map_item || listItem[i]==current_point || listItem[i]==main_path_item)
            continue;
        scene->removeItem (listItem[i]);
        delete listItem[i];
        listItem.removeAt(i);
        i--;
    }
}

//文字输出当前状态
void MainWindow::currentOutput() {
    //正在行进中
    if(current->get_velocity()!=STOP) {
        lb_current->setText("距当前终点： "+current->get_path().back()->get_description()+" 还剩 "+
                QString::number(current->get_total_dis()-current->get_total_pass())+" 米，还剩 "+
                QString::number(current->get_total_time()-current->get_total_pass_time())+" 秒");
    }
    //停下了
    else {
        if(current->get_current_pass()==0)
            lb_current->setText("当前不在行进中，位于 "+current->get_last_ver()->get_description());
        else
            lb_current->setText("当前不在行进中，位于从 "+current->get_last_ver()->get_description()+
                                " 到 "+current->get_next_ver()->get_description()+" 中距离 "+
                                current->get_last_ver()->get_description()+QString::number(current->get_current_pass())+" m处");
    }
    if(!current->get_path().isEmpty() && (current->get_path()==dij_second_bus.get_dij_path()||
        current->get_path()==dij_second_shuttle.get_dij_path()) && wait_sec>0) {
        lb_current->setText("当前正在等车，剩余等待时间："+QString::number(wait_sec)+"秒\n"+lb_current->text());
    }
}

//清空右侧按钮
void MainWindow::cancel() {
    //清除antfind的按钮
    glayout->removeWidget(lb_antfind_hint);
    glayout->removeWidget(le_antfind);
    glayout->removeWidget(pbt_antfind_next);
    glayout->removeWidget(pbt_antfind_start);
    lb_antfind_hint->setParent(nullptr);
    le_antfind->setParent(nullptr);
    pbt_antfind_next->setParent(nullptr);
    pbt_antfind_start->setParent(nullptr);

    glayout->removeWidget(lb_antfind_dis);
    glayout->removeWidget(lb_antfind_time);
    glayout->removeWidget(pbt_antfind_dis);
    glayout->removeWidget(pbt_antfind_time);
    lb_antfind_dis->setParent(nullptr);
    lb_antfind_time->setParent(nullptr);
    pbt_antfind_dis->setParent(nullptr);
    pbt_antfind_time->setParent(nullptr);

    //清除findpath的按钮，如果没有也不会有影响
    glayout->removeWidget(lb_findpath_dis);
    glayout->removeWidget(lb_findpath_time);
    glayout->removeWidget(pbt_findpath_dis);
    glayout->removeWidget(pbt_findpath_time);
    lb_findpath_dis->setParent(nullptr);
    lb_findpath_time->setParent(nullptr);
    pbt_findpath_dis->setParent(nullptr);
    pbt_findpath_time->setParent(nullptr);

    glayout->removeWidget(lb_fuzzysearch);
    glayout->removeWidget(pbt_fuzzysearch_reenter);
    lb_fuzzysearch->setParent(nullptr);
    pbt_fuzzysearch_reenter->setParent(nullptr);

    glayout->removeWidget(lb_class);
    lb_class->setParent(nullptr);

    //pbt_cancel不用删除
    glayout->removeWidget(pbt_cancel);
    pbt_cancel->setParent(nullptr);

    //清除searchnear的按钮，如果没有也不会有影响
    while(lblist_searchnear.size()!=0) {
        glayout->removeWidget(lblist_searchnear[0]);
        delete lblist_searchnear[0];
        lblist_searchnear.removeAt(0);
    }
    while(pbtlist_searchnear.size()!=0) {
        glayout->removeWidget(pbtlist_searchnear[0]);
        delete pbtlist_searchnear[0];
        pbtlist_searchnear.removeAt(0);
    }

    //清除fuzzysearch的按钮
    while(lblist_fuzzysearch.size()!=0) {
        glayout->removeWidget(lblist_fuzzysearch[0]);
        delete lblist_fuzzysearch[0];
        lblist_fuzzysearch.removeAt(0);
    }
    while(pbtlist_fuzzysearch.size()!=0) {
        glayout->removeWidget(pbtlist_fuzzysearch[0]);
        delete pbtlist_fuzzysearch[0];
        pbtlist_fuzzysearch.removeAt(0);
    }

}

//清空右侧按钮，将fuzzysearch_res设为-1，等输入的槽函数将lineedit的text改变
void MainWindow::reenter() {
    log->addItem(*clock, REENTER, "清空按钮");

    fuzzysearch_res = -1;
    le_search->setText("");
    le_begin->setText("");
    le_end->setText("");
    le_antfind->setText("");

    search_end = -2;
    findpath_begin = -2;
    findpath_end = -2;
    ant_spot = -2;

    //这两个都不用删除
    glayout->removeWidget(lb_fuzzysearch);
    glayout->removeWidget(pbt_fuzzysearch_reenter);
    lb_fuzzysearch->setParent(nullptr);
    pbt_fuzzysearch_reenter->setParent(nullptr);

    while(lblist_fuzzysearch.size()!=0) {
        glayout->removeWidget(lblist_fuzzysearch[0]);
        delete lblist_fuzzysearch[0];
        lblist_fuzzysearch.removeAt(0);
    }
    while(pbtlist_fuzzysearch.size()!=0) {
        glayout->removeWidget(pbtlist_fuzzysearch[0]);
        delete pbtlist_fuzzysearch[0];
        pbtlist_fuzzysearch.removeAt(0);
    }
}

//选择路线后的槽函数,改变当前状态，清空右侧标签和按钮，新建导航路线
void MainWindow::choose_findpath_dis() {
    //如果要跨校区
    if(cross) {
        //设置当前状态开始第一段的导航
        setCurrent(dij_dis_first.get_dij_path(),dij_dis_first.get_total_dis(),dij_dis_first.get_total_time(),Pr_Distance);
    } else {
        setCurrent(dij_findpath_dis.get_dij_path(),dij_findpath_dis.get_total_dis(),dij_findpath_dis.get_total_time(),Pr_Distance);
    }
    //清空按钮
    cancel();

    //更改导航路线（删除旧的，创建新的）
    delete shahe_painter_path;
    delete between_painter_path;
    delete benbu_painter_path;
    shahe_painter_path = new QPainterPath;
    between_painter_path = new QPainterPath;
    benbu_painter_path = new QPainterPath;

    //自动切换校园地图，初始化导航路线
    if(current->inShahe()) {
        switchShahe();
        //设置线路起点
        shahe_painter_path->moveTo(current->get_x(),current->get_y());
    } else if(current->inBenbu()) {
        switchBenbu();
        benbu_painter_path->moveTo(current->get_x(),current->get_y());
    } else {
        switchBetween();
        between_painter_path->moveTo(current->get_x(),current->get_y());
    }

    //时间恢复流动
    tmr_clock->start();
}

void MainWindow::choose_findpath_time() {
    //如果要跨校区
    if(cross) {
        //设置当前状态开始第一段的导航
        setCurrent(dij_time_first.get_dij_path(),dij_time_first.get_total_dis(),dij_time_first.get_total_time(),Pr_Time);
    } else {
        setCurrent(dij_findpath_time.get_dij_path(),dij_findpath_time.get_total_dis(),dij_findpath_time.get_total_time(),Pr_Time);
    }
    //清空按钮
    cancel();

    //更改导航路线（删除旧的，创建新的）
    delete shahe_painter_path;
    delete between_painter_path;
    delete benbu_painter_path;
    shahe_painter_path = new QPainterPath;
    between_painter_path = new QPainterPath;
    benbu_painter_path = new QPainterPath;

    //自动切换校园地图，初始化导航路线
    if(current->inShahe()) {
        //switch中包含显示该线路
        switchShahe();
        //设置线路起点
        shahe_painter_path->moveTo(current->get_x(),current->get_y());
    } else if(current->inBenbu()) {
        switchBenbu();
        benbu_painter_path->moveTo(current->get_x(),current->get_y());
    } else {
        switchBetween();
        between_painter_path->moveTo(current->get_x(),current->get_y());
    }

    //时间恢复流动
    tmr_clock->start();
}

//根据选择的路径，改变当前状态，新建导航路线
void MainWindow::choose_searchnear(int i) {
    //下标对应到顶点编号
    int no = list_to_no[i];
    QVector<Vertex*> temp_path = dij_searchnear.get_paths()[no];
    int temp_dis = dij_searchnear.get_dis()[no];
    int temp_time = dij_searchnear.get_time()[no];

    setCurrent(temp_path,temp_dis,temp_time,Pr_Distance);
    //清除所有按钮
    cancel();

    //更改导航路线（删除旧的，创建新的）
    delete shahe_painter_path;
    delete between_painter_path;
    delete benbu_painter_path;
    shahe_painter_path = new QPainterPath;
    between_painter_path = new QPainterPath;
    benbu_painter_path = new QPainterPath;

    //自动切换校园地图，初始化导航路线
    if(current->inShahe()) {
        //switch中包含显示该线路
        switchShahe();
        //设置线路起点
        shahe_painter_path->moveTo(current->get_x(),current->get_y());
    } else if(current->inBenbu()) {
        switchBenbu();
        benbu_painter_path->moveTo(current->get_x(),current->get_y());
    } else {
        switchBetween();
        between_painter_path->moveTo(current->get_x(),current->get_y());
    }

    //时间恢复流动
    tmr_clock->start();
}

//存储本次输入的地点，继续下一个地点的输入
void MainWindow::antfindNext() {
    if(ant_spot==-2) {
        //输出warning
        QMessageBox::warning(this, "Warning!", "地点不能为空!", QMessageBox::Yes);
        return;
    }

    //把本次输入的地点加入ant_spots
    ant_spots.append(ant_spot);

    //沙河的点加入shahe_spot
    if(inShahe(ant_spot)) {
        have_shahe = true;
        ant_shahe_spots_bus.append(ant_spot);
        ant_shahe_spots_shuttle.append(ant_spot);
    }
    else if(inBenbu(ant_spot)) {
        have_benbu = true;
        ant_benbu_spots_bus.append(ant_spot);
        ant_benbu_spots_shuttle.append(ant_spot);
    }

    //清空lineedit
    ant_spot = -2;
    le_antfind->setText("");
}

//存储最后一次输入的地点，开始寻路
void MainWindow::antfindStart() {
    if(ant_spot==-2) {
        //输出warning
        QMessageBox::warning(this, "Warning!", "地点不能为空!", QMessageBox::Yes);
        return;
    }
    //如果只有一个地点，还多点寻个der啊
    if(ant_spots.isEmpty()) {
        QMessageBox::warning(this, "Warning!", "地点不能只有一个!", QMessageBox::Yes);
        return;
    }

    //正在多点寻路的标志
    is_anting = true;

    //把本次输入的地点加入ant_spots
    ant_spots.append(ant_spot);
    //沙河的点加入shahe_spot
    if(inShahe(ant_spot)) {
        have_shahe = true;
        ant_shahe_spots_bus.append(ant_spot);
        ant_shahe_spots_shuttle.append(ant_spot);
    }
    else if(inBenbu(ant_spot)) {
        have_benbu = true;
        ant_benbu_spots_bus.append(ant_spot);
        ant_benbu_spots_shuttle.append(ant_spot);
    }

    //初始化ant_cross
    if(have_benbu && have_shahe) cross = true;
    else cross = false;

    //生成抽象完全图
    //首先初始化
    abmap_dis.clear();
    abmap_time.clear();
    abmap_dis_bus.clear();
    abmap_dis_shuttle.clear();
    abmap_time_bus.clear();
    abmap_time_shuttle.clear();

    ant_path_dis.clear();
    ant_path_time.clear();

    //如果不用跨校区
    if(cross==false) {
        //遍历antspots
        for(int i = 0; i<ant_spots.size(); i++) {
            for(int j = 0; j<ant_spots.size(); j++) {
                Dijkstra dij;
                dij.set_adj_map(adj_map);
                dij.set_vertices(vertices);

                dij.singleFind(ant_spots[i], ant_spots[j], Pr_Distance, velocity);
                abmap_dis[ant_spots[i]][ant_spots[j]] = dij;

                dij.singleFind(ant_spots[i], ant_spots[j], Pr_Time, velocity);
                abmap_time[ant_spots[i]][ant_spots[j]] = dij;
            }
        }
        ant_dis.set_abmap(abmap_dis);
        ant_time.set_abmap(abmap_time);

        //开始寻路
        ant_dis.start(ant_spots, Pr_Distance);
        ant_time.start(ant_spots, Pr_Time);

        for(int i = 1; i<ant_dis.get_min_rout_inall().size(); i++) {
            if(!ant_path_dis.isEmpty()) ant_path_dis.pop_back();
            for(auto ver : abmap_dis[ant_dis.get_min_rout_inall()[i-1]][ant_dis.get_min_rout_inall()[i]].get_dij_path()) {
                ant_path_dis.append(ver);
            }
        }

        for(int i = 1; i<ant_time.get_min_rout_inall().size(); i++) {
            if(!ant_path_time.isEmpty()) ant_path_time.pop_back();
            for(auto ver : abmap_time[ant_time.get_min_rout_inall()[i-1]][ant_time.get_min_rout_inall()[i]].get_dij_path()) {
                ant_path_time.append(ver);
            }
        }
    }
    //要跨校区，跨校区路段借用dij_second_bus/shuttle和cross_dis/time_v
    else {
        if(ant_shahe_spots_bus.count(SHAHE_BUS_VER_NO)==0)
            ant_shahe_spots_bus.append(SHAHE_BUS_VER_NO);
        if(ant_shahe_spots_shuttle.count(SHAHE_SHUTTLE_VER_NO)==0)
            ant_shahe_spots_shuttle.append(SHAHE_SHUTTLE_VER_NO);
        if(ant_benbu_spots_bus.count(BENBU_BUS_VER_NO)==0)
            ant_benbu_spots_bus.append(BENBU_BUS_VER_NO);
        if(ant_benbu_spots_shuttle.count(BENBU_SHUTTLE_VER_NO)==0)
            ant_benbu_spots_shuttle.append(BENBU_SHUTTLE_VER_NO);

        //shuttle和bus的点个数可能不同

        //遍历ant_shahe_spots_bus
        for(int i = 0; i<ant_shahe_spots_bus.size(); i++) {
            for(int j = 0; j<ant_shahe_spots_bus.size(); j++) {
                Dijkstra dij;
                dij.set_adj_map(adj_map);
                dij.set_vertices(vertices);

                dij.singleFind(ant_shahe_spots_bus[i], ant_shahe_spots_bus[j], Pr_Distance, velocity);
                abmap_dis_bus[ant_shahe_spots_bus[i]][ant_shahe_spots_bus[j]] = dij;

                dij.singleFind(ant_shahe_spots_bus[i], ant_shahe_spots_bus[j], Pr_Time, velocity);
                abmap_time_bus[ant_shahe_spots_bus[i]][ant_shahe_spots_bus[j]] = dij;

            }
        }
        qDebug()<<"shahe bus abmap done"<<endl;

        //遍历ant_shahe_spots_shuttle
        for(int i = 0; i<ant_shahe_spots_shuttle.size(); i++) {
            for(int j = 0; j<ant_shahe_spots_shuttle.size(); j++) {
                Dijkstra dij;
                dij.set_adj_map(adj_map);
                dij.set_vertices(vertices);

                dij.singleFind(ant_shahe_spots_shuttle[i], ant_shahe_spots_shuttle[j], Pr_Distance, velocity);
                abmap_dis_shuttle[ant_shahe_spots_shuttle[i]][ant_shahe_spots_shuttle[j]] = dij;

                dij.singleFind(ant_shahe_spots_shuttle[i], ant_shahe_spots_shuttle[j], Pr_Time, velocity);
                abmap_time_shuttle[ant_shahe_spots_shuttle[i]][ant_shahe_spots_shuttle[j]] = dij;
            }
        }
        qDebug()<<"shahe shuttle abmap done"<<endl;

        //遍历ant_benbu_spots_bus
        for(int i = 0; i<ant_benbu_spots_bus.size(); i++) {
            for(int j = 0; j<ant_benbu_spots_bus.size(); j++) {
                Dijkstra dij;
                dij.set_adj_map(adj_map);
                dij.set_vertices(vertices);

                dij.singleFind(ant_benbu_spots_bus[i], ant_benbu_spots_bus[j], Pr_Distance, velocity);
                abmap_dis_bus[ant_benbu_spots_bus[i]][ant_benbu_spots_bus[j]] = dij;

                dij.singleFind(ant_benbu_spots_bus[i], ant_benbu_spots_bus[j], Pr_Time, velocity);
                abmap_time_bus[ant_benbu_spots_bus[i]][ant_benbu_spots_bus[j]] = dij;
            }
        }
        qDebug()<<"benbu bus abmap done"<<endl;

        //遍历ant_benbu_spots_shuttle
        for(int i = 0; i<ant_benbu_spots_shuttle.size(); i++) {
            for(int j = 0; j<ant_benbu_spots_shuttle.size(); j++) {
                Dijkstra dij;
                dij.set_adj_map(adj_map);
                dij.set_vertices(vertices);

                dij.singleFind(ant_benbu_spots_shuttle[i], ant_benbu_spots_shuttle[j], Pr_Distance, velocity);
                abmap_dis_shuttle[ant_benbu_spots_shuttle[i]][ant_benbu_spots_shuttle[j]] = dij;

                dij.singleFind(ant_benbu_spots_shuttle[i], ant_benbu_spots_shuttle[j], Pr_Time, velocity);
                abmap_time_shuttle[ant_benbu_spots_shuttle[i]][ant_benbu_spots_shuttle[j]] = dij;
            }
        }
        qDebug()<<"benbu shuttle abmap done"<<endl;

        //bus
        for(int i = 0; i<ant_shahe_spots_bus.size(); i++) {
            for(int j = 0; j<ant_benbu_spots_bus.size(); j++) {
                Dijkstra dij_dis;
                Dijkstra dij_time;
                dij_dis.set_adj_map(adj_map);
                dij_dis.set_vertices(vertices);
                dij_time.set_adj_map(adj_map);
                dij_time.set_vertices(vertices);

                //从i到j(dis)
                dij_dis.set_begin(ant_shahe_spots_bus[i]);
                dij_dis.set_end(ant_benbu_spots_bus[j]);
                dij_dis.set_total_dis(abmap_dis_bus[ant_shahe_spots_bus[i]][SHAHE_BUS_VER_NO].get_total_dis()+
                                  abmap_dis_bus[BENBU_BUS_VER_NO][ant_benbu_spots_bus[j]].get_total_dis()+
                                  bus_total_dis);
                dij_dis.set_total_time(abmap_dis_bus[ant_shahe_spots_bus[i]][SHAHE_BUS_VER_NO].get_total_time()+
                                   abmap_dis_bus[BENBU_BUS_VER_NO][ant_benbu_spots_bus[j]].get_total_time()+
                                   bus_total_time);
                QVector<Vertex*> tmp1 = abmap_dis_bus[ant_shahe_spots_bus[i]][SHAHE_BUS_VER_NO].get_dij_path();
                tmp1.pop_back();
                for(auto no : S2B_BUS_PATH_NO) {
                    tmp1.append(vertices[no]);
                }
                tmp1.pop_back();
                tmp1.append(abmap_dis_bus[BENBU_BUS_VER_NO][ant_benbu_spots_bus[j]].get_dij_path());
                dij_dis.set_dij_path(tmp1);
                abmap_dis_bus[ant_shahe_spots_bus[i]][ant_benbu_spots_bus[j]] = dij_dis;

                qDebug()<<"i("<<i<<") to j("<<j<<") dis"<<endl;

                //从j到i(dis)
                dij_dis.set_begin(ant_benbu_spots_bus[j]);
                dij_dis.set_end(ant_shahe_spots_bus[i]);
                dij_dis.set_total_dis(abmap_dis_bus[ant_benbu_spots_bus[j]][BENBU_BUS_VER_NO].get_total_dis()+
                                  abmap_dis_bus[SHAHE_BUS_VER_NO][ant_shahe_spots_bus[i]].get_total_dis()+
                                  bus_total_dis);
                dij_dis.set_total_time(abmap_dis_bus[ant_benbu_spots_bus[j]][BENBU_BUS_VER_NO].get_total_time()+
                                   abmap_dis_bus[SHAHE_BUS_VER_NO][ant_shahe_spots_bus[i]].get_total_time()+
                                   bus_total_time);
                QVector<Vertex*> tmp2 = abmap_dis_bus[ant_benbu_spots_bus[j]][BENBU_BUS_VER_NO].get_dij_path();
                tmp2.pop_back();
                for(int k = S2B_BUS_PATH_NO.size()-1; k>=0; k--) {
                    tmp2.append(vertices[S2B_BUS_PATH_NO[k]]);
                }
                tmp2.pop_back();
                tmp2.append(abmap_dis_bus[SHAHE_BUS_VER_NO][ant_shahe_spots_bus[i]].get_dij_path());
                dij_dis.set_dij_path(tmp2);
                abmap_dis_bus[ant_benbu_spots_bus[j]][ant_shahe_spots_bus[i]] = dij_dis;

                qDebug()<<"j("<<j<<") to i("<<i<<") dis"<<endl;

                //从i到j(time)
                dij_time.set_begin(ant_shahe_spots_bus[i]);
                dij_time.set_end(ant_benbu_spots_bus[j]);
                dij_time.set_total_dis(abmap_time_bus[ant_shahe_spots_bus[i]][SHAHE_BUS_VER_NO].get_total_dis()+
                                  abmap_time_bus[BENBU_BUS_VER_NO][ant_benbu_spots_bus[j]].get_total_dis()+
                                  bus_total_dis);
                dij_time.set_total_time(abmap_time_bus[ant_shahe_spots_bus[i]][SHAHE_BUS_VER_NO].get_total_time()+
                                   abmap_time_bus[BENBU_BUS_VER_NO][ant_benbu_spots_bus[j]].get_total_time()+
                                   bus_total_time);
                QVector<Vertex*> tmp3 = abmap_time_bus[ant_shahe_spots_bus[i]][SHAHE_BUS_VER_NO].get_dij_path();
                tmp3.pop_back();
                for(auto no : S2B_BUS_PATH_NO) {
                    tmp3.append(vertices[no]);
                }
                tmp3.pop_back();
                tmp3.append(abmap_time_bus[BENBU_BUS_VER_NO][ant_benbu_spots_bus[j]].get_dij_path());
                dij_time.set_dij_path(tmp3);
                abmap_time_bus[ant_shahe_spots_bus[i]][ant_benbu_spots_bus[j]] = dij_time;

                qDebug()<<"i("<<i<<") to j("<<j<<") time"<<endl;

                //从j到i(time)
                dij_time.set_begin(ant_benbu_spots_bus[j]);
                dij_time.set_end(ant_shahe_spots_bus[i]);
                dij_time.set_total_dis(abmap_time_bus[ant_benbu_spots_bus[j]][BENBU_BUS_VER_NO].get_total_dis()+
                                  abmap_time_bus[SHAHE_BUS_VER_NO][ant_shahe_spots_bus[i]].get_total_dis()+
                                  bus_total_dis);
                dij_time.set_total_time(abmap_time_bus[ant_benbu_spots_bus[j]][BENBU_BUS_VER_NO].get_total_time()+
                                   abmap_time_bus[SHAHE_BUS_VER_NO][ant_shahe_spots_bus[i]].get_total_time()+
                                   bus_total_time);
                QVector<Vertex*> tmp4 = abmap_time_bus[ant_benbu_spots_bus[j]][BENBU_BUS_VER_NO].get_dij_path();
                tmp4.pop_back();
                for(int k = S2B_BUS_PATH_NO.size()-1; k>=0; k--) {
                    tmp4.append(vertices[S2B_BUS_PATH_NO[k]]);
                }
                tmp4.pop_back();
                tmp4.append(abmap_time_bus[SHAHE_BUS_VER_NO][ant_shahe_spots_bus[i]].get_dij_path());
                dij_time.set_dij_path(tmp4);
                abmap_time_bus[ant_benbu_spots_bus[j]][ant_shahe_spots_bus[i]] = dij_time;

                qDebug()<<"j("<<j<<") to i("<<i<<") time"<<endl;

            }
        }
        qDebug()<<"bus all cross abmap done"<<endl;

        //shuttle
        for(int i = 0; i<ant_shahe_spots_shuttle.size(); i++) {
            for(int j = 0; j<ant_benbu_spots_shuttle.size(); j++) {
                Dijkstra dij;
                dij.set_adj_map(adj_map);
                dij.set_vertices(vertices);

                //从i到j(dis)
                dij.set_begin(ant_shahe_spots_shuttle[i]);
                dij.set_end(ant_benbu_spots_shuttle[j]);
                dij.set_total_dis(abmap_dis_shuttle[ant_shahe_spots_shuttle[i]][SHAHE_SHUTTLE_VER_NO].get_total_dis()+
                                  abmap_dis_shuttle[BENBU_SHUTTLE_VER_NO][ant_benbu_spots_shuttle[j]].get_total_dis()+
                                  shuttle_total_dis);
                dij.set_total_time(abmap_dis_shuttle[ant_shahe_spots_shuttle[i]][SHAHE_SHUTTLE_VER_NO].get_total_time()+
                                   abmap_dis_shuttle[BENBU_SHUTTLE_VER_NO][ant_benbu_spots_shuttle[j]].get_total_time()+
                                   shuttle_total_time);
                QVector<Vertex*> tmp1 = abmap_dis_shuttle[ant_shahe_spots_shuttle[i]][SHAHE_SHUTTLE_VER_NO].get_dij_path();
                tmp1.pop_back();
                for(auto no : S2B_SHUTTLE_PATH_NO) {
                    tmp1.append(vertices[no]);
                }
                tmp1.pop_back();
                tmp1.append(abmap_dis_shuttle[BENBU_SHUTTLE_VER_NO][ant_benbu_spots_shuttle[j]].get_dij_path());
                dij.set_dij_path(tmp1);
                abmap_dis_shuttle[ant_shahe_spots_shuttle[i]][ant_benbu_spots_shuttle[j]] = dij;

                //从j到i(dis)
                dij.set_begin(ant_benbu_spots_shuttle[j]);
                dij.set_end(ant_shahe_spots_shuttle[i]);
                dij.set_total_dis(abmap_dis_shuttle[ant_benbu_spots_shuttle[j]][BENBU_SHUTTLE_VER_NO].get_total_dis()+
                                  abmap_dis_shuttle[SHAHE_SHUTTLE_VER_NO][ant_shahe_spots_shuttle[i]].get_total_dis()+
                                  bus_total_dis);
                dij.set_total_time(abmap_dis_shuttle[ant_benbu_spots_shuttle[j]][BENBU_SHUTTLE_VER_NO].get_total_time()+
                                   abmap_dis_shuttle[SHAHE_SHUTTLE_VER_NO][ant_shahe_spots_shuttle[i]].get_total_time()+
                                   bus_total_time);
                QVector<Vertex*> tmp2 = abmap_dis_shuttle[ant_benbu_spots_shuttle[j]][BENBU_SHUTTLE_VER_NO].get_dij_path();
                tmp2.pop_back();
                for(int k = S2B_SHUTTLE_PATH_NO.size()-1; k>=0; k--) {
                    tmp2.append(vertices[S2B_SHUTTLE_PATH_NO[k]]);
                }
                tmp2.pop_back();
                tmp2.append(abmap_dis_shuttle[SHAHE_SHUTTLE_VER_NO][ant_shahe_spots_shuttle[i]].get_dij_path());
                dij.set_dij_path(tmp2);
                abmap_dis_shuttle[ant_benbu_spots_shuttle[j]][ant_shahe_spots_shuttle[i]] = dij;

                //从i到j(time)
                dij.set_begin(ant_shahe_spots_shuttle[i]);
                dij.set_end(ant_benbu_spots_shuttle[j]);
                dij.set_total_dis(abmap_time_shuttle[ant_shahe_spots_shuttle[i]][SHAHE_SHUTTLE_VER_NO].get_total_dis()+
                                  abmap_time_shuttle[BENBU_SHUTTLE_VER_NO][ant_benbu_spots_shuttle[j]].get_total_dis()+
                                  shuttle_total_dis);
                dij.set_total_time(abmap_time_shuttle[ant_shahe_spots_shuttle[i]][SHAHE_SHUTTLE_VER_NO].get_total_time()+
                                   abmap_time_shuttle[BENBU_SHUTTLE_VER_NO][ant_benbu_spots_shuttle[j]].get_total_time()+
                                   shuttle_total_time);
                QVector<Vertex*> tmp3 = abmap_time_shuttle[ant_shahe_spots_shuttle[i]][SHAHE_SHUTTLE_VER_NO].get_dij_path();
                tmp3.pop_back();
                for(auto no : S2B_SHUTTLE_PATH_NO) {
                    tmp3.append(vertices[no]);
                }
                tmp3.pop_back();
                tmp3.append(abmap_time_shuttle[BENBU_SHUTTLE_VER_NO][ant_benbu_spots_shuttle[j]].get_dij_path());
                dij.set_dij_path(tmp3);
                abmap_time_shuttle[ant_shahe_spots_shuttle[i]][ant_benbu_spots_shuttle[j]] = dij;

                //从j到i(time)
                dij.set_begin(ant_benbu_spots_shuttle[j]);
                dij.set_end(ant_shahe_spots_shuttle[i]);
                dij.set_total_dis(abmap_time_shuttle[ant_benbu_spots_shuttle[j]][BENBU_SHUTTLE_VER_NO].get_total_dis()+
                                  abmap_time_shuttle[SHAHE_SHUTTLE_VER_NO][ant_shahe_spots_shuttle[i]].get_total_dis()+
                                  shuttle_total_dis);
                dij.set_total_time(abmap_time_shuttle[ant_benbu_spots_shuttle[j]][BENBU_SHUTTLE_VER_NO].get_total_time()+
                                   abmap_time_shuttle[SHAHE_SHUTTLE_VER_NO][ant_shahe_spots_shuttle[i]].get_total_time()+
                                   shuttle_total_time);
                QVector<Vertex*> tmp4 = abmap_time_shuttle[ant_benbu_spots_shuttle[j]][BENBU_SHUTTLE_VER_NO].get_dij_path();
                tmp4.pop_back();
                for(int k = S2B_SHUTTLE_PATH_NO.size()-1; k>=0; k--) {
                    tmp4.append(vertices[S2B_SHUTTLE_PATH_NO[k]]);
                }
                tmp4.pop_back();
                tmp4.append(abmap_time_shuttle[SHAHE_SHUTTLE_VER_NO][ant_shahe_spots_shuttle[i]].get_dij_path());
                dij.set_dij_path(tmp4);
                abmap_time_shuttle[ant_benbu_spots_shuttle[j]][ant_shahe_spots_shuttle[i]] = dij;
            }
        }
        qDebug()<<"shuttle all cross abmap done"<<endl;

        //为ant设置abmap
        ant_dis_bus.set_abmap(abmap_dis_bus);
        ant_dis_shuttle.set_abmap(abmap_dis_shuttle);
        ant_time_bus.set_abmap(abmap_time_bus);
        ant_time_shuttle.set_abmap(abmap_time_shuttle);

        //合并两个校区的spots
        ant_spots_bus = ant_shahe_spots_bus;
        ant_spots_bus.append(ant_benbu_spots_bus);
        ant_spots_shuttle = ant_shahe_spots_shuttle;
        ant_spots_shuttle.append(ant_benbu_spots_shuttle);

        //开始寻路
        ant_dis_bus.start(ant_spots_bus,Pr_Distance);
        ant_dis_shuttle.start(ant_spots_shuttle,Pr_Distance);
        ant_time_bus.start(ant_spots_bus,Pr_Time);
        ant_time_shuttle.start(ant_spots_shuttle,Pr_Time);

        qDebug()<<"4 antfind done"<<endl;

        //分割路段
        //首先初始化
        ant_path_shahe_dis_bus.clear();
        ant_path_shahe_time_bus.clear();
        ant_path_shahe_dis_shuttle.clear();
        ant_path_shahe_time_shuttle.clear();
        ant_path_benbu_dis_bus.clear();
        ant_path_benbu_time_bus.clear();
        ant_path_benbu_dis_shuttle.clear();
        ant_path_benbu_time_shuttle.clear();

        ant_shahe_d_dis_bus = 0;
        ant_shahe_d_time_bus = 0;
        ant_shahe_d_dis_shuttle = 0;
        ant_shahe_d_time_shuttle = 0;
        ant_benbu_d_dis_bus = 0;
        ant_benbu_d_time_bus = 0;
        ant_benbu_d_dis_shuttle = 0;
        ant_benbu_d_time_shuttle = 0;
        ant_shahe_t_dis_bus = 0;
        ant_shahe_t_time_bus = 0;
        ant_shahe_t_dis_shuttle = 0;
        ant_shahe_t_time_shuttle = 0;
        ant_benbu_t_dis_bus = 0;
        ant_benbu_t_time_bus = 0;
        ant_benbu_t_dis_shuttle = 0;
        ant_benbu_t_time_shuttle = 0;

        //rout中的都是在spots中点的编号（不包括dij中的点）
        QVector<Vertex*> tmp_rout_dis_bus;
        QVector<Vertex*> tmp_rout_dis_shuttle;
        QVector<Vertex*> tmp_rout_time_bus;
        QVector<Vertex*> tmp_rout_time_shuttle;

        for(int i = 1; i<ant_dis_bus.get_min_rout_inall().size(); i++) {
            if(!tmp_rout_dis_bus.isEmpty()) tmp_rout_dis_bus.pop_back();
            for(auto ver : abmap_dis_bus[ant_dis_bus.get_min_rout_inall()[i-1]][ant_dis_bus.get_min_rout_inall()[i]].get_dij_path()) {
                tmp_rout_dis_bus.append(ver);
            }
        }
        qDebug()<<"ant_dis_bus convert to path done"<<endl;

        for(int i = 1; i<ant_dis_shuttle.get_min_rout_inall().size(); i++) {
            if(!tmp_rout_dis_shuttle.isEmpty()) tmp_rout_dis_shuttle.pop_back();
            for(auto ver : abmap_dis_shuttle[ant_dis_shuttle.get_min_rout_inall()[i-1]][ant_dis_shuttle.get_min_rout_inall()[i]].get_dij_path()) {
                tmp_rout_dis_shuttle.append(ver);
            }
        }
        qDebug()<<"ant_dis_shuttle convert to path done"<<endl;

        for(int i = 1; i<ant_time_bus.get_min_rout_inall().size(); i++) {
            if(!tmp_rout_time_bus.isEmpty()) tmp_rout_time_bus.pop_back();
            for(auto ver : abmap_time_bus[ant_time_bus.get_min_rout_inall()[i-1]][ant_time_bus.get_min_rout_inall()[i]].get_dij_path()) {
                tmp_rout_time_bus.append(ver);
            }
        }
        qDebug()<<"ant_time_bus convert to path done"<<endl;

        for(int i = 1; i<ant_time_shuttle.get_min_rout_inall().size(); i++) {
            if(!tmp_rout_time_shuttle.isEmpty()) tmp_rout_time_shuttle.pop_back();
            for(auto ver : abmap_time_shuttle[ant_time_shuttle.get_min_rout_inall()[i-1]][ant_time_shuttle.get_min_rout_inall()[i]].get_dij_path()) {
                tmp_rout_time_shuttle.append(ver);
            }
        }
        qDebug()<<"ant_time_shuttle convert to path done"<<endl;

        //dis_bus
        //如果沙河在前半部分
        if(inShahe(tmp_rout_dis_bus[0]->get_no())) {
            //校门也算沙河
            for(int i = 0; inShahe(tmp_rout_dis_bus[i]->get_no()); i++) {
                ant_path_shahe_dis_bus.append(tmp_rout_dis_bus[i]);
                if(i>0) {
                    int l = tmp_rout_dis_bus[i-1]->get_no();
                    int r = tmp_rout_dis_bus[i]->get_no();
                    ant_shahe_d_dis_bus += adj_map[l][r]->get_distance();
                    ant_shahe_t_dis_bus += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
            for(int i = 0; inBenbu(tmp_rout_dis_bus[tmp_rout_dis_bus.size()-1-i]->get_no()); i++) {
                ant_path_benbu_dis_bus.prepend(tmp_rout_dis_bus[tmp_rout_dis_bus.size()-1-i]);
                if(i>0) {
                    int l = tmp_rout_dis_bus[tmp_rout_dis_bus.size()-1-i]->get_no();
                    int r = tmp_rout_dis_bus[tmp_rout_dis_bus.size()-i]->get_no();
                    ant_benbu_d_dis_bus += adj_map[l][r]->get_distance();
                    ant_benbu_t_dis_bus += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
        } else {
            for(int i = 0; inBenbu(tmp_rout_dis_bus[i]->get_no()); i++) {
                ant_path_benbu_dis_bus.prepend(tmp_rout_dis_bus[i]);
                if(i>0) {
                    //更小的在右边
                    int l = tmp_rout_dis_bus[i]->get_no();
                    int r = tmp_rout_dis_bus[i-1]->get_no();
                    ant_benbu_d_dis_bus += adj_map[l][r]->get_distance();
                    ant_benbu_t_dis_bus += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
            for(int i = 0; inShahe(tmp_rout_dis_bus[tmp_rout_dis_bus.size()-1-i]->get_no()); i++) {
                ant_path_shahe_dis_bus.append(tmp_rout_dis_bus[tmp_rout_dis_bus.size()-1-i]);
                if(i>0) {
                    int l = tmp_rout_dis_bus[tmp_rout_dis_bus.size()-i]->get_no();
                    int r = tmp_rout_dis_bus[tmp_rout_dis_bus.size()-1-i]->get_no();
                    ant_shahe_d_dis_bus += adj_map[l][r]->get_distance();
                    ant_shahe_t_dis_bus += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
        }
        qDebug()<<"dis_bus split done"<<endl;

        //dis_shuttle
        if(inShahe(tmp_rout_dis_shuttle[0]->get_no())) {
            //shahe_spots_bus中包含校门
            for(int i = 0; inShahe(tmp_rout_dis_shuttle[i]->get_no()); i++) {
                ant_path_shahe_dis_shuttle.append(tmp_rout_dis_shuttle[i]);
                if(i>0) {
                    int l = tmp_rout_dis_shuttle[i-1]->get_no();
                    int r = tmp_rout_dis_shuttle[i]->get_no();
                    ant_shahe_d_dis_shuttle += adj_map[l][r]->get_distance();
                    ant_shahe_t_dis_shuttle += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
            qDebug()<<"1 "<<endl;

            for(int i = 0; inBenbu(tmp_rout_dis_shuttle[tmp_rout_dis_shuttle.size()-1-i]->get_no()); i++) {
                ant_path_benbu_dis_shuttle.prepend(tmp_rout_dis_shuttle[tmp_rout_dis_shuttle.size()-1-i]);
                if(i>0) {
                    int l = tmp_rout_dis_shuttle[tmp_rout_dis_shuttle.size()-1-i]->get_no();
                    int r = tmp_rout_dis_shuttle[tmp_rout_dis_shuttle.size()-i]->get_no();
                    ant_benbu_d_dis_shuttle += adj_map[l][r]->get_distance();
                    ant_benbu_t_dis_shuttle += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
            qDebug()<<"2"<<endl;
        } else {
            for(int i = 0; inBenbu(tmp_rout_dis_shuttle[i]->get_no()); i++) {
                ant_path_benbu_dis_shuttle.prepend(tmp_rout_dis_shuttle[i]);
                if(i>0) {
                    int l = tmp_rout_dis_shuttle[i]->get_no();
                    int r = tmp_rout_dis_shuttle[i-1]->get_no();
                    ant_benbu_d_dis_shuttle += adj_map[l][r]->get_distance();
                    ant_benbu_t_dis_shuttle += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
            qDebug()<<"3"<<endl;

            for(int i = 0; inShahe(tmp_rout_dis_shuttle[tmp_rout_dis_shuttle.size()-1-i]->get_no()); i++) {
                ant_path_shahe_dis_shuttle.append(tmp_rout_dis_shuttle[tmp_rout_dis_shuttle.size()-1-i]);
                if(i>0) {
                    int l = tmp_rout_dis_shuttle[tmp_rout_dis_shuttle.size()-i]->get_no();
                    int r = tmp_rout_dis_shuttle[tmp_rout_dis_shuttle.size()-1-i]->get_no();
                    ant_shahe_d_dis_shuttle += adj_map[l][r]->get_distance();
                    ant_shahe_t_dis_shuttle += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
        }
        qDebug()<<"dis_shuttle split done"<<endl;

        //time_bus
        if(inShahe(tmp_rout_time_bus[0]->get_no())) {
            //shahe_spots_bus中包含校门
            for(int i = 0; inShahe(tmp_rout_time_bus[i]->get_no()); i++) {
                ant_path_shahe_time_bus.append(tmp_rout_time_bus[i]);
                if(i>0) {
                    int l = tmp_rout_time_bus[i-1]->get_no();
                    int r = tmp_rout_time_bus[i]->get_no();
                    ant_shahe_d_time_bus += adj_map[l][r]->get_distance();
                    ant_shahe_t_time_bus += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
            for(int i = 0; inBenbu(tmp_rout_time_bus[tmp_rout_time_bus.size()-1-i]->get_no()); i++) {
                ant_path_benbu_time_bus.prepend(tmp_rout_time_bus[tmp_rout_time_bus.size()-1-i]);
                if(i>0) {
                    int l = tmp_rout_time_bus[tmp_rout_time_bus.size()-1-i]->get_no();
                    int r = tmp_rout_time_bus[tmp_rout_time_bus.size()-i]->get_no();
                    ant_benbu_d_time_bus += adj_map[l][r]->get_distance();
                    ant_benbu_t_time_bus += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
        } else {
            for(int i = 0; inBenbu(tmp_rout_time_bus[i]->get_no()); i++) {
                ant_path_benbu_time_bus.prepend(tmp_rout_time_bus[i]);
                if(i>0) {
                    int l = tmp_rout_time_bus[i]->get_no();
                    int r = tmp_rout_time_bus[i-1]->get_no();
                    ant_benbu_d_time_bus += adj_map[l][r]->get_distance();
                    ant_benbu_t_time_bus += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
            for(int i = 0; inShahe(tmp_rout_time_bus[tmp_rout_time_bus.size()-1-i]->get_no()); i++) {
                ant_path_shahe_time_bus.append(tmp_rout_time_bus[tmp_rout_time_bus.size()-1-i]);
                if(i>0) {
                    int l = tmp_rout_time_bus[tmp_rout_time_bus.size()-i]->get_no();
                    int r = tmp_rout_time_bus[tmp_rout_time_bus.size()-1-i]->get_no();
                    ant_shahe_d_time_bus += adj_map[l][r]->get_distance();
                    ant_shahe_t_time_bus += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
        }
        qDebug()<<"time_bus split done"<<endl;

        //time_shuttle
        if(inShahe(tmp_rout_time_shuttle[0]->get_no())) {
            //shahe_spots_bus中包含校门
            for(int i = 0; i<inShahe(tmp_rout_time_shuttle[i]->get_no()); i++) {
                ant_path_shahe_time_shuttle.append(tmp_rout_time_shuttle[i]);
                if(i>0) {
                    int l = tmp_rout_time_shuttle[i-1]->get_no();
                    int r = tmp_rout_time_shuttle[i]->get_no();
                    ant_shahe_d_time_shuttle += adj_map[l][r]->get_distance();
                    ant_shahe_t_time_shuttle += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
            for(int i = 0; inBenbu(tmp_rout_time_shuttle[tmp_rout_time_shuttle.size()-1-i]->get_no()); i++) {
                ant_path_benbu_time_shuttle.prepend(tmp_rout_time_shuttle[tmp_rout_time_shuttle.size()-1-i]);
                if(i>0) {
                    int l = tmp_rout_time_shuttle[tmp_rout_time_shuttle.size()-1-i]->get_no();
                    int r = tmp_rout_time_shuttle[tmp_rout_time_shuttle.size()-i]->get_no();
                    ant_benbu_d_time_shuttle += adj_map[l][r]->get_distance();
                    ant_benbu_t_time_shuttle += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
        } else {
            for(int i = 0; inBenbu(tmp_rout_time_shuttle[i]->get_no()); i++) {
                ant_path_benbu_time_shuttle.prepend(tmp_rout_time_shuttle[i]);
                if(i>0) {
                    int l = tmp_rout_time_shuttle[i]->get_no();
                    int r = tmp_rout_time_shuttle[i-1]->get_no();
                    ant_benbu_d_time_shuttle += adj_map[l][r]->get_distance();
                    ant_benbu_t_time_shuttle += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
            for(int i = 0; inShahe(tmp_rout_time_shuttle[tmp_rout_time_shuttle.size()-1-i]->get_no()); i++) {
                ant_path_shahe_time_shuttle.append(tmp_rout_time_shuttle[tmp_rout_time_shuttle.size()-1-i]);
                if(i>0) {
                    int l = tmp_rout_time_shuttle[tmp_rout_time_shuttle.size()-i]->get_no();
                    int r = tmp_rout_time_shuttle[tmp_rout_time_shuttle.size()-1-i]->get_no();
                    ant_shahe_d_time_shuttle += adj_map[l][r]->get_distance();
                    ant_shahe_t_time_shuttle += adj_map[l][r]->get_distance()/adj_map[l][r]->get_crowdness()*velocity;
                }
            }
        }
        qDebug()<<"time_shuttle split done"<<endl;

        //设置中间路段
        dij_second_bus.set_dij_path(s2b_bus_path);
        dij_second_bus.set_begin(SHAHE_BUS_VER_NO);
        dij_second_bus.set_end(BENBU_BUS_VER_NO);
        dij_second_bus.set_total_dis(bus_total_dis);
        dij_second_bus.set_total_time(bus_total_time);

        dij_second_shuttle.set_dij_path(s2b_shuttle_path);
        dij_second_shuttle.set_begin(SHAHE_SHUTTLE_VER_NO);
        dij_second_shuttle.set_end(BENBU_SHUTTLE_VER_NO);
        dij_second_shuttle.set_total_dis(shuttle_total_dis);
        dij_second_shuttle.set_total_time(shuttle_total_time);

        //距离优先
        if(ant_shahe_d_dis_bus+ant_benbu_d_dis_bus < ant_shahe_d_dis_shuttle + ant_benbu_d_dis_shuttle) {
            ant_path_shahe_dis = ant_path_shahe_dis_bus;
            dij_dis_second = dij_second_bus;
            ant_path_benbu_dis = ant_path_benbu_dis_bus;
            ant_shahe_d_dis = ant_shahe_d_dis_bus;
            ant_shahe_t_dis = ant_shahe_t_dis_bus;
            ant_benbu_d_dis = ant_benbu_d_dis_bus;
            ant_benbu_t_dis = ant_benbu_t_dis_bus;
            cross_dis_v = BUS;
        } else {
            ant_path_shahe_dis = ant_path_shahe_dis_shuttle;
            dij_dis_second = dij_second_shuttle;
            ant_path_benbu_dis = ant_path_benbu_dis_shuttle;
            ant_shahe_d_dis = ant_shahe_d_dis_shuttle;
            ant_shahe_t_dis = ant_shahe_t_dis_shuttle;
            ant_benbu_d_dis = ant_benbu_d_dis_shuttle;
            ant_benbu_t_dis = ant_benbu_t_dis_shuttle;
            cross_dis_v = SHUTTLE;
        }

        //时间优先
        if(ant_shahe_t_time_bus + dij_second_bus.get_total_time() +
                ant_benbu_t_time_bus + getFirstBusTime(ant_shahe_t_time_bus)<
                ant_shahe_t_time_shuttle + dij_second_shuttle.get_total_time() +
                ant_benbu_t_time_shuttle + getFirstShuttleTime(ant_shahe_t_time_shuttle)) {
            ant_path_shahe_time = ant_path_shahe_time_bus;
            dij_time_second = dij_second_bus;
            ant_path_benbu_time = ant_path_benbu_time_bus;
            ant_shahe_d_time = ant_shahe_d_time_bus;
            ant_shahe_t_time = ant_shahe_t_time_bus;
            ant_benbu_d_time = ant_benbu_d_time_bus;
            ant_benbu_t_time = ant_benbu_t_time_bus;
            cross_time_v = BUS;
            wait_sec = getFirstBusTime(ant_shahe_t_time_bus);
        } else {
            ant_path_shahe_time = ant_path_shahe_time_shuttle;
            dij_time_second = dij_second_shuttle;
            ant_path_benbu_time = ant_path_benbu_time_shuttle;
            ant_shahe_d_time = ant_shahe_d_time_shuttle;
            ant_shahe_t_time = ant_shahe_t_time_shuttle;
            ant_benbu_d_time = ant_benbu_d_time_shuttle;
            ant_benbu_t_time = ant_benbu_t_time_shuttle;
            cross_time_v = SHUTTLE;
            wait_sec = getFirstShuttleTime(ant_shahe_t_time_shuttle);
        }
    }

    //显示用于选择优先度的按钮
    if(cross==false) {
        lb_antfind_dis->setText("距离优先，总距离:"+QString::number(ant_dis.get_min_dis_inall())+
                                "总时间:"+QString::number(ant_dis.get_min_time_inall()));
        lb_antfind_time->setText("时间优先，总距离:"+QString::number(ant_time.get_min_dis_inall())+
                                 "总时间:"+QString::number(ant_time.get_min_time_inall()));
    } else {
        lb_antfind_dis->setText("步行距离优先，总步行距离:"+QString::number(ant_shahe_d_dis + ant_benbu_d_dis)+
                                "总时间:"+QString::number(ant_shahe_t_dis + dij_dis_second.get_total_time() + ant_benbu_t_dis));
        lb_antfind_time->setText("时间优先，总步行距离:"+QString::number(ant_shahe_d_time + ant_benbu_d_time)+
                                 "总时间:"+QString::number(ant_shahe_t_time + dij_time_second.get_total_time() + ant_benbu_t_time));
    }
    //清空所有标签按钮
    cancel();
    glayout->addWidget(lb_antfind_dis,0,1);
    glayout->addWidget(lb_antfind_time,1,1);
    glayout->addWidget(pbt_antfind_dis,0,2);
    glayout->addWidget(pbt_antfind_time,1,2);
}

//选择距离优先
void MainWindow::antfindStartDis() {
    if(cross==false) {
        //更改当前位置，到起点
        setCurrentToVer(ant_path_dis.front()->get_no());

        ant_path = ant_path_dis;

        setCurrent(ant_path,ant_dis.get_min_dis_inall(),ant_dis.get_min_time_inall(),Pr_Distance);


    } else {
        ant_path_shahe = ant_path_shahe_dis;
        ant_path_benbu = ant_path_benbu_dis;
        ant_shahe_d = ant_shahe_d_dis;
        ant_shahe_t = ant_shahe_t_dis;
        ant_benbu_d = ant_benbu_d_dis;
        ant_benbu_t = ant_benbu_t_dis;

        //更改当前位置，到起点
        setCurrentToVer(ant_path_shahe[0]->get_no());

        //设置当前状态，添加路径
        setCurrent(ant_path_shahe, ant_shahe_d, ant_shahe_t, Pr_Distance);

    }

    cancel();

    //更改导航路线（删除旧的，创建新的）
    delete shahe_painter_path;
    delete between_painter_path;
    delete benbu_painter_path;
    shahe_painter_path = new QPainterPath;
    between_painter_path = new QPainterPath;
    benbu_painter_path = new QPainterPath;

    //自动切换校园地图，初始化导航路线
    if(current->inShahe()) {
        //switch中包含显示该线路
        switchShahe();
        //设置线路起点
        shahe_painter_path->moveTo(current->get_x(),current->get_y());
    } else if(current->inBenbu()) {
        switchBenbu();
        benbu_painter_path->moveTo(current->get_x(),current->get_y());
    } else {
        switchBetween();
        between_painter_path->moveTo(current->get_x(),current->get_y());
    }
}

//选择时间优先
void MainWindow::antfindStartTime() {
    if(cross==false) {
        //更改当前位置，到起点
        setCurrentToVer(ant_time.get_min_rout_inall().front());

        ant_path = ant_path_time;

        setCurrent(ant_path,ant_time.get_min_dis_inall(),ant_time.get_min_time_inall(),Pr_Time);

    } else {
        ant_path_shahe = ant_path_shahe_time;
        ant_path_benbu = ant_path_benbu_time;
        ant_shahe_d = ant_shahe_d_time;
        ant_shahe_t = ant_shahe_t_time;
        ant_benbu_d = ant_benbu_d_time;
        ant_benbu_t = ant_benbu_t_time;

        //更改当前位置，到起点
        setCurrentToVer(ant_path_shahe[0]->get_no());

        //设置当前状态，添加路径
        setCurrent(ant_path_shahe, ant_shahe_d, ant_shahe_t, Pr_Time);

    }

    cancel();

    //更改导航路线（删除旧的，创建新的）
    delete shahe_painter_path;
    delete between_painter_path;
    delete benbu_painter_path;
    shahe_painter_path = new QPainterPath;
    between_painter_path = new QPainterPath;
    benbu_painter_path = new QPainterPath;

    //自动切换校园地图，初始化导航路线
    if(current->inShahe()) {
        //switch中包含显示该线路
        switchShahe();
        //设置线路起点
        shahe_painter_path->moveTo(current->get_x(),current->get_y());
    } else if(current->inBenbu()) {
        switchBenbu();
        benbu_painter_path->moveTo(current->get_x(),current->get_y());
    } else {
        switchBetween();
        between_painter_path->moveTo(current->get_x(),current->get_y());
    }
}

void MainWindow::choose_search_fzs(int i) {
    fuzzysearch_res = fuzzysearch_vec[i];

    cancel();

    le_search->setText(vertices[fuzzysearch_res]->get_description());
    search_end = fuzzysearch_res;

}

//将选中的字符串暂存，由每个lineEdit的不同的槽函数进行更改text
void MainWindow::choose_fp_begin_fzs(int i) {
    fuzzysearch_res = fuzzysearch_vec[i];

    cancel();

    le_begin->setText(vertices[fuzzysearch_res]->get_description());
    findpath_begin = fuzzysearch_res;

}

//将选中的字符串暂存，由每个lineEdit的不同的槽函数进行更改text
void MainWindow::choose_fp_end_fzs(int i) {
    fuzzysearch_res = fuzzysearch_vec[i];

    cancel();

    le_end->setText(vertices[fuzzysearch_res]->get_description());
    findpath_end = fuzzysearch_res;

}

//将选中的字符串暂存，由每个lineEdit的不同的槽函数进行更改text
void MainWindow::choose_ant_fzs(int i) {
    fuzzysearch_res = fuzzysearch_vec[i];

    //antfind不能直接cancel，会把蚁群的标签都去掉了
    //清除fuzzysearch的按钮
    while(lblist_fuzzysearch.size()!=0) {
        glayout->removeWidget(lblist_fuzzysearch[0]);
        delete lblist_fuzzysearch[0];
        lblist_fuzzysearch.removeAt(0);
    }
    while(pbtlist_fuzzysearch.size()!=0) {
        glayout->removeWidget(pbtlist_fuzzysearch[0]);
        delete pbtlist_fuzzysearch[0];
        pbtlist_fuzzysearch.removeAt(0);
    }
    glayout->removeWidget(lb_fuzzysearch);
    glayout->removeWidget(pbt_fuzzysearch_reenter);
    lb_fuzzysearch->setParent(nullptr);
    pbt_fuzzysearch_reenter->setParent(nullptr);

    le_antfind->setText(vertices[fuzzysearch_res]->get_description());
    ant_spot = fuzzysearch_res;

}

//模糊搜索，输入必须全中文，当le输入完成后执行此函数
//空串不能模糊搜索，是任何串的子串
void MainWindow::fuzzySearch(const QString &str) {
    QVector<int> res;
    QMap<int,int> in;   //标志该点是否已经在res中了
    //首先在所有逻辑名中查找
    for(QMap<QString,QVector<int>>::iterator iter = logic2phy.begin(); iter!=logic2phy.end(); iter++) {
        //中文字符占3个
        //str在iter.key中第一次出现的下标
        if(iter.key().toStdString().find(str.toStdString())!=string::npos && iter.key().toStdString().find(str.toStdString())%3==0) {
            for(auto item : iter.value()) {
                if(in[item]==0) {
                    res.append(item);
                    in[item] = 1;
                }
            }
        }
    }
    //然后在所有物理名中查找（跳过不是spot的）
    for(auto vertex : vertices) {
        if(vertex->get_isSpot()==false) continue;
        if(vertex->get_description().toStdString().find(str.toStdString())!=string::npos &&
                vertex->get_description().toStdString().find(str.toStdString())%3==0) {
            if(in[vertex->get_no()]==0) {
                res.append(vertex->get_no());
                in[vertex->get_no()] = 1;
            }
        }
    }
    fuzzysearch_vec = res;
}

//对应每个lineedit有相应的fzs函数，在其中调用统一的fuzzysearch
//然后各自创建相应的标签、按钮，连接各自的槽函数
void MainWindow::search_fzs(const QString &str) {
    //空串直接返回
    if(str.size()==0) {
        search_end = -2;
        return;
    }
    fuzzySearch(str);

    int i = 0;
    if(fuzzysearch_vec.isEmpty()) {
        lb_fuzzysearch->setText("没有找到您输入的地点，请重新输入");
        glayout->addWidget(lb_fuzzysearch,0,3,1,2);
    }
    //新建标签、按钮进行选择
    else {
        QLabel *lb_temp;
        QPushButton *pbt_temp;
        for(i = 0; i<fuzzysearch_vec.size(); i++) {
            lb_temp = new QLabel(vertices[fuzzysearch_vec[i]]->get_description());
            pbt_temp = new QPushButton("选择");
            lblist_fuzzysearch.append(lb_temp);
            pbtlist_fuzzysearch.append(pbt_temp);
            connect(pbt_temp, &QPushButton::clicked, this, [=](){choose_search_fzs(i);});
            glayout->addWidget(lb_temp,i,3);
            glayout->addWidget(pbt_temp,i,4);
        }
    }
    //重新输入按钮
    glayout->addWidget(pbt_fuzzysearch_reenter,i,3,1,2);
}

//findpath的起点可以是当前位置(输入完成即会绑定编号)
void MainWindow::findpath_begin_fzs(const QString &str) {
    //如果输入为当前位置，不用模糊搜索
    if(str=="当前位置") {
        findpath_begin = -1;
        return;
    }
    if(str.size()==0) {
        findpath_begin = -2;
        return;
    }
    //模糊搜索
    fuzzySearch(str);

    int i = 0;
    if(fuzzysearch_vec.isEmpty()) {
        lb_fuzzysearch->setText("没有找到您输入的地点，请重新输入");
        glayout->addWidget(lb_fuzzysearch,0,3,1,2);
    }
    //新建标签、按钮进行选择
    else {
        QLabel *lb_temp;
        QPushButton *pbt_temp;
        for(i = 0; i<fuzzysearch_vec.size(); i++) {
            lb_temp = new QLabel(vertices[fuzzysearch_vec[i]]->get_description());
            pbt_temp = new QPushButton("选择");
            lblist_fuzzysearch.append(lb_temp);
            pbtlist_fuzzysearch.append(pbt_temp);
            connect(pbt_temp, &QPushButton::clicked, this, [=](){choose_fp_begin_fzs(i);});
            glayout->addWidget(lb_temp,i,3);
            glayout->addWidget(pbt_temp,i,4);
        }
    }
    //重新输入按钮
    glayout->addWidget(pbt_fuzzysearch_reenter,i,3,1,2);
}

void MainWindow::findpath_end_fzs(const QString &str) {
    if(str.size()==0) {
        findpath_end = -2;
        return;
    }
    fuzzySearch(str);

    int i = 0;
    if(fuzzysearch_vec.isEmpty()) {
        lb_fuzzysearch->setText("没有找到您输入的地点，请重新输入");
        glayout->addWidget(lb_fuzzysearch,0,3,1,2);
    }
    //新建标签、按钮进行选择
    else {
        QLabel *lb_temp;
        QPushButton *pbt_temp;
        for(i = 0; i<fuzzysearch_vec.size(); i++) {
            lb_temp = new QLabel(vertices[fuzzysearch_vec[i]]->get_description());
            pbt_temp = new QPushButton("选择");
            lblist_fuzzysearch.append(lb_temp);
            pbtlist_fuzzysearch.append(pbt_temp);
            connect(pbt_temp, &QPushButton::clicked, this, [=](){choose_fp_end_fzs(i);});
            glayout->addWidget(lb_temp,i,3);
            glayout->addWidget(pbt_temp,i,4);
        }
    }
    //重新输入按钮
    glayout->addWidget(pbt_fuzzysearch_reenter,i,3,1,2);
}

void MainWindow::antfind_fzs(const QString &str) {
    if(str.size()==0) {
        ant_spot = -2;
        return;
    }
    fuzzySearch(str);

    int i = 0;
    if(fuzzysearch_vec.isEmpty()) {
        lb_fuzzysearch->setText("没有找到您输入的地点，请重新输入");
        glayout->addWidget(lb_fuzzysearch,0,3,1,2);
    }
    //新建标签、按钮进行选择
    else {
        QLabel *lb_temp;
        QPushButton *pbt_temp;
        for(i = 0; i<fuzzysearch_vec.size(); i++) {
            lb_temp = new QLabel(vertices[fuzzysearch_vec[i]]->get_description());
            pbt_temp = new QPushButton("选择");
            lblist_fuzzysearch.append(lb_temp);
            pbtlist_fuzzysearch.append(pbt_temp);
            connect(pbt_temp, &QPushButton::clicked, this, [=](){choose_ant_fzs(i);});
            glayout->addWidget(lb_temp,i,3);
            glayout->addWidget(pbt_temp,i,4);
        }
    }
    //重新输入按钮
    glayout->addWidget(pbt_fuzzysearch_reenter,i,3,1,2);
}

//开始导航，初始化当前状态
void MainWindow::setCurrent(const QVector<Vertex*> &path, qreal dis, qreal time, Priority pr) {
    QVector<Vertex*> temp_path = path;
    qreal temp_dis = dis;
    current->set_velocity(velocity);
    current->set_priority(pr);
    //如果current在节点上（单源点）
    if(current->get_current_pass()==0) {
        current->set_total_dis(temp_dis);
        current->set_total_time(time);
        current->set_path(temp_path);
        current->set_total_pass(0);
        current->set_total_pass_time(0);
        //如果终点不是当前节点
        if(temp_path.size()>1){
            current->set_next_ver_index(1);
            current->set_current_dis(adj_map[temp_path[0]->get_no()][temp_path[1]->get_no()]->get_distance());
        }
        //如果终点是当前节点,直接到达
        else {
            current->set_velocity(STOP);
            current->set_next_ver_index(-1);
            current->set_current_dis(0);
        }
    }
    //双源点（如果current在路中间）
    //如果要回头
    else if(current->get_last_ver()==temp_path[0]) {
        temp_path.prepend(current->get_next_ver());
        current->set_path(temp_path);
        current->set_last_ver(temp_path[0]);
        current->set_next_ver_index(1);
        current->set_current_pass(current->get_current_dis()-current->get_current_pass());

        current->set_total_pass(current->get_current_pass());
        current->set_total_pass_time(current->get_current_pass()/(velocity*
               adj_map[current->get_next_ver()->get_no()][current->get_last_ver()->get_no()]->get_crowdness()));
        current->set_total_dis(temp_dis+current->get_current_pass());
        current->set_total_time(time+current->get_total_pass_time());
        temp_path.removeAt(0);
    }
    //如果不要回头
    else {
        current->set_total_pass(current->get_current_pass());
        current->set_total_pass_time(current->get_current_pass()/(velocity*
               adj_map[current->get_last_ver()->get_no()][current->get_next_ver()->get_no()]->get_crowdness()));
        current->set_total_dis(temp_dis+current->get_current_pass());
        current->set_total_time(time+current->get_total_pass_time());
        temp_path.prepend(current->get_last_ver());
        current->set_path(temp_path);
        //next_ver是根据path决定的，因此改变path后一定要改变next_ver_index
        current->set_next_ver_index(1);
        temp_path.removeAt(0);
    }
}

bool MainWindow::inShahe(int spot) {
    return spot>=0 && spot<=SHAHE_MAX_NO;
}

bool MainWindow::inBenbu(int spot) {
    return spot<=BENBU_MAX_NO && spot>=BENBU_BUS_VER_NO;
}

bool MainWindow::inBetween(int spot) {
    return spot<=BETWEEN_MAX_NO && spot>=BETWEEN_MIN_NO;
}
