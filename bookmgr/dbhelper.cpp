#include "dbhelper.h"
#include <QDebug>

DBHelper::DBHelper(QObject *parent)
    : QObject{parent}
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:\\Qt\\book\\bookmgr\\book.db");
    if (db.open()) {
        qDebug() << "数据库连接成功";
        createTables();
    } else {
        qDebug() << "数据库连接失败：" << db.lastError().text();
    }
}

DBHelper::~DBHelper()
{
    if (db.isOpen()) db.close();
}

bool DBHelper::isConnected() const
{
    return db.isOpen();
}

// 同步创建新表结构
void DBHelper::createTables()
{
    QSqlQuery query;
    // 1. 用户表(User)
    QString createUserTable = R"(
        CREATE TABLE IF NOT EXISTS User (
            user_id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            nickname TEXT NOT NULL,
            role TEXT NOT NULL CHECK(role IN ('admin', 'student')),
            class TEXT DEFAULT '',
            major TEXT DEFAULT '',
            gender TEXT DEFAULT '未知' CHECK(gender IN ('男', '女', '未知')),
            phone TEXT DEFAULT '',
            email TEXT DEFAULT '',
            account_status TEXT NOT NULL DEFAULT 'normal' CHECK(account_status IN ('normal', 'disabled')),
            create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
            update_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
        );
    )";
    // 2. 图书表(Book)
    QString createBookTable = R"(
        CREATE TABLE IF NOT EXISTS Book (
            book_id INTEGER PRIMARY KEY AUTOINCREMENT,
            isbn TEXT UNIQUE NOT NULL,
            book_name TEXT NOT NULL,
            author TEXT NOT NULL DEFAULT '',
            publisher TEXT NOT NULL DEFAULT '',
            publish_date DATE DEFAULT NULL,
            category TEXT NOT NULL DEFAULT '',
            sub_category TEXT NOT NULL DEFAULT '',
            tags TEXT DEFAULT '',
            price DECIMAL(10,2) NOT NULL DEFAULT 0.00,
            stock INTEGER NOT NULL DEFAULT 0 CHECK(stock >= 0),
            book_status TEXT NOT NULL DEFAULT 'available' CHECK(book_status IN ('available', 'damaged', 'lost')),
            overdue_fee_rate DECIMAL(5,2) NOT NULL DEFAULT 0.50,
            cover_path TEXT DEFAULT NULL,
            description TEXT DEFAULT '',
            create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
            update_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
        );
    )";
    // 3. 借阅记录表(BorrowRecord) - 新结构
    QString createRecordTable = R"(
        CREATE TABLE IF NOT EXISTS BorrowRecord (
            record_id INTEGER PRIMARY KEY AUTOINCREMENT,
            book_id INTEGER NOT NULL,
            user_id INTEGER NOT NULL,
            operator_id INTEGER NOT NULL,
            borrow_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
            due_date DATETIME NOT NULL,
            return_date DATETIME DEFAULT NULL,
            borrow_status TEXT NOT NULL CHECK(borrow_status IN ('borrowed', 'returned', 'overdue', 'lost')),
            overdue_days INTEGER DEFAULT 0 CHECK(overdue_days >= 0),
            fine_amount DECIMAL(10,2) DEFAULT 0.00 CHECK(fine_amount >= 0),
            remarks TEXT DEFAULT '',
            FOREIGN KEY(book_id) REFERENCES Book(book_id) ON DELETE RESTRICT,
            FOREIGN KEY(user_id) REFERENCES User(user_id) ON DELETE RESTRICT,
            FOREIGN KEY(operator_id) REFERENCES User(user_id) ON DELETE RESTRICT
        );
    )";
    // 4. 索引（按新表优化）
    QString createIndexes = R"(
        CREATE INDEX IF NOT EXISTS idx_book_category ON Book(category);
        CREATE INDEX IF NOT EXISTS idx_book_sub_category ON Book(sub_category);
        CREATE INDEX IF NOT EXISTS idx_book_name ON Book(book_name);
        CREATE INDEX IF NOT EXISTS idx_borrow_user ON BorrowRecord(user_id);
        CREATE INDEX IF NOT EXISTS idx_borrow_book ON BorrowRecord(book_id);
        CREATE INDEX IF NOT EXISTS idx_borrow_status ON BorrowRecord(borrow_status);
        CREATE INDEX IF NOT EXISTS idx_borrow_due_date ON BorrowRecord(due_date);
    )";

    if (!query.exec(createUserTable)) qDebug() << "创建User表失败：" << query.lastError().text();
    if (!query.exec(createBookTable)) qDebug() << "创建Book表失败：" << query.lastError().text();
    if (!query.exec(createRecordTable)) qDebug() << "创建BorrowRecord表失败：" << query.lastError().text();
    if (!query.exec(createIndexes)) qDebug() << "创建索引失败：" << query.lastError().text();
}

