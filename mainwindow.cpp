#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    root=0;
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));
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
    //delTree(root);
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
bool MainWindow::isContained2(QList <int>task,int number)
{
    for (int i=0;i<task.size();i++)
    {
        if (task[i]==number) return true;
    }
    return false;
}

QVector <int> MainWindow::approximateTaskList(QList <int> taskList,int number)
{
    QVector <int> tasker;
    tasker.append(number);
    for (int i=taskList.size()-1;i>=0;i--)
    {
        if (!isContained(taskList[i],tasker) && tasker.size()<=taskList.size()) tasker.prepend(taskList[i]);
    }
    return tasker;
}

QList <int> MainWindow::getApproximateTask()
{
    QList <int> taskList;
    QList <int> result;
    for (int i=0;i<tasks.size();i++)
    {
        taskList.append(i);
    }
    while (taskList.size()!=0)
    {
        QVector <int> tasks=approximateTaskList(taskList,taskList[0]);
        int min=getMaxShtraf(approximateTaskList(taskList,taskList[0]));
        ui->textBrowser_2ndAlgWork->append("F("+getTaskList(tasks)+")="+QString::number(min));
        int min_number=0;
        for (int i=1;i<taskList.size();i++)
        {
            tasks=approximateTaskList(taskList,taskList[i]);
            int shtraf=getMaxShtraf(tasks);
            ui->textBrowser_2ndAlgWork->append("F("+getTaskList(tasks)+")="+QString::number(shtraf));
            if (shtraf<min)
            {
                min=shtraf;
                min_number=i;
            }
        }
        result.prepend(taskList[min_number]);
        taskList.removeAt(min_number);
    }
    return result;
}
QString MainWindow::getTaskList(QVector<int> list)
{
    QString tmp="";
    for (int i=0;i<list.size()-1;i++)
    {
        tmp+=QString::number(list[i]+1)+",";
    }
    tmp+=QString::number(list[list.size()-1]+1);
    return tmp;
}

QString MainWindow::getTaskList2(QList<int> list)
{
    QString tmp="";
    for (int i=0;i<list.size()-1;i++)
    {
        tmp+=QString::number(list[i]+1)+",";
    }
    tmp+=QString::number(list[list.size()-1]+1);
    return tmp;
}

void MainWindow::showTree(QTreeWidgetItem *rootItem,Tree *root)
{
    QTreeWidgetItem *newRootItem;
    if (root->level==0)
    {
        newRootItem=new QTreeWidgetItem(ui->tree);
        newRootItem->setText(0,"It's a root");

    }
    else
    {
        newRootItem=new QTreeWidgetItem(rootItem);
        newRootItem->setText(1,QString::number(root->max));
        newRootItem->setText(0,getTaskList(root->task));
    }
    for (int i=0;i<root->branchs.size();i++)
    {
        showTree(newRootItem,root->branchs[i]);
    }
    ui->tree->expandAll();
}
int MainWindow::getMaxShtraf2(QList <int> task)
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
void MainWindow::on_CalculateButton_clicked()
{
    ui->tree->clear();
    ui->textBrowser->clear();
    ui->textBrowser_2ndAlg->clear();
    ui->textBrowser_2ndAlgWork->clear();
    if (root) delTree(root);
    if (tasks.size()!=0) tasks.clear();
    int taskCount=ui->taskCount->text().toInt();
    ui->tasks->setColumnCount(taskCount);
    int k=ui->lineEditDiapazon->text().toInt();
    int n;
    if (ui->radioButton_gen->isChecked())
    {
        for (int i=0;i<taskCount;i++)
        {
            n=qrand()%(k+1);
            QTableWidgetItem *item=new QTableWidgetItem(QString::number(n));
            ui->tasks->setItem(0,i,item);
            n=qrand()%(k+1);
            item=new QTableWidgetItem(QString::number(n));
            ui->tasks->setItem(1,i,item);
            n=qrand()%(k+1);
            item=new QTableWidgetItem(QString::number(n));
            ui->tasks->setItem(2,i,item);
        }
    }

    for (int i=0;i<ui->tasks->columnCount();i++)
    {
        Task New;
        tasks.append(New);
        tasks[i].begin=ui->tasks->item(0,i)->text().toInt();
        tasks[i].duration=ui->tasks->item(1,i)->text().toInt();
        tasks[i].deadline=ui->tasks->item(2,i)->text().toInt();
    }
    QTime time;
    time.start();
    QVector <int> shedule=getOptimalTask();
    int time11=time.elapsed();

    ui->tree->expandAll();
    ui->textBrowser->append(getTaskList(shedule));
    QTime time2;
    time2.start();
    QList<int> taskList2=getApproximateTask();
    int time22=time2.elapsed();
    ui->textBrowser_2ndAlg->append(getTaskList2(taskList2));
    ui->label_time->setText("Время МВГ="+QString::number(time11)+"\nВремя прибл. алг.="
                           +QString::number(time22)+"\nШтраф первого алгоритма="+QString::number(getMaxShtraf(shedule))
                           +"\nШтраф второго алгоритма="+QString::number(getMaxShtraf2(taskList2)));
    //ui->textBrowser_2ndAlg->append(QString::number(getMaxShtraf2((taskList2))));
}

void MainWindow::on_radioButton_2_our_clicked()
{
    int taskCount=ui->taskCount->text().toInt();
    ui->tasks->setColumnCount(taskCount);
}

void MainWindow::on_pushButton_tree_clicked()
{
    Tree *r=root;
    showTree(0,r);
}
