#include "dbhelper.h"
#include <QDebug>

DBHelper::DBHelper(QObject *parent)
    : QObject{parent}
{
    // 连接SQLite数据库
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:\\Qt\\book\\bookmgr\\book.db");
    if (db.open()) {
        qDebug() << "数据库连接成功";
        createTables(); // 初始化表结构
    } else {
        qDebug() << "数据库连接失败：" << db.lastError().text();
    }
}

DBHelper::~DBHelper()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool DBHelper::isConnected() const
{
    return db.isOpen();
}

void DBHelper::createTables()
{
    QSqlQuery query;

    // 1. 图书表(book)
    QString createBookTable = R"(
        CREATE TABLE IF NOT EXISTS book (
            bookid INTEGER PRIMARY KEY,
            type1 TEXT,
            type2 TEXT,
            type3 TEXT,
            pic TEXT,
            name TEXT NOT NULL,
            press TEXT,
            cnt INTEGER NOT NULL DEFAULT 0
        );
    )";

    // 2. 读者表(user)
    QString createUserTable = R"(
        CREATE TABLE IF NOT EXISTS user (
            userid INTEGER PRIMARY KEY,
            username TEXT NOT NULL UNIQUE,
            password TEXT NOT NULL,
            nickname TEXT,
            auth TEXT NOT NULL DEFAULT '学生',
            department TEXT,
            grade TEXT,
            UNIQUE(username)
        );
    )";

    // 3. 借阅记录表(record)
    QString createRecordTable = R"(
        CREATE TABLE IF NOT EXISTS record (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            bookid INTEGER NOT NULL,
            userid INTEGER NOT NULL,
            start TEXT NOT NULL,
            end TEXT,
            FOREIGN KEY(bookid) REFERENCES book(bookid),
            FOREIGN KEY(userid) REFERENCES user(userid)
        );
    )";

    // 执行建表语句
    if (!query.exec(createBookTable)) qDebug() << "创建book表失败：" << query.lastError().text();
    if (!query.exec(createUserTable)) qDebug() << "创建user表失败：" << query.lastError().text();
    if (!query.exec(createRecordTable)) qDebug() << "创建record表失败：" << query.lastError().text();
}

// ========== 图书表操作 ==========
bool DBHelper::insertBook(int bookid, const QString &type1, const QString &type2, const QString &type3, const QString &pic, const QString &name, const QString &press, int cnt)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO book (bookid, type1, type2, type3, pic, name, press, cnt)
        VALUES (:bookid, :type1, :type2, :type3, :pic, :name, :press, :cnt)
    )");
    query.bindValue(":bookid", bookid);
    query.bindValue(":type1", type1);
    query.bindValue(":type2", type2);
    query.bindValue(":type3", type3);
    query.bindValue(":pic", pic);
    query.bindValue(":name", name);
    query.bindValue(":press", press);
    query.bindValue(":cnt", cnt);

    return query.exec();
}

bool DBHelper::updateBook(int bookid, const QString &type1, const QString &type2, const QString &type3, const QString &pic, const QString &name, const QString &press, int cnt)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE book SET type1=:type1, type2=:type2, type3=:type3, pic=:pic, name=:name, press=:press, cnt=:cnt
        WHERE bookid=:bookid
    )");
    query.bindValue(":bookid", bookid);
    query.bindValue(":type1", type1);
    query.bindValue(":type2", type2);
    query.bindValue(":type3", type3);
    query.bindValue(":pic", pic);
    query.bindValue(":name", name);
    query.bindValue(":press", press);
    query.bindValue(":cnt", cnt);

    return query.exec();
}

bool DBHelper::deleteBook(int bookid)
{
    QSqlQuery query;
    query.prepare("DELETE FROM book WHERE bookid=:bookid");
    query.bindValue(":bookid", bookid);
    return query.exec();
}

QSqlQuery DBHelper::queryAllBooks()
{
    QSqlQuery query("SELECT * FROM book ORDER BY bookid ASC");
    return query;
}

QSqlQuery DBHelper::filterBooks(const QString &key, const QString &type)
{
    QSqlQuery query;
    QString sql = "SELECT * FROM book WHERE ";

    if (type == "name") {
        sql += "name LIKE :key";
    } else if (type == "type") {
        sql += "type1 LIKE :key OR type2 LIKE :key OR type3 LIKE :key";
    } else if (type == "press") {
        sql += "press LIKE :key";
    } else { // all：多条件模糊查询
        sql += "name LIKE :key OR type1 LIKE :key OR type2 LIKE :key OR type3 LIKE :key OR press LIKE :key";
    }

    query.prepare(sql);
    query.bindValue(":key", "%" + key + "%");
    query.exec();
    return query;
}

// ========== 读者表操作 ==========
bool DBHelper::insertUser(int userid, const QString &username, const QString &password, const QString &nickname, const QString &auth, const QString &department, const QString &grade)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO user (userid, username, password, nickname, auth, department, grade)
        VALUES (:userid, :username, :password, :nickname, :auth, :department, :grade)
    )");
    query.bindValue(":userid", userid);
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":nickname", nickname);
    query.bindValue(":auth", auth);
    query.bindValue(":department", department);
    query.bindValue(":grade", grade);

    return query.exec();
}

bool DBHelper::deleteUser(int userid)
{
    QSqlQuery query;
    query.prepare("DELETE FROM user WHERE userid=:userid");
    query.bindValue(":userid", userid);
    return query.exec();
}

