#include "log_search.h"
#include "ui_log_search.h"

Log_search::Log_search(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Log_search)
{
    ui->setupUi(this);
}

Log_search::Log_search(Log* log, QWidget *parent) : QDialog(parent),
    ui(new Ui::Log_search), log(log) {
    ui->setupUi(this);
}

Log_search::~Log_search()
{
    delete ui;
}

void Log_search::on_pushButton_clicked()
{
    ui->textBrowser->clear();
    res_list.clear();
    //只要有一个是空
    if(ui->le_begin_w->text().isEmpty() || ui->le_begin_h->text().isEmpty() ||
            ui->le_begin_m->text().isEmpty() || ui->le_end_w->text().isEmpty()||
            ui->le_end_h->text().isEmpty() || ui->le_end_m->text().isEmpty()) {
        //使用默认end（即当前时间）
        //默认输出从3小时前到当前时间的
        begin = end-3;
    } else {
        Clock tmp(0,ui->le_begin_m->text().toInt(),ui->le_begin_h->text().toInt(),ui->le_begin_w->text().toInt());
        begin = tmp;
        Clock tmp2(0,ui->le_end_m->text().toInt(),ui->le_end_h->text().toInt(),ui->le_end_w->text().toInt());
        end = tmp2;
    }
    ope = ui->comboBox->currentText();

    res_list = log->search(end,begin,ope);
    for(auto str : res_list) {
        ui->textBrowser->append(str);
    }
}

void Log_search::sendData(Clock cur) {
    end = cur;
}