// ========== 图书表(Book)操作 ==========
bool DBHelper::insertBook(int book_id, const QString& isbn, const QString& book_name, const QString& author,
                          const QString& publisher, const QString& publish_date, const QString& category,
                          const QString& sub_category, const QString& tags, double price, int stock,
                          const QString& book_status, double overdue_fee_rate, const QString& cover_path,
                          const QString& description)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO Book (book_id, isbn, book_name, author, publisher, publish_date, category,
                          sub_category, tags, price, stock, book_status, overdue_fee_rate, cover_path, description)
        VALUES (:book_id, :isbn, :book_name, :author, :publisher, :publish_date, :category,
                :sub_category, :tags, :price, :stock, :book_status, :overdue_fee_rate, :cover_path, :description)
    )");
    query.bindValue(":book_id", book_id);
    query.bindValue(":isbn", isbn);
    query.bindValue(":book_name", book_name);
    query.bindValue(":author", author);
    query.bindValue(":publisher", publisher);
    query.bindValue(":publish_date", publish_date);
    query.bindValue(":category", category);
    query.bindValue(":sub_category", sub_category);
    query.bindValue(":tags", tags);
    query.bindValue(":price", price);
    query.bindValue(":stock", stock);
    query.bindValue(":book_status", book_status);
    query.bindValue(":overdue_fee_rate", overdue_fee_rate);
    query.bindValue(":cover_path", cover_path);
    query.bindValue(":description", description);
    return query.exec();
}

bool DBHelper::updateBook(int book_id, const QString& isbn, const QString& book_name, const QString& author,
                          const QString& publisher, const QString& publish_date, const QString& category,
                          const QString& sub_category, const QString& tags, double price, int stock,
                          const QString& book_status, double overdue_fee_rate, const QString& cover_path,
                          const QString& description)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE Book SET isbn=:isbn, book_name=:book_name, author=:author, publisher=:publisher,
                        publish_date=:publish_date, category=:category, sub_category=:sub_category,
                        tags=:tags, price=:price, stock=:stock, book_status=:book_status,
                        overdue_fee_rate=:overdue_fee_rate, cover_path=:cover_path, description=:description
        WHERE book_id=:book_id
    )");
    query.bindValue(":book_id", book_id);
    query.bindValue(":isbn", isbn);
    query.bindValue(":book_name", book_name);
    query.bindValue(":author", author);
    query.bindValue(":publisher", publisher);
    query.bindValue(":publish_date", publish_date);
    query.bindValue(":category", category);
    query.bindValue(":sub_category", sub_category);
    query.bindValue(":tags", tags);
    query.bindValue(":price", price);
    query.bindValue(":stock", stock);
    query.bindValue(":book_status", book_status);
    query.bindValue(":overdue_fee_rate", overdue_fee_rate);
    query.bindValue(":cover_path", cover_path);
    query.bindValue(":description", description);
    return query.exec();
}

bool DBHelper::deleteBook(int book_id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM Book WHERE book_id=:book_id");
    query.bindValue(":book_id", book_id);
    return query.exec();
}

QSqlQuery DBHelper::queryAllBooks()
{
    QSqlQuery query("SELECT * FROM Book ORDER BY book_id ASC");
    return query;
}

QSqlQuery DBHelper::filterBooks(const QString& key, const QString& type)
{
    QSqlQuery query;
    QString sql = "SELECT * FROM Book WHERE ";
    if (type == "name") sql += "book_name LIKE :key";
    else if (type == "category") sql += "category LIKE :key OR sub_category LIKE :key";
    else if (type == "publisher") sql += "publisher LIKE :key";
    else sql += "book_name LIKE :key OR category LIKE :key OR sub_category LIKE :key OR publisher LIKE :key OR isbn LIKE :key";
    query.prepare(sql);
    query.bindValue(":key", "%" + key + "%");
    query.exec();
    return query;
}

