#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableView>
#include <QItemDelegate>
#include <QRegExp>
#include <QMenu>
#include <form.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initToolBarLineEdits();
    ui->radioButtonFemale->setChecked(true);
    editor = "";
    localAddr = "";

    QRegExp regExpPhoneNum("1[3|4|5|7|8|][0-9]{9}"); // 判断输入是否为手机号，只允许输入正常手机号
    ui->lineEditPhone->setValidator(new QRegExpValidator(regExpPhoneNum, this));

    QRegExp pid("^(\\d{15}$|^\\d{18}$|^\\d{17}(\\d|X|x))$");
    ui->lineEditID->setValidator(new QRegExpValidator(pid, this));
}

MainWindow::~MainWindow()
{
    delete ui;
}

#ifdef Q_OS_WIN
#define LINE_EDIT_LEN_SEARCH 300
#define LINE_EDIT_LEN 200
#define LINE_EDIT_HEIGHT 50
#else
#define LINE_EDIT_LEN_SEARCH 200
#define LINE_EDIT_LEN 100
#define LINE_EDIT_HEIGHT 25
#endif

void MainWindow::initToolBarLineEdits()
{

    // search lineEdit
   {
        lineEditSearch = new QLineEdit;
        lineEditSearch->setFixedSize(LINE_EDIT_LEN_SEARCH, LINE_EDIT_HEIGHT);
        lineEditSearch->setStyleSheet("border-radius: 5px;");
        lineEditSearch->setPlaceholderText(" 查询");
        ui->mainToolBar->addWidget(lineEditSearch);
        ui->mainToolBar->addSeparator();
    }

    // editor
    {
        lineEditor = new QLineEdit;
        lineEditor->setFixedSize(LINE_EDIT_LEN, LINE_EDIT_HEIGHT);
        lineEditor->setStyleSheet("border-radius:5px; background-color: yellow;");
        lineEditor->setPlaceholderText(" 编辑人姓名");
        ui->mainToolBar->addWidget(lineEditor);
        ui->mainToolBar->addSeparator();
    }

    // serverIP

    {
        lineEditIP = new QLineEdit;
        lineEditIP->setFixedSize(LINE_EDIT_LEN, LINE_EDIT_HEIGHT);
        lineEditIP->setStyleSheet("border-radius:5px; background-color: yellow;");
        lineEditIP->setText(SERVER_IP);
        lineEditIP->setPlaceholderText(" 服务器IP地址");
        ui->mainToolBar->addWidget(lineEditIP);
        ui->mainToolBar->addSeparator();
    }

}

bool MainWindow::initDatabase(QString hostname, QString username, QString password)
{
    initDbHandle = QSqlDatabase::addDatabase("QMYSQL");
    initDbHandle.setHostName(hostname);
    initDbHandle.setUserName(username);
    initDbHandle.setPassword(password);

    if(!initDbHandle.open()) {
        qDebug() << "func=initDatabase,hostname=" << hostname << ",username="
                 << username << ",err=" << initDbHandle.lastError().text();
        return false;
    }

    QSqlQuery query(initDbHandle);
    QFile sqlFile(":/files/init.sql");
    sqlFile.open(QIODevice::ReadOnly);
    QTextStream in(&sqlFile);
    QString text = in.readAll();
    QStringList list = text.split(";");

    foreach (QString str, list) {
        QString sql = str.replace("\n", "");
        if (str.isEmpty()) continue;
        query.exec(sql);
    }

    sqlFile.close();

    return true;
}

bool MainWindow::connDatabase(QString hostname, QString username, QString password)
{
    connDbHandle = QSqlDatabase::addDatabase("QMYSQL");
    connDbHandle.setDatabaseName(DB_NAME);
    connDbHandle.setHostName(hostname);
    connDbHandle.setUserName(username);
    connDbHandle.setPassword(password);

    if(!connDbHandle.open()) {
        qDebug() << "func=connDatabase,hostname=" << hostname << ",username="
                 << username << ",err=" << connDbHandle.lastError().text();
        return false;
    }

    qDebug() << connDbHandle.databaseName();

    setQueryModel();

    getConfig();

    return true;
}

