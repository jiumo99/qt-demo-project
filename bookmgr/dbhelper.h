#ifndef DBHELPER_H
#define DBHELPER_H
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QVariantList>

// 逾期天数阈值（可配置）
#define OVERDUE_DAYS 30

class DBHelper : public QObject
{
    Q_OBJECT
public:
    explicit DBHelper(QObject *parent = nullptr);
    ~DBHelper();
    QSqlDatabase getDatabase() const { return db; }
    // 数据库连接状态
    bool isConnected() const;
    // ========== 图书表(book)操作 ==========
    // 添加图书
    bool insertBook(int bookid, const QString& type1, const QString& type2, const QString& type3,
                    const QString& pic, const QString& name, const QString& press, int cnt);
    // 修改图书
    bool updateBook(int bookid, const QString& type1, const QString& type2, const QString& type3,
                    const QString& pic, const QString& name, const QString& press, int cnt);
    // 删除图书
    bool deleteBook(int bookid);
    // 查询所有图书（返回QSqlQuery，用于Model绑定）
    QSqlQuery queryAllBooks();
    // 按条件筛选图书（支持名称、类型、出版社多条件）
    QSqlQuery filterBooks(const QString& key, const QString& type = "all");

    // ========== 读者表(user)操作 ==========
    // 添加读者
    bool insertUser(int userid, const QString& username, const QString& password,
                    const QString& nickname, const QString& auth, const QString& department, const QString& grade);
    // 删除读者
    bool deleteUser(int userid);
    // 查询所有读者
    QSqlQuery queryAllUsers();
    // 筛选读者（用户名/昵称/部门）
    QSqlQuery filterUsers(const QString& key);
    // 验证登录（用户名+密码）
    bool verifyLogin(const QString& username, const QString& password);

    // ========== 借阅记录表(record)操作 ==========
    // 新增借阅记录（未归还，end为NULL）
    bool borrowBook(int bookid, int userid, const QString& startDate);
    // 归还图书（更新end日期）
    bool returnBook(int bookid, int userid);
    // 查询所有借阅记录
    QSqlQuery queryAllRecords();
    // 筛选借阅记录（读者ID/图书ID/是否逾期）
    QSqlQuery filterRecords(const QString& key, bool isOverdue = false);
    // 统计借阅数据（返回：总借阅数、逾期数、热门图书TOP3）
    QVariantMap statisticsBorrow();
    // 查询逾期记录
    QSqlQuery queryOverdueRecords();

private:
    // 创建数据库表（如果不存在）
    void createTables();
    QSqlDatabase db;
};

#endif // DBHELPER_H
