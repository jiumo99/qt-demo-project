#include "record.h"
#include "ui_record.h"
#include <QMessageBox>
#include <QDate>
#include <QChart>
#include <QBarSeries>
#include <QBarSet>
#include <QChartView>
#include <QBarCategoryAxis>
#include <QValueAxis>

Record::Record(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Record)
{
    ui->setupUi(this);
    dbHelper = new DBHelper(this);
    initModel();
    showOverdueAlert(); // 初始化时显示逾期提醒
}

Record::~Record()
{
    delete ui;
}

void Record::initModel()
{
    // 1. 查询模型（关联record+book+user表，显示关联信息）
    recordModel = new QSqlQueryModel(this);
    recordModel->setQuery(dbHelper->queryAllRecords());

    // 2. 设置列名
    recordModel->setHeaderData(0, Qt::Horizontal, "记录ID");
    recordModel->setHeaderData(1, Qt::Horizontal, "图书ID");
    recordModel->setHeaderData(2, Qt::Horizontal, "图书名称");
    recordModel->setHeaderData(3, Qt::Horizontal, "读者ID");
    recordModel->setHeaderData(4, Qt::Horizontal, "读者昵称");
    recordModel->setHeaderData(5, Qt::Horizontal, "借阅日期");
    recordModel->setHeaderData(6, Qt::Horizontal, "归还日期");

    // 3. 筛选代理模型
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(recordModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    // 4. 绑定表格
    ui->borrowtableView->setModel(proxyModel);
    ui->borrowtableView->setColumnWidth(2, 150);
    ui->borrowtableView->setSortingEnabled(true);
}

void Record::showOverdueAlert()
{
    // 查询逾期记录
    QSqlQuery overdueQuery = dbHelper->queryOverdueRecords();
    int overdueCnt = 0;
    QString overdueInfo = "逾期借阅记录：\n";

    while (overdueQuery.next()) {
        overdueCnt++;
        QString bookname = overdueQuery.value("bookname").toString();
        QString username = overdueQuery.value("username").toString();
        QString startDate = overdueQuery.value("start").toString();
        overdueInfo += QString("图书：%1 | 读者：%2 | 借阅日期：%3\n").arg(bookname).arg(username).arg(startDate);
    }

    // 有逾期记录则弹窗提醒
    if (overdueCnt > 0) {
        QMessageBox::warning(this, "逾期提醒", QString("共%1条逾期记录！\n%2").arg(overdueCnt).arg(overdueInfo));
    }
}

// 筛选借阅记录
void Record::on_selectEdit_textChanged(const QString &arg1)
{
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterFixedString(arg1);
}

// 清空记录（仅删除已归还的记录）
void Record::on_btnclearrecord_clicked()
{
    if (QMessageBox::question(this, "确认", "确定要清空所有已归还的借阅记录吗？", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    QSqlQuery query;
    query.exec("DELETE FROM record WHERE end IS NOT NULL");
    QMessageBox::information(this, "成功", "已清空所有已归还的借阅记录！");
    recordModel->setQuery(dbHelper->queryAllRecords()); // 刷新
}

// 借阅数据统计（图表展示）
void Record::on_btndatastatistics_clicked()
{
    QVariantMap stats = dbHelper->statisticsBorrow();

    // 1. 创建统计窗口
    QDialog *statDialog = new QDialog(this);
    statDialog->setWindowTitle("借阅数据统计");
    statDialog->setFixedSize(600, 400);

    // 2. 创建柱状图（总借阅数、逾期数）
    QBarSet *set = new QBarSet("数量");
    *set << stats["total"].toInt() << stats["overdue"].toInt();

    QBarSeries *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("借阅统计");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // X轴标签
    QStringList categories;
    categories << "总借阅数" << "逾期数";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Y轴（整数）
    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%d");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // 图表视图
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 3. 热门图书文本
    QLabel *hotBookLabel = new QLabel(statDialog);
    QString hotBookText = "热门图书TOP3：\n" + stats["hotBooks"].toStringList().join("\n");
    hotBookLabel->setText(hotBookText);

    // 4. 布局
    QVBoxLayout *layout = new QVBoxLayout(statDialog);
    layout->addWidget(chartView);
    layout->addWidget(hotBookLabel);

    statDialog->exec();
    delete statDialog;
}

// 查看逾期记录
void Record::on_btncheckrecord_clicked()
{
    recordModel->setQuery(dbHelper->queryOverdueRecords());
    QMessageBox::information(this, "逾期记录", "已显示所有逾期借阅记录！");
}
