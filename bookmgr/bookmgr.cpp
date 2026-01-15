#include "bookmgr.h"
#include "ui_bookmgr.h"
#include <QGridLayout>
#include <QMessageBox>
#include <QDate>
#include <QInputDialog>

// ========== 图书编辑对话框实现 ==========
BookEditDialog::BookEditDialog(QWidget *parent, bool isAdd, int bookid)
    : QDialog(parent)
    , m_isAdd(isAdd)
    , m_bookid(bookid)
{
    setWindowTitle(isAdd ? "添加图书" : "修改图书");
    setFixedSize(400, 350);

    // 布局与控件
    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(20, 20, 20, 20);

    layout->addWidget(new QLabel("图书ID："), 0, 0);
    leBookid = new QLineEdit(this);
    leBookid->setEnabled(isAdd); // 修改模式下ID不可编辑
    layout->addWidget(leBookid, 0, 1);

    layout->addWidget(new QLabel("类型1："), 1, 0);
    leType1 = new QLineEdit(this);
    layout->addWidget(leType1, 1, 1);

    layout->addWidget(new QLabel("类型2："), 2, 0);
    leType2 = new QLineEdit(this);
    layout->addWidget(leType2, 2, 1);

    layout->addWidget(new QLabel("类型3："), 3, 0);
    leType3 = new QLineEdit(this);
    layout->addWidget(leType3, 3, 1);

    layout->addWidget(new QLabel("图片路径："), 4, 0);
    lePic = new QLineEdit(this);
    layout->addWidget(lePic, 4, 1);

    layout->addWidget(new QLabel("图书名称："), 5, 0);
    leName = new QLineEdit(this);
    layout->addWidget(leName, 5, 1);

    layout->addWidget(new QLabel("出版社："), 6, 0);
    lePress = new QLineEdit(this);
    layout->addWidget(lePress, 6, 1);

    layout->addWidget(new QLabel("库存数量："), 7, 0);
    leCnt = new QLineEdit(this);
    leCnt->setValidator(new QIntValidator(0, 999, this));
    layout->addWidget(leCnt, 7, 1);

    // 按钮
    btnOk = new QPushButton("确定", this);
    btnCancel = new QPushButton("取消", this);
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);
    layout->addLayout(btnLayout, 8, 0, 1, 2);

    // 绑定信号
    connect(btnOk, &QPushButton::clicked, this, &BookEditDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &BookEditDialog::reject);

    // 修改模式：加载已有数据
    if (!isAdd) {
        DBHelper db;
        QSqlQuery query = db.queryAllBooks();
        while (query.next()) {
            if (query.value("bookid").toInt() == bookid) {
                leBookid->setText(QString::number(bookid));
                leType1->setText(query.value("type1").toString());
                leType2->setText(query.value("type2").toString());
                leType3->setText(query.value("type3").toString());
                lePic->setText(query.value("pic").toString());
                leName->setText(query.value("name").toString());
                lePress->setText(query.value("press").toString());
                leCnt->setText(query.value("cnt").toString());
                break;
            }
        }
    }
}

QStringList BookEditDialog::getBookInfo() const
{
    QStringList info;
    info << leBookid->text()
         << leType1->text()
         << leType2->text()
         << leType3->text()
         << lePic->text()
         << leName->text()
         << lePress->text()
         << leCnt->text();
    return info;
}

// ========== Bookmgr主类实现 ==========
Bookmgr::Bookmgr(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Bookmgr)
{
    ui->setupUi(this);
    dbHelper = new DBHelper(this);
    initModel(); // 初始化数据模型

    // 绑定筛选信号（搜索框文本变化）
    connect(ui->selectEdit, &QLineEdit::textChanged, this, &Bookmgr::on_selectEdit_textChanged);
}

Bookmgr::~Bookmgr()
{
    delete ui;
}

void Bookmgr::initModel()
{
    // 1. 原始模型（绑定book表）
    bookModel = new QSqlTableModel(this, dbHelper->isConnected() ? dbHelper->getDatabase() : QSqlDatabase());
    bookModel->setTable("book");
    bookModel->setEditStrategy(QSqlTableModel::OnManualSubmit); // 手动提交修改
    bookModel->select(); // 加载数据

    // 2. 设置列名
    bookModel->setHeaderData(0, Qt::Horizontal, "图书ID");
    bookModel->setHeaderData(1, Qt::Horizontal, "类型1");
    bookModel->setHeaderData(2, Qt::Horizontal, "类型2");
    bookModel->setHeaderData(3, Qt::Horizontal, "类型3");
    bookModel->setHeaderData(4, Qt::Horizontal, "图片路径");
    bookModel->setHeaderData(5, Qt::Horizontal, "图书名称");
    bookModel->setHeaderData(6, Qt::Horizontal, "出版社");
    bookModel->setHeaderData(7, Qt::Horizontal, "库存数量");

    // 3. 筛选代理模型（支持多条件筛选）
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(bookModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive); // 不区分大小写

    // 4. 绑定到TableView
    ui->booktableView->setModel(proxyModel);
    ui->booktableView->setColumnWidth(5, 150); // 图书名称列加宽
    ui->booktableView->setSortingEnabled(true); // 允许排序
}

