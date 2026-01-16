#include "bookmgr.h"
#include "ui_bookmgr.h"
#include <QGridLayout>
#include <QMessageBox>
#include <QDate>
#include <QInputDialog>

// ========== 图书编辑对话框实现 ==========
BookEditDialog::BookEditDialog(QWidget *parent, bool isAdd, int book_id)
    : QDialog(parent)
    , m_isAdd(isAdd)
    , m_book_id(book_id)
{
    setWindowTitle(isAdd ? "添加图书" : "修改图书");
    setFixedSize(500, 550);

    // 布局与控件
    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(20, 20, 20, 20);

    // 1. 基础字段
    layout->addWidget(new QLabel("图书ID："), 0, 0);
    leBookId = new QLineEdit(this);
    leBookId->setEnabled(isAdd);
    layout->addWidget(leBookId, 0, 1);

    layout->addWidget(new QLabel("ISBN："), 1, 0);
    leIsbn = new QLineEdit(this);
    layout->addWidget(leIsbn, 1, 1);

    layout->addWidget(new QLabel("图书名称："), 2, 0);
    leBookName = new QLineEdit(this);
    layout->addWidget(leBookName, 2, 1);

    layout->addWidget(new QLabel("作者："), 3, 0);
    leAuthor = new QLineEdit(this);
    layout->addWidget(leAuthor, 3, 1);

    layout->addWidget(new QLabel("出版社："), 4, 0);
    lePublisher = new QLineEdit(this);
    layout->addWidget(lePublisher, 4, 1);

    layout->addWidget(new QLabel("出版日期："), 5, 0);
    dePublishDate = new QDateEdit(this);
    dePublishDate->setCalendarPopup(true);
    dePublishDate->setDate(QDate::currentDate());
    layout->addWidget(dePublishDate, 5, 1);

    // 2. 分类字段
    layout->addWidget(new QLabel("分类："), 6, 0);
    leCategory = new QLineEdit(this);
    layout->addWidget(leCategory, 6, 1);

    layout->addWidget(new QLabel("子分类："), 7, 0);
    leSubCategory = new QLineEdit(this);
    layout->addWidget(leSubCategory, 7, 1);

    layout->addWidget(new QLabel("标签："), 8, 0);
    leTags = new QLineEdit(this);
    layout->addWidget(leTags, 8, 1);

    // 3. 价格与库存
    layout->addWidget(new QLabel("价格："), 9, 0);
    dsbPrice = new QDoubleSpinBox(this);
    dsbPrice->setRange(0.00, 9999.99);
    dsbPrice->setDecimals(2);
    layout->addWidget(dsbPrice, 9, 1);

    layout->addWidget(new QLabel("库存数量："), 10, 0);
    sbStock = new QSpinBox(this);
    sbStock->setRange(0, 999);
    layout->addWidget(sbStock, 10, 1);

    // 4. 状态与费率
    layout->addWidget(new QLabel("图书状态："), 11, 0);
    cbBookStatus = new QComboBox(this);
    cbBookStatus->addItems({"available", "damaged", "lost"});
    layout->addWidget(cbBookStatus, 11, 1);

    layout->addWidget(new QLabel("逾期费率(元/天)："), 12, 0);
    dsbOverdueFeeRate = new QDoubleSpinBox(this);
    dsbOverdueFeeRate->setRange(0.00, 10.00);
    dsbOverdueFeeRate->setDecimals(2);
    dsbOverdueFeeRate->setValue(0.50);
    layout->addWidget(dsbOverdueFeeRate, 12, 1);

    // 5. 其他字段
    layout->addWidget(new QLabel("封面路径："), 13, 0);
    leCoverPath = new QLineEdit(this);
    layout->addWidget(leCoverPath, 13, 1);

    layout->addWidget(new QLabel("描述："), 14, 0);
    teDescription = new QTextEdit(this);
    teDescription->setMaximumHeight(60);
    layout->addWidget(teDescription, 14, 1);

    // 按钮
    btnOk = new QPushButton("确定", this);
    btnCancel = new QPushButton("取消", this);
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);
    layout->addLayout(btnLayout, 15, 0, 1, 2);

    // 绑定信号
    connect(btnOk, &QPushButton::clicked, this, &BookEditDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &BookEditDialog::reject);

    // 修改模式：加载已有数据
    if (!isAdd) {
        DBHelper db;
        QSqlQuery query = db.queryAllBooks();
        while (query.next()) {
            if (query.value("book_id").toInt() == book_id) {
                leBookId->setText(QString::number(book_id));
                leIsbn->setText(query.value("isbn").toString());
                leBookName->setText(query.value("book_name").toString());
                leAuthor->setText(query.value("author").toString());
                lePublisher->setText(query.value("publisher").toString());
                dePublishDate->setDate(query.value("publish_date").toDate());
                leCategory->setText(query.value("category").toString());
                leSubCategory->setText(query.value("sub_category").toString());
                leTags->setText(query.value("tags").toString());
                dsbPrice->setValue(query.value("price").toDouble());
                sbStock->setValue(query.value("stock").toInt());
                cbBookStatus->setCurrentText(query.value("book_status").toString());
                dsbOverdueFeeRate->setValue(query.value("overdue_fee_rate").toDouble());
                leCoverPath->setText(query.value("cover_path").toString());
                teDescription->setText(query.value("description").toString());
                break;
            }
        }
    }
}