bool MainWindow::portTest(QString ip, int port)
{
    QTcpSocket tsock;
    tsock.connectToHost(ip, port);
    bool ret = tsock.waitForConnected(1000);
    if (ret) tsock.close();
    qDebug() << "func=portTest,ip=" << ip << ",port=" << port << ",stat=" << ret;
    return ret;
}

void MainWindow::setQueryModel()
{

    QString filter = QString(" editor = '%1' and data_ready_mark != '%2'").arg(editor).arg(DATA_READY_MARK_DEL);
    maleModel = new QSqlTableModel(this);
    maleModel->setTable("zen_male");
    maleModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    maleModel->setSort(0, Qt::AscendingOrder);
    maleModel->setHeaderData(1,  Qt::Horizontal, "手环号");
    maleModel->setHeaderData(2,  Qt::Horizontal, "姓名");
    maleModel->setHeaderData(3,  Qt::Horizontal, "手机号");
    maleModel->setHeaderData(4,  Qt::Horizontal, "性别");
    maleModel->setHeaderData(5,  Qt::Horizontal, "法名");
    maleModel->setHeaderData(13,  Qt::Horizontal, "皈依证号码");
    maleModel->setHeaderData(16,  Qt::Horizontal, "身份证号码");
    maleModel->setHeaderData(48, Qt::Horizontal, "更改时间");

    maleModel->setFilter(filter);
    maleModel->select();
    // ui->tableViewMale->setItemDelegateForColumn;
    ui->tableViewMale->setModel(maleModel);
    // hide useless columns here
    ui->tableViewMale->hideColumn(0);
    for(int i = 14; i < 53; i++) {
        if (i == 16 || i == 48) continue;
        ui->tableViewMale->hideColumn(i);
    }

    for(int i = 6; i < 13; i++) {
        ui->tableViewMale->hideColumn(i);
    }

    ui->tableViewMale->reset();


    femaleModel = new QSqlTableModel(this);
    femaleModel->setTable("zen_female");
    femaleModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    femaleModel->setSort(0, Qt::AscendingOrder);
    femaleModel->setHeaderData(1,  Qt::Horizontal, "手环号");
    femaleModel->setHeaderData(2,  Qt::Horizontal, "姓名");
    femaleModel->setHeaderData(3,  Qt::Horizontal, "手机号");
    femaleModel->setHeaderData(4,  Qt::Horizontal, "性别");
    femaleModel->setHeaderData(5,  Qt::Horizontal, "法名");
    femaleModel->setHeaderData(13,  Qt::Horizontal, "皈依证号码");
    femaleModel->setHeaderData(16,  Qt::Horizontal, "身份证号码");
    femaleModel->setHeaderData(48, Qt::Horizontal, "更改时间");
    femaleModel->setFilter(filter);
    femaleModel->select();
    ui->tableViewFemale->setModel(femaleModel);

    // hide useless columns here
    ui->tableViewFemale->hideColumn(0);
    for(int i = 14; i < 53; i++) {
        if (i == 16 || i == 48) continue;
        ui->tableViewFemale->hideColumn(i);
    }
    for(int i = 6; i < 13; i++) {
        ui->tableViewFemale->hideColumn(i);
    }

    ui->tableViewFemale->reset();

    ui->tableViewMale->horizontalHeader()->setStretchLastSection(true);
    ui->tableViewFemale->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::closeDatabase()
{
    connDbHandle.close();
    qApp->closeAllWindows();
}

bool MainWindow::getConfig()
{
    lastMaleCode = -1;
    lastFemaleCode = -1;

    currentDatetime = QDateTime::currentDateTime();
    QString dt = currentDatetime.date().toString("yyyy-MM-dd");
    QString sql = QString(" SELECT `last_male_code`, `last_female_code`, `fahui_name` "
                          " FROM `zen_config` "
                          " WHERE `date` = '%1' ").arg(dt);
    QSqlQuery query;
    query.exec(sql);
    qDebug() << query.lastError().text();

    while(query.next()) {
        lastMaleCode = query.value(0).toInt();
        lastFemaleCode = query.value(1).toInt();
        fahuiName = query.value(2).toString();
    }

    if (lastMaleCode == -1) {
        QMessageBox::information(this, "", "未设置皈依配置信息，请联系管理员，设置成功后重新打开本程序");
        closeDatabase();
        return false;
    }

    qDebug() << lastMaleCode << lastFemaleCode;

    return true;
}

void MainWindow::clearLineEdits()
{
    ui->lineEditName->clear();
    ui->lineEditID->clear();
    ui->lineEditPhone->clear();
    ui->lineEditName->setFocus();
    ui->lineEditFname->clear();
}

void MainWindow::refreshTableView(QString tableName, QString filter)
{
    QSqlTableModel *model = (tableName == "zen_male") ? maleModel: femaleModel;
    if (filter.trimmed().isEmpty()) {
        filter = QString("editor = '%1' and data_ready_mark != '%2'").arg(editor).arg(DATA_READY_MARK_DEL);
    } else {
        filter = QString("editor = '%1' and data_ready_mark != '%2' and %3").arg(editor)
                .arg(DATA_READY_MARK_DEL)
                .arg(filter);
    }

    model->setFilter(filter);
    model->select();
    tableName == "zen_male" ? ui->tableViewMale->reset() : ui->tableViewMale->reset();
    qDebug() << "func=refreshTableView,tableName=" << tableName << ",filter=" << filter;
}


bool MainWindow::isChinese(QString name)
{
    int count = name.count();
    for (int i = 0; i < count; i++) {
        QChar ch = name.at(i);
        ushort unicode = ch.unicode();
        if (unicode >= 0x4E00 && unicode <= 0x9FA5)
            return true;
    }

    return false;
}

QString MainWindow::makeFname(QString name)
{
    if (!isChinese(name)) {
        return QString("贤德");
    }

    int order = name.length() - 1;
    return QString("贤%1").arg(name[order]);
}

void MainWindow::on_actionConnect_triggered()
{

    editor = lineEditor->text().trimmed();
    if (editor.isEmpty()) {
        QMessageBox::information(this, "", "请输入编辑人姓名");
        return;
    }

    QString hostname = lineEditIP->text().trimmed();
    int ret = connDatabase(hostname, DB_USERNAME, DB_PASSWORD);
    if (!ret) {
        QMessageBox::critical(this, "", "CONNECT DATABASE ERROR");
        return;
    }
    qDebug() << "func=on_actionConnect_triggered";
    ui->actionConnect->setDisabled(true);
    lineEditor->setDisabled(true);
    lineEditIP->setDisabled(true);

    getLocalAddr();

}

void MainWindow::on_pushButtonSave_clicked()
{
    // [ get info ]
    QString name = ui->lineEditName->text().trimmed();
    QString phone = ui->lineEditPhone->text().trimmed();
    QString gender = (ui->radioButtonMale->isChecked()) ? "男": "女";
    QString pid = ui->lineEditID->text().trimmed();
    QString fname = makeFname(name);

    if (!ui->lineEditFname->text().isEmpty()) {
        fname = ui->lineEditFname->text().trimmed();
    }

    if (name.isEmpty() || phone.isEmpty() || pid.isEmpty()) {
        QMessageBox::information(this, "", "请不要输入空记录");
        ui->lineEditName->setFocus();
        return;
    }

    QString sphone = phone;
    QString spid = pid;

    sphone.insert(3, "-");
    sphone.insert(8, "-");

    if (spid.length() == 18) {
        spid.insert(3, "-");
        spid.insert(7, "-");
        spid.insert(12, "-");
        spid.insert(17, "-");
    }

    QMessageBox msgBox;
    QString msg = QString("<font color=red>请核查，正确请点击 Yes 错误点击 No</font><br><br>姓名: %1<br>性别: %2<br>手机号: %3<br>身份证号: %4")
            .arg(name).arg(gender).arg(sphone).arg(spid);

    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int ret = msgBox.exec();
    switch(ret) {
    case QMessageBox::Yes:
        break;
    case QMessageBox::No:
        return;
    default:
        return;
    }

    int id = 0;
    QString tableName;
    QString receipt = "";
    tableName = (ui->radioButtonFemale->isChecked()) ? "zen_female": "zen_male";

    QSqlQuery query;
    QString findSql = QString("select id, receipt from %1 where data_ready_mark = %2 and editor = '%3'")
            .arg(tableName).arg(DATA_READY_MARK_DEL).arg(editor);
    query.exec(findSql);
    while(query.next()) {
        id = query.value(0).toInt();
        receipt = query.value(1).toString();
        break;
    }

    clearLineEdits();

    if (id) {
        QString updateSql = QString("update %1 set name = '%2', gender = '%3', phone_num = '%4', "
                                    " personnel_id = '%5', data_ready_mark = '%6', mod_time = '%7', editor = '%8', "
                                    " fname = '%9' where id = '%10' ")
                .arg(tableName)
                .arg(name)
                .arg(gender)
                .arg(phone)
                .arg(pid)
                .arg(DATA_READY_MARK_REUSE)
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:MM:ss"))
                .arg(editor)
                .arg(fname)
                .arg(id);
        query.exec(updateSql);
        qDebug() << updateSql << query.lastError().text();
    } else {
        // just insert a new row
        QString insertSql = QString("insert into %1 (name, gender, phone_num, personnel_id, fname, editor, ipaddress, "
                                    " fahui_name ) values ("
                                    " '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9')")
                .arg(tableName).arg(name).arg(gender).arg(phone).arg(pid).arg(fname).arg(editor).arg(localAddr).arg(fahuiName);
        query.exec(insertSql);
        qDebug () << query.lastError().text();
        id = query.lastInsertId().toInt();

        int lastCode;
        char header;

        if (gender == "男") {
            header = 'A';
            lastCode = lastMaleCode;
        } else {
            header = 'B';
            lastCode = lastFemaleCode;
        }

        QString code;
        code.sprintf("%c%010d", header, id + lastCode);

        receipt.sprintf("%c%04d", header, id);

        QString upReceiptCodeSql = QString("update %1 set receipt = '%2', code = '%3' where id = '%4' ")
                .arg(tableName).arg(receipt).arg(code).arg(id);
        query.exec(upReceiptCodeSql);

    }

    QMessageBox mbox;
    mbox.setText(QString("姓名: %1 性别: %2 手环号:<font color=red>%3</font>").arg(name).arg(gender).arg(receipt));
    mbox.exec();
    refreshTableView(tableName, "");
}

void MainWindow::on_actionQuery_triggered()
{
    QString search = lineEditSearch->text().trimmed();
    qDebug() << "search =" << search;

    // empty
    if (search.isEmpty()) {
        refreshTableView("zen_male", "");
        refreshTableView("zen_female", "");
        return;
    }

    // name
    if (isChinese(search)) {
        refreshTableView("zen_male", QString(" name = '%1'").arg(search));
        refreshTableView("zen_female", QString(" name = '%1'").arg(search));

        return;
    }

    // phone

    if (search.startsWith("1") && search.length() == 11) {
        QString sql = QString(" phone_num = '%1'").arg(search);
        refreshTableView("zen_male", sql);
        refreshTableView("zen_female", sql);
        return;
    }

    // receipt
    if (search.startsWith("A")) {
        refreshTableView("zen_male", QString(" receipt = '%1'").arg(search));
        return;
    } else if (search.startsWith("B")) {
        refreshTableView("zen_female", QString(" receipt = '%1'").arg(search));
        return;
    }

    // sql where
    if (search.startsWith("sql:")) {
        QString sql = search.section(":", 1);
        refreshTableView("zen_male", sql);
        refreshTableView("zen_female", sql);
        return;
    }
}

void MainWindow::viewPopMenu()
{
    QMenu *popMenu = new QMenu(this);
    popMenu->addAction(ui->actionSetResue);
    popMenu->addAction(ui->actionModify);
    popMenu->exec(QCursor::pos());

    delete popMenu;
}

void MainWindow::getLocalAddr()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();

    for(int i = 0; i < list.count(); i++) {
        if(!list[i].isLoopback())
            if (list[i].protocol() == QAbstractSocket::IPv4Protocol ) {
                QString ip = list[i].toString();
                qDebug() << ip;
                if (ip.startsWith("192.168")) {
                    localAddr = ip;
                }
            }
    }

    if (lineEditIP->text().trimmed().startsWith("127")) {
        localAddr = "127.0.0.1";
    }
}

