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
    connect(ui->selectuserEdit, &QLineEdit::textChanged, this, &usermgr::on_selectuserEdit_textChanged);
}

usermgr::~usermgr()
{
    delete ui;
}

void usermgr::initModel()
{
    userModel = new QSqlTableModel(this, dbHelper->isConnected() ? dbHelper->getDatabase() : QSqlDatabase());
    userModel->setTable("User"); // 新表名
    userModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    userModel->select();

    // 设置列名（适配新表字段）
    userModel->setHeaderData(0, Qt::Horizontal, "读者ID");
    userModel->setHeaderData(1, Qt::Horizontal, "用户名");
    userModel->setHeaderData(2, Qt::Horizontal, "密码");
    userModel->setHeaderData(3, Qt::Horizontal, "昵称");
    userModel->setHeaderData(4, Qt::Horizontal, "角色");
    userModel->setHeaderData(5, Qt::Horizontal, "班级");
    userModel->setHeaderData(6, Qt::Horizontal, "专业");
    userModel->setHeaderData(7, Qt::Horizontal, "性别");
    userModel->setHeaderData(8, Qt::Horizontal, "电话");
    userModel->setHeaderData(9, Qt::Horizontal, "邮箱");
    userModel->setHeaderData(10, Qt::Horizontal, "账户状态");

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(userModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->usertableView->setModel(proxyModel);
    ui->usertableView->hideColumn(2); // 隐藏密码列
    ui->usertableView->setColumnWidth(3, 100);
    ui->usertableView->setColumnWidth(5, 120);
    ui->usertableView->setColumnWidth(6, 150);
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
    int user_id = getSelectedUserId();
    if (user_id == -1) return;
    if (QMessageBox::question(this, "确认", "确定要删除该读者吗？", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }
    bool success = dbHelper->deleteUser(user_id);
    if (success) {
        QMessageBox::information(this, "成功", "读者删除成功！");
        userModel->select();
    } else {
        QMessageBox::critical(this, "失败", "读者删除失败（可能存在未归还的借阅记录）！");
    }
}

// 导入用户（CSV格式：user_id,username,password,nickname,role,class,major,gender,phone,email,account_status）
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
    in.readLine(); // 跳过表头

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList fields = line.split(",");
        if (fields.size() != 11) { // 新表11个核心字段
            failCnt++;
            continue;
        }
        // 解析字段（按新表顺序）
        int user_id = fields[0].toInt();
        QString username = fields[1];
        QString password = fields[2];
        QString nickname = fields[3];
        QString role = fields[4];
        QString class_ = fields[5];
        QString major = fields[6];
        QString gender = fields[7];
        QString phone = fields[8];
        QString email = fields[9];
        QString account_status = fields[10];

        // 插入数据库
        if (dbHelper->insertUser(user_id, username, password, nickname, role, class_, major, gender, phone, email, account_status)) {
            successCnt++;
        } else {
            failCnt++;
        }
    }
    file.close();
    QMessageBox::information(this, "导入结果",
                             QString("导入完成！\n成功：%1条\n失败：%2条").arg(successCnt).arg(failCnt));
    userModel->select();
}
