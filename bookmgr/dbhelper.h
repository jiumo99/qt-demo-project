#ifndef DBHELPER_H
#define DBHELPER_H
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QVariantList>

// 逾期天数阈值（与新表overdue_fee_rate配合）
#define OVERDUE_DAYS 30
// 默认操作员ID（管理员ID，对应之前插入的admin001）
#define DEFAULT_OPERATOR_ID 1

class DBHelper : public QObject
{
    Q_OBJECT
public:
    explicit DBHelper(QObject *parent = nullptr);
    ~DBHelper();
    QSqlDatabase getDatabase() const { return db; }
    bool isConnected() const;

    // ========== 图书表(Book)操作（适配新表字段） ==========
    bool insertBook(int book_id, const QString& isbn, const QString& book_name, const QString& author,
                    const QString& publisher, const QString& publish_date, const QString& category,
                    const QString& sub_category, const QString& tags, double price, int stock,
                    const QString& book_status, double overdue_fee_rate, const QString& cover_path,
                    const QString& description);
    bool updateBook(int book_id, const QString& isbn, const QString& book_name, const QString& author,
                    const QString& publisher, const QString& publish_date, const QString& category,
                    const QString& sub_category, const QString& tags, double price, int stock,
                    const QString& book_status, double overdue_fee_rate, const QString& cover_path,
                    const QString& description);
    bool deleteBook(int book_id);
    QSqlQuery queryAllBooks();
    QSqlQuery filterBooks(const QString& key, const QString& type = "all");

    // ========== 用户表(User)操作（适配新表字段） ==========
    bool insertUser(int user_id, const QString& username, const QString& password, const QString& nickname,
                    const QString& role, const QString& class_, const QString& major, const QString& gender,
                    const QString& phone, const QString& email, const QString& account_status);
    bool deleteUser(int user_id);
    QSqlQuery queryAllUsers();
    QSqlQuery filterUsers(const QString& key);
    bool verifyLogin(const QString& username, const QString& password);
    // 新增：根据用户名获取用户ID（用于登录后关联operator_id）
    int getUserIdByUsername(const QString& username);

    // ========== 借阅记录表(BorrowRecord)操作（适配新表字段） ==========
    bool borrowBook(int book_id, int user_id, int operator_id, const QString& borrow_date, const QString& due_date);
    bool returnBook(int book_id, int user_id);
    QSqlQuery queryAllRecords();
    QSqlQuery filterRecords(const QString& key, bool isOverdue = false);
    QVariantMap statisticsBorrow();
    QSqlQuery queryOverdueRecords();

private:
    void createTables(); // 同步创建新表结构
    QSqlDatabase db;
};
#endif // DBHELPER_H