int Bookmgr::getSelectedBookId()
{
    QModelIndexList selected = ui->booktableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选中一行图书！");
        return -1;
    }
    // 从代理模型转换到原始模型的索引，获取bookid
    QModelIndex sourceIndex = proxyModel->mapToSource(selected.first());
    return bookModel->data(bookModel->index(sourceIndex.row(), 0)).toInt();
}

// 添加图书
void Bookmgr::on_btnadd_clicked()
{
    BookEditDialog dialog(this, true);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList info = dialog.getBookInfo();
        // 空值校验
        if (info[0].isEmpty() || info[5].isEmpty() || info[7].isEmpty()) {
            QMessageBox::warning(this, "警告", "图书ID、名称、库存不能为空！");
            return;
        }
        // 调用DBHelper添加
        bool success = dbHelper->insertBook(
            info[0].toInt(), info[1], info[2], info[3], info[4], info[5], info[6], info[7].toInt()
            );
        if (success) {
            QMessageBox::information(this, "成功", "图书添加成功！");
            bookModel->select(); // 刷新表格
        } else {
           QMessageBox::critical(this, "失败", "图书添加失败：" + dbHelper->getDatabase().lastError().text());
        }
    }
}

// 修改图书
void Bookmgr::on_btnchange_clicked()
{
    int bookid = getSelectedBookId();
    if (bookid == -1) return;

    BookEditDialog dialog(this, false, bookid);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList info = dialog.getBookInfo();
        if (info[5].isEmpty() || info[7].isEmpty()) {
            QMessageBox::warning(this, "警告", "图书名称、库存不能为空！");
            return;
        }
        bool success = dbHelper->updateBook(
            bookid, info[1], info[2], info[3], info[4], info[5], info[6], info[7].toInt()
            );
        if (success) {
            QMessageBox::information(this, "成功", "图书修改成功！");
            bookModel->select();
        } else {
            QMessageBox::critical(this, "失败", "图书修改失败！");
        }
    }
}

// 删除图书
void Bookmgr::on_btndelete_clicked()
{
    int bookid = getSelectedBookId();
    if (bookid == -1) return;

    if (QMessageBox::question(this, "确认", "确定要删除该图书吗？", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    bool success = dbHelper->deleteBook(bookid);
    if (success) {
        QMessageBox::information(this, "成功", "图书删除成功！");
        bookModel->select();
    } else {
        QMessageBox::critical(this, "失败", "图书删除失败！");
    }
}

// 借书功能
void Bookmgr::on_btnborrow_clicked()
{
    int bookid = getSelectedBookId();
    if (bookid == -1) return;

    // 输入读者ID
    bool ok;
    QString useridStr = QInputDialog::getText(this, "借书", "请输入读者ID：", QLineEdit::Normal, "", &ok);
    if (!ok || useridStr.isEmpty()) return;
    int userid = useridStr.toInt();

    // 检查读者是否存在
    QSqlQuery userQuery = dbHelper->filterUsers(useridStr);
    if (!userQuery.next()) {
        QMessageBox::warning(this, "警告", "该读者不存在！");
        return;
    }

    // 执行借书
    QString startDate = QDate::currentDate().toString("yyyy-MM-dd");
    bool success = dbHelper->borrowBook(bookid, userid, startDate);
    if (success) {
        QMessageBox::information(this, "成功", "借书成功！\n借阅日期：" + startDate);
        bookModel->select(); // 刷新库存
    } else {
        QMessageBox::critical(this, "失败", "借书失败（库存不足或数据错误）！");
    }
}

// 还书功能
void Bookmgr::on_btnreturn_clicked()
{
    int bookid = getSelectedBookId();
    if (bookid == -1) return;

    // 输入读者ID
    bool ok;
    QString useridStr = QInputDialog::getText(this, "还书", "请输入读者ID：", QLineEdit::Normal, "", &ok);
    if (!ok || useridStr.isEmpty()) return;
    int userid = useridStr.toInt();

    // 执行还书
    bool success = dbHelper->returnBook(bookid, userid);
    if (success) {
        QMessageBox::information(this, "成功", "还书成功！\n归还日期：" + QDate::currentDate().toString("yyyy-MM-dd"));
        bookModel->select(); // 刷新库存
    } else {
        QMessageBox::critical(this, "失败", "还书失败（无匹配借阅记录）！");
    }
}

// 筛选图书（搜索框文本变化）
void Bookmgr::on_selectEdit_textChanged(const QString &arg1)
{
    // 多列筛选：图书名称、类型1-3、出版社
    proxyModel->setFilterKeyColumn(-1); // -1表示所有列
    proxyModel->setFilterFixedString(arg1); // 精确匹配（可改为setFilterRegExp实现模糊匹配）
}
