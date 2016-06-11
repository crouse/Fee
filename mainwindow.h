#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSql>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#define SERVER_IP "192.168.1.5"

#define DB_USERNAME "citta"
#define DB_PASSWORD "attic"
#define DB_NAME "citta"
#define DATA_READY_MARK_DEL 1
#define DATA_READY_MARK_REUSE 2


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QLineEdit *lineEditSearch;
    QLineEdit *lineEditor;
    QLineEdit *lineEditIP;

    QSqlDatabase initDbHandle;
    QSqlDatabase connDbHandle;

    QSqlTableModel *maleModel;
    QSqlTableModel *femaleModel;

    int lastMaleCode;
    int lastFemaleCode;

    QString fahuiName;
    QString editor;
    QString globalReceipt;
    QString localAddr;

    QDateTime currentDatetime;

    bool initDatabase(QString hostname, QString username, QString password);
    bool connDatabase(QString hostname, QString username, QString password);
    bool portTest(QString ip, int port);
    bool getConfig();
    bool isChinese(QString name);

    bool setRowReuseStat(QString receipt);

    void closeDatabase();
    void refreshTableView(QString tableName, QString filter);
    void clearLineEdits();
    void initToolBarLineEdits();
    void setQueryModel();
    void viewPopMenu();
    void getLocalAddr();

    QString makeFname(QString name);


private slots:
    void on_actionConnect_triggered();

    void on_pushButtonSave_clicked();

    void on_actionQuery_triggered();

    void on_tableViewMale_customContextMenuRequested(const QPoint &pos);

    void on_tableViewFemale_customContextMenuRequested(const QPoint &pos);

    void on_actionSetResue_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