QStringList BookEditDialog::getBookInfo() const
{
    QStringList info;
    info << leBookId->text()          // 0: book_id
         << leIsbn->text()            // 1: isbn
         << leBookName->text()        // 2: book_name
         << leAuthor->text()          // 3: author
         << lePublisher->text()       // 4: publisher
         << dePublishDate->date().toString("yyyy-MM-dd") // 5: publish_date
         << leCategory->text()        // 6: category
         << leSubCategory->text()     // 7: sub_category
         << leTags->text()            // 8: tags
         << QString::number(dsbPrice->value()) // 9: price
         << QString::number(sbStock->value())  // 10: stock
         << cbBookStatus->currentText() // 11: book_status
         << QString::number(dsbOverdueFeeRate->value()) // 12: overdue_fee_rate
         << leCoverPath->text()       // 13: cover_path
         << teDescription->toPlainText(); // 14: description
    return info;
}

// ========== Bookmgr主类实现 ==========
Bookmgr::Bookmgr(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Bookmgr)
    , m_operatorId(DEFAULT_OPERATOR_ID) // 默认操作员ID
{
    ui->setupUi(this);
    dbHelper = new DBHelper(this);
    initModel();
    connect(ui->selectEdit, &QLineEdit::textChanged, this, &Bookmgr::on_selectEdit_textChanged);
}

Bookmgr::~Bookmgr()
{
    delete ui;
}

void Bookmgr::initModel()
{
    bookModel = new QSqlTableModel(this, dbHelper->isConnected() ? dbHelper->getDatabase() : QSqlDatabase());
    bookModel->setTable("Book");
    bookModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    bookModel->select();

    // 设置列名
    bookModel->setHeaderData(0, Qt::Horizontal, "图书ID");
    bookModel->setHeaderData(1, Qt::Horizontal, "ISBN");
    bookModel->setHeaderData(2, Qt::Horizontal, "图书名称");
    bookModel->setHeaderData(3, Qt::Horizontal, "作者");
    bookModel->setHeaderData(4, Qt::Horizontal, "出版社");
    bookModel->setHeaderData(5, Qt::Horizontal, "出版日期");
    bookModel->setHeaderData(6, Qt::Horizontal, "分类");
    bookModel->setHeaderData(7, Qt::Horizontal, "子分类");
    bookModel->setHeaderData(8, Qt::Horizontal, "标签");
    bookModel->setHeaderData(9, Qt::Horizontal, "价格");
    bookModel->setHeaderData(10, Qt::Horizontal, "库存");
    bookModel->setHeaderData(11, Qt::Horizontal, "状态");
    bookModel->setHeaderData(12, Qt::Horizontal, "逾期费率");
    bookModel->setHeaderData(13, Qt::Horizontal, "封面路径");
    bookModel->setHeaderData(14, Qt::Horizontal, "描述");

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(bookModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->booktableView->setModel(proxyModel);
    ui->booktableView->setColumnWidth(2, 180); // 图书名称列加宽
    ui->booktableView->setColumnWidth(14, 200); // 描述列加宽
    ui->booktableView->setSortingEnabled(true);
    // 隐藏部分不常用列
    ui->booktableView->hideColumn(13); // 封面路径
}

int Bookmgr::getSelectedBookId()
{
    QModelIndexList selected = ui->booktableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选中一行图书！");
        return -1;
    }
    QModelIndex sourceIndex = proxyModel->mapToSource(selected.first());
    return bookModel->data(bookModel->index(sourceIndex.row(), 0)).toInt();
}

// 添加图书
void Bookmgr::on_btnadd_clicked()
{
    BookEditDialog dialog(this, true);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList info = dialog.getBookInfo();
        // 必填字段校验
        if (info[1].isEmpty() || info[2].isEmpty() || info[10].toInt() < 0) {
            QMessageBox::warning(this, "警告", "ISBN、图书名称不能为空，库存不能为负！");
            return;
        }

        // 获取图书ID，如果为空则使用-1（表示自增）
        int book_id = -1;
        if (!info[0].isEmpty()) {
            book_id = info[0].toInt();
        }

        // 调用DBHelper添加
        bool success = dbHelper->insertBook(
            book_id, info[1], info[2], info[3], info[4], info[5],
            info[6], info[7], info[8], info[9].toDouble(), info[10].toInt(),
            info[11], info[12].toDouble(), info[13], info[14]
            );

        if (success) {
            QMessageBox::information(this, "成功", "图书添加成功！");
            bookModel->select();
        } else {
            QMessageBox::critical(this, "失败",
                                  QString("图书添加失败：%1").arg(dbHelper->getDatabase().lastError().text()));
        }
    }
}

