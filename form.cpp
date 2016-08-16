#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

Form::~Form()
{
    delete ui;
}

void Form::on_pushButtonM_save_clicked()
{
    emit sendData(receipt, ui->lineEditM_name->text().trimmed(),
                  ui->lineEditM_phone_num->text().trimmed(),
                  ui->lineEditM_personnel_id->text().trimmed(),
                  ui->lineEditM_fname->text().trimmed()
                  );
    this->close();
}

void Form::on_pushButtonM_cancel_clicked()
{
    this->close();
}

void Form::setUiValues()
{
    ui->lineEditM_name->setText(name);
    ui->lineEditM_fname->setText(fname);
    ui->lineEditM_phone_num->setText(phone);
    ui->lineEditM_personnel_id->setText(pid);
}