// ========== 用户表(User)操作 ==========
bool DBHelper::insertUser(int user_id, const QString& username, const QString& password, const QString& nickname,
                          const QString& role, const QString& class_, const QString& major, const QString& gender,
                          const QString& phone, const QString& email, const QString& account_status)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO User (user_id, username, password, nickname, role, class, major, gender, phone, email, account_status)
        VALUES (:user_id, :username, :password, :nickname, :role, :class, :major, :gender, :phone, :email, :account_status)
    )");
    query.bindValue(":user_id", user_id);
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":nickname", nickname);
    query.bindValue(":role", role);
    query.bindValue(":class", class_);
    query.bindValue(":major", major);
    query.bindValue(":gender", gender);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);
    query.bindValue(":account_status", account_status);
    return query.exec();
}

bool DBHelper::deleteUser(int user_id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM User WHERE user_id=:user_id");
    query.bindValue(":user_id", user_id);
    return query.exec();
}

QSqlQuery DBHelper::queryAllUsers()
{
    QSqlQuery query("SELECT * FROM User ORDER BY user_id ASC");
    return query;
}

QSqlQuery DBHelper::filterUsers(const QString& key)
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT * FROM User WHERE username LIKE :key OR nickname LIKE :key OR major LIKE :key OR class LIKE :key
    )");
    query.bindValue(":key", "%" + key + "%");
    query.exec();
    return query;
}

bool DBHelper::verifyLogin(const QString& username, const QString& password)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE username=:username AND password=:password AND account_status='normal'");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    if (!query.exec()) {
        qDebug() << "登录查询失败：" << query.lastError().text();
        return false;
    }
    return query.next();
}

int DBHelper::getUserIdByUsername(const QString& username)
{
    QSqlQuery query;
    query.prepare("SELECT user_id FROM User WHERE username=:username");
    query.bindValue(":username", username);
    if (query.exec() && query.next()) {
        return query.value("user_id").toInt();
    }
    return DEFAULT_OPERATOR_ID; // 默认管理员ID
}

// ========== 借阅记录表(BorrowRecord)操作 ==========
bool DBHelper::borrowBook(int book_id, int user_id, int operator_id, const QString& borrow_date, const QString& due_date)
{
    // 1. 检查图书库存和状态
    QSqlQuery checkBook;
    checkBook.prepare("SELECT stock, book_status FROM Book WHERE book_id=:book_id");
    checkBook.bindValue(":book_id", book_id);
    if (!checkBook.exec() || !checkBook.next()) return false;
    int stock = checkBook.value("stock").toInt();
    QString book_status = checkBook.value("book_status").toString();
    if (stock <= 0 || book_status != "available") return false;

    // 2. 新增借阅记录（状态为borrowed）
    QSqlQuery insertRecord;
    insertRecord.prepare(R"(
        INSERT INTO BorrowRecord (book_id, user_id, operator_id, borrow_date, due_date, borrow_status)
        VALUES (:book_id, :user_id, :operator_id, :borrow_date, :due_date, 'borrowed')
    )");
    insertRecord.bindValue(":book_id", book_id);
    insertRecord.bindValue(":user_id", user_id);
    insertRecord.bindValue(":operator_id", operator_id);
    insertRecord.bindValue(":borrow_date", borrow_date);
    insertRecord.bindValue(":due_date", due_date);
    if (!insertRecord.exec()) return false;

    // 3. 库存减1
    QSqlQuery updateStock;
    updateStock.prepare("UPDATE Book SET stock = stock - 1 WHERE book_id=:book_id");
    updateStock.bindValue(":book_id", book_id);
    return updateStock.exec();
}
bool DBHelper::returnBook(int book_id, int user_id)
{
// 1. 查询借阅记录和图书逾期费率
QSqlQuery query;
query.prepare(R"(
        SELECT br.record_id, br.due_date, b.overdue_fee_rate
        FROM BorrowRecord br
        LEFT JOIN Book b ON br.book_id = b.book_id
        WHERE br.book_id=:book_id AND br.user_id=:user_id AND br.borrow_status='borrowed'
    )");
query.bindValue(":book_id", book_id);
query.bindValue(":user_id", user_id);
if (!query.exec() || !query.next()) return false;

// 2. 计算逾期天数和罚金（删除了borrowDate相关代码）
int record_id = query.value("record_id").toInt();
QDate dueDate = QDate::fromString(query.value("due_date").toString().left(10), "yyyy-MM-dd");
QDate returnDate = QDate::currentDate();
int overdue_days = qMax(0, dueDate.daysTo(returnDate));
double fee_rate = query.value("overdue_fee_rate").toDouble();
double fine_amount = overdue_days * fee_rate;

    // 3. 更新借阅记录
    QSqlQuery updateRecord;
    updateRecord.prepare(R"(
        UPDATE BorrowRecord SET return_date=:return_date, borrow_status=:status,
                                overdue_days=:overdue_days, fine_amount=:fine_amount
        WHERE record_id=:record_id
    )");
    updateRecord.bindValue(":return_date", returnDate.toString("yyyy-MM-dd"));
    updateRecord.bindValue(":status", overdue_days > 0 ? "overdue" : "returned");
    updateRecord.bindValue(":overdue_days", overdue_days);
    updateRecord.bindValue(":fine_amount", fine_amount);
    updateRecord.bindValue(":record_id", record_id);
    if (!updateRecord.exec()) return false;

    // 4. 库存加1
    QSqlQuery updateStock;
    updateStock.prepare("UPDATE Book SET stock = stock + 1 WHERE book_id=:book_id");
    updateStock.bindValue(":book_id", book_id);
    return updateStock.exec();
}

