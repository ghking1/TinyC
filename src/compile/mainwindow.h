#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_open_clicked();
    void on_source_clicked();
    void on_token_clicked();
    void on_tree_clicked();
    void on_mcode_clicked();
    void on_dcode_clicked();
    void on_lexical_clicked();
    void on_grammar_clicked();
    
private:
    Ui::MainWindow *ui;
    QString fileName;
    QProcess myprocess1;
    QProcess myprocess2;
    QProcess myprocess3;
    QProcess myprocess4;
};

#endif // MAINWINDOW_H