bool MainWindow::setRowReuseStat(QString receipt)
{
    QSqlQuery query;
    QString sql;
    if (receipt.startsWith("A")) {
        sql = QString("update zen_male set name = '已删除(请不要修改)', data_ready_mark = '%1' where receipt = '%2'").arg(DATA_READY_MARK_DEL).arg(receipt);
    } else {
        sql = QString("update zen_female set name = '已删除(请不要修改)', data_ready_mark = '%1' where receipt = '%2'").arg(DATA_READY_MARK_DEL).arg(receipt);
    }

    query.exec(sql);
    qDebug() << query.lastError().text() << sql;
    if (query.lastError().text().isEmpty()) return true;
    else return false;
}

void MainWindow::on_tableViewMale_customContextMenuRequested(const QPoint &pos)
{
    int rowNum = ui->tableViewMale->verticalHeader()->logicalIndexAt(pos);

    QString receipt = maleModel->index(rowNum, 1).data().toString();
    qDebug() << "receipt: " << receipt;
    globalReceipt = receipt;
    viewPopMenu();
    globalReceipt = "";
}

void MainWindow::on_tableViewFemale_customContextMenuRequested(const QPoint &pos)
{
    int rowNum = ui->tableViewFemale->verticalHeader()->logicalIndexAt(pos);

    QString receipt = femaleModel->index(rowNum, 1).data().toString();
    qDebug() << "receipt: " << receipt;
    globalReceipt = receipt;
    viewPopMenu();
    globalReceipt = "";
}

