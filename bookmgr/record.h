#ifndef RECORD_H
#define RECORD_H
#include <QWidget>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include "dbhelper.h"

namespace Ui {
class Record;
}

class Record : public QWidget
{
    Q_OBJECT
public:
    explicit Record(QWidget *parent = nullptr);
    ~Record();

private slots:
    void on_selectEdit_textChanged(const QString &arg1);
    void on_btnclearrecord_clicked();
    void on_btndatastatistics_clicked();
    void on_btncheckrecord_clicked();

private:
    Ui::Record *ui;
    DBHelper *dbHelper;
    QSqlQueryModel *recordModel;
    QSortFilterProxyModel *proxyModel;

    void initModel();
    void showOverdueAlert();
};

#endif // RECORD_H
