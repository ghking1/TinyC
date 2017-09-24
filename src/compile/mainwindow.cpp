#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->open, SIGNAL(clicked()), this, SLOT(on_open_clicked));
    connect(ui->source, SIGNAL(clicked()), this, SLOT(on_source_clicked));
    connect(ui->token, SIGNAL(clicked()), this, SLOT(on_token_clicked));
    connect(ui->tree, SIGNAL(clicked()), this, SLOT(on_tree_clicked));
    connect(ui->mcode, SIGNAL(clicked()), this, SLOT(on_tree_clicked));
    connect(ui->dcode, SIGNAL(clicked()), this, SLOT(on_dcode_clicked));
    connect(ui->lexical, SIGNAL(clicked()), this, SLOT(on_lexical_clicked));
    connect(ui->grammar, SIGNAL(clicked()), this, SLOT(on_grammar_clicked));
    setWindowIcon(QIcon(":/tiny.ico"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_open_clicked()
{
    QString fileNameTemp=fileName;
    fileName=QFileDialog::getOpenFileName(this, tr("select file"), "./", tr("tiny(*tiny)"));
    if(fileName=="")
    {
        fileName=fileNameTemp;
        return;
    }
    else
    {
        ui->textBrowser->clear();
        ui->source->setEnabled(true);
    }

    myprocess1.start("./lexical.exe "+fileName);
    if(myprocess1.waitForFinished())
    {
        ui->token->setEnabled(true);
        ui->textBrowser->append("lexical succeed!\n");
    }
    else
    {
        ui->token->setDisabled(true);
        ui->textBrowser->append("lexical error!\n");
        return;
    }

    myprocess2.start("./grammer.exe");
    if(myprocess2.waitForFinished())
    {
        ui->tree->setEnabled(true);
        ui->textBrowser->append("grammar succeed!\n");
    }
    else
    {
        ui->tree->setDisabled(true);
        ui->textBrowser->append("grammar error!\n");
        return;
    }

    myprocess3.start("./mcode.exe");
    if(myprocess3.waitForFinished())
    {
        ui->mcode->setEnabled(true);
        ui->textBrowser->append("mcode succeed!\n");
    }
    else
    {
        ui->mcode->setDisabled(true);
        ui->textBrowser->append("mcode error!\n");
        return;
    }

    myprocess4.start("./dcode.exe");
    if(myprocess4.waitForFinished())
    {
        ui->dcode->setEnabled(true);
        ui->textBrowser->append("dcode succeed!\n");
    }
    else
    {
        ui->dcode->setDisabled(true);
        ui->textBrowser->append("dcode error!\n");
        return;
    }
}

void MainWindow::on_source_clicked()
{
    QFile f(fileName);
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return ;
    }

    QTextStream txtInput(&f);
    QString lineStr;
    ui->textBrowser->clear();
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        ui->textBrowser->append(lineStr);
    }

    f.close();
}

void MainWindow::on_token_clicked()
{
    QFile f("./token.txt");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return ;
    }

    QTextStream txtInput(&f);
    QString lineStr;
    ui->textBrowser->clear();
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        ui->textBrowser->append(lineStr);
    }

    f.close();
}

void MainWindow::on_tree_clicked()
{
    QFile f("./tree.txt");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return ;
    }

    QTextStream txtInput(&f);
    QString lineStr;
    ui->textBrowser->clear();
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        ui->textBrowser->append(lineStr);
    }

    f.close();
}

void MainWindow::on_mcode_clicked()
{
    QFile f("./mcode.txt");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return ;
    }

    QTextStream txtInput(&f);
    QString lineStr;
    ui->textBrowser->clear();
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        ui->textBrowser->append(lineStr);
    }

    f.close();
}

void MainWindow::on_dcode_clicked()
{
    QFile f("./dcode.txt");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return ;
    }

    QTextStream txtInput(&f);
    QString lineStr;
    ui->textBrowser->clear();
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        ui->textBrowser->append(lineStr);
    }

    f.close();
}

void MainWindow::on_lexical_clicked()
{
    QFile f("./RE.txt");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return ;
    }

    QTextStream txtInput(&f);
    QString lineStr;
    ui->textBrowser->clear();
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        ui->textBrowser->append(lineStr);
    }

    f.close();
}

void MainWindow::on_grammar_clicked()
{
    QFile f("./grammar.txt");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return ;
    }

    QTextStream txtInput(&f);
    QString lineStr;
    ui->textBrowser->clear();
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        ui->textBrowser->append(lineStr);
    }

    f.close();
}
