#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QTime>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    struct Tree
    {
        int max;
        int level;
        bool isRoot;
        QVector <int>task;
        QVector <Tree*> branchs;
    };
    struct Tree2
    {
        int max;
        int level;
        bool isRoot;
        QVector <int>task;
        QList <int> mask;
        QVector <Tree*> branchs;
    };
private slots:
    void on_tableWidget_cellChanged(int row, int column);

    void on_CalculateButton_clicked();

    void on_radioButton_2_our_clicked();

private:
    Ui::MainWindow *ui;
    struct Task
    {
        int begin;
        int duration;
        int deadline;
    };
    QVector <Task> tasks;

    Tree *root;
    Tree* getMin(Tree *root);
    int getMaxShtraf(QVector <int> task);
    bool isContained(int number,QVector <int>task);
    QVector <int>getOptimalTask();
    void delTree(Tree *root);
    QList <int>getApproximateTask();
    bool isContained2(QList <int> taskList, int number);
    QVector <int>approximateTaskList(QList <int> taskList,int number);
    void showTree(QTreeWidgetItem *rootItem,Tree *root);
    QString getTaskList(QVector <int> list);
    int getMaxShtraf2(QList <int> task);
    QString getTaskList2(QList <int> task);
    //bool isContained2(QList task,int number);
};

#endif // MAINWINDOW_H
