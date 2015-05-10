#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    root=0;
    //Task New;
    //setTasks(tasks);
    //for (int i=0;i<2;i++) tasks.append(New);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_tableWidget_cellChanged(int row, int column)
{

    //setTasks(tasks);
}
MainWindow::Tree *MainWindow::getMin(Tree *root)
{

    Tree *begin = root;
    if (begin->branchs.size()!=0)
    {
        QVector <Tree *> shtraf;
        for (int i=0;i<begin->branchs.size();i++)
        {
            Tree *tmp=getMin(begin->branchs[i]);
            shtraf.append(tmp);
        }
        int min=shtraf[0]->max;
        int min_number=0;
        for (int i=1;i<shtraf.size();i++)
        {
            if (shtraf[i]->max<min)
            {
                min=shtraf[i]->max;
                min_number=i;
            }
        }
        return shtraf[min_number];
    }
    else return begin;
}
int MainWindow::getMaxShtraf(QVector <int> task)
{
    int time = tasks[task[0]].begin+tasks[task[0]].duration;
    int shtraf = time-tasks[task[0]].deadline;
    for (int i=1;i<task.size();i++)
    {
        if (tasks[task[i]].begin>=time)
        {
            time=tasks[task[i]].begin+tasks[task[i]].duration;
        }
        else
        {
            time+=tasks[task[i]].duration;
        }
        if (time-tasks[task[i]].deadline>shtraf) shtraf=time-tasks[task[i]].deadline;
    }
    return shtraf;
}
bool MainWindow::isContained(int number,QVector <int>task)
{
    for (int i=0;i<task.size();i++)
    {
        if (number==task[i]) return true;
    }
    return false;
}
QVector <int> MainWindow::getOptimalTask()
{
    root = new Tree;
    root->level=0;
    QVector <int> task;
    for (int i=0;i<tasks.size();i++)
    {
        Tree *branch=new Tree;
        root->branchs.append(branch);
        branch->level=1;
        branch->task.append(i);
        branch->max=getMaxShtraf(branch->task);
    }
    while(1)
    {
        Tree *current = getMin(root);
        if (current->level==tasks.size())
        {
            task=current->task;
            break;
        }
        if (current->level<tasks.size())
        {
            for (int i=0;i<tasks.size();i++)
            {
                if (!isContained(i,current->task))
                {
                    Tree *branch=new Tree;
                    current->branchs.append(branch);
                    branch->level=current->level+1;
                    branch->task=current->task;
                    branch->task.append(i);
                    branch->max=getMaxShtraf(branch->task);
                }
            }
        }
    }
    return task;
}
void MainWindow::delTree(Tree *root)
{
    for (int i=0;i<root->branchs.size();i++)
    {
        delTree(root->branchs[i]);
    }
    delete root;
}
/*bool MainWindow::isContained2(QList task,int number)
{
    for (int i=0;i<task.size();i++)
    {
        if (task[i]==number) return true;
    }
    return false;
}*/

QVector <int> MainWindow::approximateTaskList(int level,QVector <int> tmp,int i)
{
    QVector <int>arg;
    for(int k=0;k<tasks.size()-level+1;k++)
    {
        for (int j=0;j<tasks.size();j++)
        {
            if (!isContained(j,tmp))
            {

            }
        }
    }
}

QList <int> MainWindow::getApproximateTask()
{
    QList <int> current;
    //QVector <QList<int>> taskLists;
    for (int i=0;i<tasks.size();i++)
    {
        for (int j=0;j<current.size();j++)
        {

        }

    }
}

void MainWindow::on_CalculateButton_clicked()
{
    ui->textBrowser->clear();
    if (root) delTree(root);
    if (tasks.size()!=0) tasks.clear();
    for (int i=0;i<ui->tasks->columnCount();i++)
    {
        Task New;
        tasks.append(New);
        tasks[i].begin=ui->tasks->item(0,i)->text().toInt();
        tasks[i].duration=ui->tasks->item(1,i)->text().toInt();
        tasks[i].deadline=ui->tasks->item(2,i)->text().toInt();
    }

    QVector <int> shedule=getOptimalTask();
    for (int i=0;i<shedule.size();i++)
    {
        ui->textBrowser->append(QString::number(shedule[i]+1));
    }
}
