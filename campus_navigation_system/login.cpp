
#include "login.h"
#include "ui_login.h"
#include "constants.h"

using namespace std;

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    setWindowTitle("用户登陆");
    readAccounts();
}

Login::~Login()
{
    delete ui;
}

void Login::on_pbt_login_clicked()
{
    if(checkLogin(ui->le_id->text(),ui->le_pwd->text())) {
        online_account = accounts[ui->le_id->text()];
        accept();
    } else {
        QMessageBox::warning(this,"warning!","密码错误！",QMessageBox::Yes);
    }
}

void Login::on_pbt_exit_clicked()
{
    reject();
}

void Login::readAccounts() {
    ifstream fin;
    fin.open(ACCOUNTS_PATH, ios::in);
    if(!fin.is_open()) {
        qDebug()<<"accounts file open error"<<endl;
        exit(1);
    }
    string id,pwd,course_name;
    int bmin,bhour,bwday,emin,ehour,ewday,classroom;
    char ch;
    //一行是一个账户
    while (fin.peek()!=EOF) {
        fin>>id>>pwd;
        fin>>course_name>>bwday>>bhour>>bmin>>ewday>>ehour>>emin>>classroom;
        ch = fin.get();
        QVector<Course> courses;

        //循环读取课程信息（名称，结束时间（周时分），教室）
        while(fin.peek()!=EOF&&ch!='\r' && ch!='\n') {
            Course temp(QString::fromStdString(course_name),Clock(0,bmin,bhour,bwday),Clock(0,emin,ehour,ewday),classroom);
            courses.push_back(temp);
            fin>>course_name>>bwday>>bhour>>bmin>>ewday>>ehour>>emin>>classroom;
            ch = fin.get();
        }
        courses.push_back(Course(QString::fromStdString(course_name),Clock(0,bmin,bhour,bwday),Clock(0,emin,ehour,ewday),classroom));
        Account *account = new Account(QString::fromStdString(id),QString::fromStdString(pwd),courses);

        //id到account*的映射
        accounts[QString::fromStdString(id)] = account;
    }
    fin.close();
    qDebug()<<"accounts load success"<<endl;
}

bool Login::checkLogin(const QString &id, const QString &pwd) {
    if(accounts.find(id)==accounts.end()) return false;
    return accounts[id]->get_password()==pwd;
}