QSqlQuery DBHelper::queryAllRecords()
{
    QSqlQuery query(R"(
        SELECT br.record_id, br.book_id, b.book_name, br.user_id, u.nickname,
               br.borrow_date, br.due_date, br.return_date, br.borrow_status,
               br.overdue_days, br.fine_amount
        FROM BorrowRecord br
        LEFT JOIN Book b ON br.book_id = b.book_id
        LEFT JOIN User u ON br.user_id = u.user_id
        ORDER BY br.record_id DESC
    )");
    return query;
}

QSqlQuery DBHelper::filterRecords(const QString& key, bool isOverdue)
{
    QSqlQuery query;
    QString sql = R"(
        SELECT br.record_id, br.book_id, b.book_name, br.user_id, u.nickname,
               br.borrow_date, br.due_date, br.return_date, br.borrow_status,
               br.overdue_days, br.fine_amount
        FROM BorrowRecord br
        LEFT JOIN Book b ON br.book_id = b.book_id
        LEFT JOIN User u ON br.user_id = u.user_id
        WHERE (br.book_id LIKE :key OR br.user_id LIKE :key OR b.book_name LIKE :key OR u.nickname LIKE :key)
    )";
    if (isOverdue) sql += " AND br.borrow_status='overdue'";
    query.prepare(sql);
    query.bindValue(":key", "%" + key + "%");
    query.exec();
    return query;
}

QVariantMap DBHelper::statisticsBorrow()
{
    QVariantMap stats;
    QSqlQuery query;

    // 总借阅数
    query.exec("SELECT COUNT(*) FROM BorrowRecord");
    query.next();
    stats["total"] = query.value(0).toInt();

    // 逾期数
    query.exec("SELECT COUNT(*) FROM BorrowRecord WHERE borrow_status='overdue'");
    query.next();
    stats["overdue"] = query.value(0).toInt();

    // 热门图书TOP3
    query.exec(R"(
        SELECT b.book_name, COUNT(br.book_id) AS borrow_cnt
        FROM Book b
        LEFT JOIN BorrowRecord br ON b.book_id = br.book_id
        GROUP BY b.book_id
        ORDER BY borrow_cnt DESC
        LIMIT 3
    )");
    QStringList hotBooks;
    while (query.next()) {
        hotBooks << QString("%1（%2次）").arg(query.value(0).toString()).arg(query.value(1).toInt());
    }
    stats["hotBooks"] = hotBooks;
    return stats;
}

QSqlQuery DBHelper::queryOverdueRecords()
{
    return filterRecords("", true);
}