// 修改图书
// 修改图书
void Bookmgr::on_btnchange_clicked()
{
    int book_id = getSelectedBookId();
    if (book_id == -1) return;

    BookEditDialog dialog(this, false, book_id);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList info = dialog.getBookInfo();
        if (info[2].isEmpty() || info[10].toInt() < 0) {
            QMessageBox::warning(this, "警告", "图书名称不能为空，库存不能为负！");
            return;
        }

        bool success = dbHelper->updateBook(
            book_id, info[1], info[2], info[3], info[4], info[5],
            info[6], info[7], info[8], info[9].toDouble(), info[10].toInt(),
            info[11], info[12].toDouble(), info[13], info[14]
            );

        if (success) {
            QMessageBox::information(this, "成功", "图书修改成功！");
            bookModel->select();
        } else {
            QMessageBox::critical(this, "失败",
                                  QString("图书修改失败：%1").arg(dbHelper->getDatabase().lastError().text()));
        }
    }
}

// 删除图书
void Bookmgr::on_btndelete_clicked()
{
    int book_id = getSelectedBookId();
    if (book_id == -1) return;
    if (QMessageBox::question(this, "确认", "确定要删除该图书吗？（已借阅的图书无法删除）", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }
    bool success = dbHelper->deleteBook(book_id);
    if (success) {
        QMessageBox::information(this, "成功", "图书删除成功！");
        bookModel->select();
    } else {
        QMessageBox::critical(this, "失败", "图书删除失败（可能存在未归还的借阅记录）！");
    }
}

// 借书功能（补充due_date和operator_id）
void Bookmgr::on_btnborrow_clicked()
{
    int book_id = getSelectedBookId();
    if (book_id == -1) return;

    // 输入读者ID
    bool ok;
    QString useridStr = QInputDialog::getText(this, "借书", "请输入读者ID：", QLineEdit::Normal, "", &ok);
    if (!ok || useridStr.isEmpty()) return;
    int user_id = useridStr.toInt();

    // 检查读者是否存在且状态正常
    QSqlQuery userQuery = dbHelper->filterUsers(useridStr);
    if (!userQuery.next() || userQuery.value("account_status").toString() != "normal") {
        QMessageBox::warning(this, "警告", "该读者不存在或账户已禁用！");
        return;
    }

    // 计算借阅日期和应还日期（默认30天）
    QString borrow_date = QDate::currentDate().toString("yyyy-MM-dd");
    QString due_date = QDate::currentDate().addDays(OVERDUE_DAYS).toString("yyyy-MM-dd");

    // 执行借书
    bool success = dbHelper->borrowBook(book_id, user_id, m_operatorId, borrow_date, due_date);
    if (success) {
        QMessageBox::information(this, "成功",
                                 QString("借书成功！\n借阅日期：%1\n应还日期：%2").arg(borrow_date).arg(due_date));
        bookModel->select();
        // ========== 新增：发射借阅成功信号 ==========
        emit borrowReturnSuccess();
    } else {
        QMessageBox::critical(this, "失败", "借书失败（库存不足或数据错误）！");
    }
}

// 还书功能（补充逾期罚金计算）
void Bookmgr::on_btnreturn_clicked()
{
    int book_id = getSelectedBookId();
    if (book_id == -1) return;

    // 输入读者ID
    bool ok;
    QString useridStr = QInputDialog::getText(this, "还书", "请输入读者ID：", QLineEdit::Normal, "", &ok);
    if (!ok || useridStr.isEmpty()) return;
    int user_id = useridStr.toInt();

    // 执行还书（增加SQL执行结果校验）
    bool success = dbHelper->returnBook(book_id, user_id);
    if (success) {
        // 强制重新查询，确保获取最新的归还状态
        QSqlQuery query;
        query.prepare(R"(
            SELECT overdue_days, fine_amount, borrow_status FROM BorrowRecord
            WHERE book_id=:book_id AND user_id=:user_id AND borrow_status IN ('returned', 'overdue')
            ORDER BY return_date DESC LIMIT 1
        )");
        query.bindValue(":book_id", book_id);
        query.bindValue(":user_id", user_id);
        // 确保查询成功才显示弹窗
        if (query.exec() && query.next()) {
            int overdue_days = query.value("overdue_days").toInt();
            double fine_amount = query.value("fine_amount").toDouble();
            QString msg = QString("还书成功！\n归还日期：%1").arg(QDate::currentDate().toString("yyyy-MM-dd"));
            if (overdue_days > 0) {
                msg += QString("\n逾期天数：%1天\n应缴罚金：%2元").arg(overdue_days).arg(fine_amount, 0, 'f', 2);
            }
            QMessageBox::information(this, "成功", msg);
            bookModel->select(); // 刷新图书库存
            // ========== 确保信号发射：放在查询成功后，万无一失 ==========
            emit borrowReturnSuccess();
        } else {
            QMessageBox::warning(this, "提示", "还书成功，但查询状态失败！");
            bookModel->select();
            emit borrowReturnSuccess(); // 即使查询失败，也发射信号刷新
        }
    } else {
        QMessageBox::critical(this, "失败", "还书失败（无匹配借阅记录）！");
    }
}
// 筛选图书
void Bookmgr::on_selectEdit_textChanged(const QString &arg1)
{
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterFixedString(arg1);
}
