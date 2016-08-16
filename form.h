#ifndef FORM_H
#define FORM_H

#include <QWidget>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    QString name;
    QString phone;
    QString pid;
    QString fname;
    QString receipt;
    ~Form();
    void setUiValues();

signals:
    void sendData(QString receipt, QString name, QString phone, QString pid, QString fname);


private slots:
    void on_pushButtonM_save_clicked();

    void on_pushButtonM_cancel_clicked();

private:
    Ui::Form *ui;
};

#endif // FORM_H
