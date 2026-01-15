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
}

Record::~Record()
{
    delete ui;
}

// 公共刷新函数
void Record::refreshRecords()
{
    recordModel->setQuery(dbHelper->queryAllRecords());
    ui->borrowtableView->viewport()->update();
}

// 公共刷新逾期记录函数
void Record::refreshOverdueRecords()
{
    recordModel->setQuery(dbHelper->queryOverdueRecords());
    ui->borrowtableView->viewport()->update();
}

void Record::initModel()
{
    recordModel = new QSqlQueryModel(this);
    recordModel->setQuery(dbHelper->queryAllRecords());

    // 设置列名（适配新表字段）
    recordModel->setHeaderData(0, Qt::Horizontal, "记录ID");
    recordModel->setHeaderData(1, Qt::Horizontal, "图书ID");
    recordModel->setHeaderData(2, Qt::Horizontal, "图书名称");
    recordModel->setHeaderData(3, Qt::Horizontal, "读者ID");
    recordModel->setHeaderData(4, Qt::Horizontal, "读者昵称");
    recordModel->setHeaderData(5, Qt::Horizontal, "借阅日期");
    recordModel->setHeaderData(6, Qt::Horizontal, "应还日期");
    recordModel->setHeaderData(7, Qt::Horizontal, "归还日期");
    recordModel->setHeaderData(8, Qt::Horizontal, "借阅状态");
    recordModel->setHeaderData(9, Qt::Horizontal, "逾期天数");
    recordModel->setHeaderData(10, Qt::Horizontal, "罚金（元）");

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(recordModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->borrowtableView->setModel(proxyModel);
    ui->borrowtableView->setColumnWidth(2, 150);
    ui->borrowtableView->setColumnWidth(5, 120);
    ui->borrowtableView->setColumnWidth(6, 120);
    ui->borrowtableView->setColumnWidth(7, 120);
    ui->borrowtableView->setSortingEnabled(true);
}

// 逾期提醒函数（保留功能，改为手动触发）
void Record::showOverdueAlert()
{
    QSqlQuery overdueQuery = dbHelper->queryOverdueRecords();
    int overdueCnt = 0;
    QString overdueInfo = "逾期借阅记录：\n";
    while (overdueQuery.next()) {
        overdueCnt++;
        QString bookname = overdueQuery.value("book_name").toString();
        QString username = overdueQuery.value("nickname").toString();
        QString startDate = overdueQuery.value("borrow_date").toString().left(10);
        int overdueDays = overdueQuery.value("overdue_days").toInt();
        double fine = overdueQuery.value("fine_amount").toDouble();
        overdueInfo += QString("图书：%1 | 读者：%2 | 借阅日期：%3 | 逾期：%4天 | 罚金：%5元\n")
                           .arg(bookname).arg(username).arg(startDate).arg(overdueDays).arg(fine, 0, 'f', 2);
    }
    if (overdueCnt > 0) {
        QMessageBox::warning(this, "逾期提醒", QString("共%1条逾期记录！\n%2").arg(overdueCnt).arg(overdueInfo));
    } else {
        QMessageBox::information(this, "逾期提醒", "暂无逾期借阅记录！");
    }
}

// 筛选借阅记录
void Record::on_selectEdit_textChanged(const QString &arg1)
{
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterFixedString(arg1);
}

// 清空已归还记录
void Record::on_btnclearrecord_clicked()
{
    if (QMessageBox::question(this, "确认", "确定要清空所有已归还的借阅记录吗？", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }
    QSqlQuery query;
    query.exec("DELETE FROM BorrowRecord WHERE borrow_status='returned'");
    QMessageBox::information(this, "成功", "已清空所有已归还的借阅记录！");
    recordModel->setQuery(dbHelper->queryAllRecords());
}

// 借阅数据统计
void Record::on_btndatastatistics_clicked()
{
    QVariantMap stats = dbHelper->statisticsBorrow();
    QDialog *statDialog = new QDialog(this);
    statDialog->setWindowTitle("借阅数据统计");
    statDialog->setFixedSize(600, 400);

    // 柱状图（总借阅数、逾期数）
    QBarSet *set = new QBarSet("数量");
    *set << stats["total"].toInt() << stats["overdue"].toInt();
    QBarSeries *series = new QBarSeries();
    series->append(set);
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("借阅统计");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories = {"总借阅数", "逾期数"};
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%d");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 热门图书文本
    QLabel *hotBookLabel = new QLabel(statDialog);
    QString hotBookText = "热门图书TOP3：\n" + stats["hotBooks"].toStringList().join("\n");
    hotBookLabel->setText(hotBookText);

    // 布局
    QVBoxLayout *layout = new QVBoxLayout(statDialog);
    layout->addWidget(chartView);
    layout->addWidget(hotBookLabel);
    statDialog->exec();
    delete statDialog;
}

// 查看逾期记录
void Record::on_btncheckrecord_clicked()
{
    refreshOverdueRecords();
    showOverdueAlert();
}