QSqlQuery DBHelper::queryAllUsers()
{
    QSqlQuery query("SELECT * FROM user ORDER BY userid ASC");
    return query;
}

QSqlQuery DBHelper::filterUsers(const QString &key)
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT * FROM user WHERE username LIKE :key OR nickname LIKE :key OR department LIKE :key
    )");
    query.bindValue(":key", "%" + key + "%");
    query.exec();
    return query;
}

bool DBHelper::verifyLogin(const QString &username, const QString &password)
{
    QSqlQuery query;
    // 关键修改：把查询条件从 username 改成 nickname（匹配新表的“登录账号字段”）
    query.prepare("SELECT * FROM user WHERE nickname=:loginAccount AND password=:loginPwd");
    query.bindValue(":loginAccount", username); // 用户输入的“账号”对应表的 nickname 字段
    query.bindValue(":loginPwd", password);

    // 可选：添加调试信息，方便排查（登录失败时查看原因）
    if (!query.exec()) {
        qDebug() << "登录查询失败：" << query.lastError().text();
        return false;
    }
    return query.next(); // 存在匹配记录则登录成功
}

// ========== 借阅记录表操作 ==========
bool DBHelper::borrowBook(int bookid, int userid, const QString &startDate)
{
    // 1. 检查图书库存
    QSqlQuery checkCnt;
    checkCnt.prepare("SELECT cnt FROM book WHERE bookid=:bookid");
    checkCnt.bindValue(":bookid", bookid);
    if (!checkCnt.exec() || !checkCnt.next()) return false;

    int cnt = checkCnt.value(0).toInt();
    if (cnt <= 0) return false; // 库存不足

    // 2. 新增借阅记录
    QSqlQuery insertRecord;
    insertRecord.prepare(R"(
        INSERT INTO record (bookid, userid, start)
        VALUES (:bookid, :userid, :start)
    )");
    insertRecord.bindValue(":bookid", bookid);
    insertRecord.bindValue(":userid", userid);
    insertRecord.bindValue(":start", startDate);
    if (!insertRecord.exec()) return false;

    // 3. 库存减1
    QSqlQuery updateCnt;
    updateCnt.prepare("UPDATE book SET cnt = cnt - 1 WHERE bookid=:bookid");
    updateCnt.bindValue(":bookid", bookid);
    return updateCnt.exec();
}

bool DBHelper::returnBook(int bookid, int userid)
{
    // 1. 更新借阅记录的end日期
    QSqlQuery updateRecord;
    updateRecord.prepare(R"(
        UPDATE record SET end=:end WHERE bookid=:bookid AND userid=:userid AND end IS NULL
    )");
    updateRecord.bindValue(":bookid", bookid);
    updateRecord.bindValue(":userid", userid);
    updateRecord.bindValue(":end", QDate::currentDate().toString("yyyy-MM-dd"));
    if (!updateRecord.exec()) return false;

    // 2. 库存加1
    QSqlQuery updateCnt;
    updateCnt.prepare("UPDATE book SET cnt = cnt + 1 WHERE bookid=:bookid");
    updateCnt.bindValue(":bookid", bookid);
    return updateCnt.exec();
}

QSqlQuery DBHelper::queryAllRecords()
{
    QSqlQuery query(R"(
        SELECT r.id, r.bookid, b.name AS bookname, r.userid, u.nickname AS username, r.start, r.end
        FROM record r
        LEFT JOIN book b ON r.bookid = b.bookid
        LEFT JOIN user u ON r.userid = u.userid
        ORDER BY r.id DESC
    )");
    return query;
}

QSqlQuery DBHelper::filterRecords(const QString &key, bool isOverdue)
{
    QSqlQuery query;
    QString sql = R"(
        SELECT r.id, r.bookid, b.name AS bookname, r.userid, u.nickname AS username, r.start, r.end
        FROM record r
        LEFT JOIN book b ON r.bookid = b.bookid
        LEFT JOIN user u ON r.userid = u.userid
        WHERE (r.bookid LIKE :key OR r.userid LIKE :key OR b.name LIKE :key OR u.nickname LIKE :key)
    )";

    // 逾期筛选：未归还且借阅时间超过OVERDUE_DAYS天
    if (isOverdue) {
        sql += " AND r.end IS NULL AND date(r.start) < date('now', '-' || :overdue_days || ' days')";
    }

    query.prepare(sql);
    query.bindValue(":key", "%" + key + "%");
    query.bindValue(":overdue_days", OVERDUE_DAYS);
    query.exec();
    return query;
}

QVariantMap DBHelper::statisticsBorrow()
{
    QVariantMap stats;
    QSqlQuery query;

    // 1. 总借阅数
    query.exec("SELECT COUNT(*) FROM record");
    query.next();
    stats["total"] = query.value(0).toInt();

    // 2. 逾期数
    query.prepare(R"(
        SELECT COUNT(*) FROM record
        WHERE end IS NULL AND date(start) < date('now', '-' || :overdue_days || ' days')
    )");
    query.bindValue(":overdue_days", OVERDUE_DAYS);
    query.exec();
    query.next();
    stats["overdue"] = query.value(0).toInt();

    // 3. 热门图书TOP3（借阅次数最多）
    query.exec(R"(
        SELECT b.name, COUNT(r.bookid) AS borrow_cnt
        FROM book b
        LEFT JOIN record r ON b.bookid = r.bookid
        GROUP BY b.bookid
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