void MainWindow::on_actionSetResue_triggered()
{
    setRowReuseStat(globalReceipt);
    if (globalReceipt.startsWith("A")) refreshTableView("zen_male", "");
    else refreshTableView("zen_female", "");
    qDebug() << "setRowReuseStat:" << globalReceipt;
}

void MainWindow::on_actionModify_triggered()
{
    Form *form = new Form();
    form->setWindowModality(Qt::ApplicationModal);
    connect(form, SIGNAL(sendData(QString,QString,QString,QString, QString)), this, SLOT(receiveData(QString,QString,QString,QString, QString)));

    QSqlQuery query;
    QString tableName = globalReceipt.startsWith("A") ? "zen_male": "zen_female";
    qDebug() << tableName;
    QString sql = QString("select name, gender, phone_num as phone, personnel_id as pid, fname from %1 where receipt = '%2'").arg(tableName).arg(globalReceipt);
    qDebug() << sql;
    query.exec(sql);

    while(query.next()) {
        form->name = query.value(0).toString();
        form->phone = query.value(2).toString();
        form->pid = query.value(3).toString();
        form->fname = query.value(4).toString();
    }

    form->receipt = globalReceipt;

    form->setUiValues();
    form->show();
}

void MainWindow::receiveData(QString receipt, QString name, QString phone, QString pid, QString fname)
{
    qDebug() << "receiveData" << receipt << name << phone << pid << fname;
    QSqlQuery query;
    QString sql;
    QString tableName = receipt.startsWith("A") ? "zen_male": "zen_female";

    sql = QString("update %1 set name = '%2', phone_num = '%3', personnel_id = '%4', fname = '%5' where receipt = '%6'").arg(tableName)
            .arg(name).arg(phone).arg(pid).arg(fname).arg(receipt);
    query.exec(sql);
    qDebug() << query.lastError().text();
    refreshTableView(tableName, "");
}
