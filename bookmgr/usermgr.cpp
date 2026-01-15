#include "usermgr.h"
#include "ui_usermgr.h"
#include <QMessageBox>

usermgr::usermgr(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::usermgr)
{
    ui->setupUi(this);
    dbHelper = new DBHelper(this);
    initModel();

    // 绑定搜索框信号
    connect(ui->selectuserEdit, &QLineEdit::textChanged, this, &usermgr::on_selectuserEdit_textChanged);
}

usermgr::~usermgr()
{
    delete ui;
}

void usermgr::initModel()
{
    userModel = new QSqlTableModel(this, dbHelper->isConnected() ? dbHelper->getDatabase() : QSqlDatabase());
    userModel->setTable("user");
    userModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    userModel->select();

    // 设置列名
    userModel->setHeaderData(0, Qt::Horizontal, "读者ID");
    userModel->setHeaderData(1, Qt::Horizontal, "用户名");
    userModel->setHeaderData(2, Qt::Horizontal, "密码");
    userModel->setHeaderData(3, Qt::Horizontal, "昵称");
    userModel->setHeaderData(4, Qt::Horizontal, "权限");
    userModel->setHeaderData(5, Qt::Horizontal, "部门");
    userModel->setHeaderData(6, Qt::Horizontal, "年级");

    // 筛选代理模型
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(userModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    // 绑定表格
    ui->usertableView->setModel(proxyModel);
    ui->usertableView->hideColumn(2); // 隐藏密码列
    ui->usertableView->setColumnWidth(3, 100);
    ui->usertableView->setSortingEnabled(true);
}

int usermgr::getSelectedUserId()
{
    QModelIndexList selected = ui->usertableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选中一行读者！");
        return -1;
    }
    QModelIndex sourceIndex = proxyModel->mapToSource(selected.first());
    return userModel->data(userModel->index(sourceIndex.row(), 0)).toInt();
}

// 搜索用户
void usermgr::on_selectuserEdit_textChanged(const QString &arg1)
{
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterFixedString(arg1);
}

// 删除用户
void usermgr::on_btndelectuser_clicked()
{
    int userid = getSelectedUserId();
    if (userid == -1) return;

    if (QMessageBox::question(this, "确认", "确定要删除该读者吗？", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    bool success = dbHelper->deleteUser(userid);
    if (success) {
        QMessageBox::information(this, "成功", "读者删除成功！");
        userModel->select();
    } else {
        QMessageBox::critical(this, "失败", "读者删除失败！");
    }
}

// 导入用户（CSV格式：userid,username,password,nickname,auth,department,grade）
void usermgr::on_btnimportuser_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择CSV文件", "", "CSV Files (*.csv)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "失败", "文件打开失败！");
        return;
    }

    QTextStream in(&file);
    int successCnt = 0;
    int failCnt = 0;

    // 跳过表头（如果CSV有表头）
    in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList fields = line.split(",");
        if (fields.size() != 7) { // 必须包含7个字段
            failCnt++;
            continue;
        }

        // 解析字段
        int userid = fields[0].toInt();
        QString username = fields[1];
        QString password = fields[2];
        QString nickname = fields[3];
        QString auth = fields[4];
        QString department = fields[5];
        QString grade = fields[6];

        // 插入数据库
        if (dbHelper->insertUser(userid, username, password, nickname, auth, department, grade)) {
            successCnt++;
        } else {
            failCnt++;
        }
    }

    file.close();
    QMessageBox::information(this, "导入结果",
                             QString("导入完成！\n成功：%1条\n失败：%2条").arg(successCnt).arg(failCnt));
    userModel->select(); // 刷新表格
}
