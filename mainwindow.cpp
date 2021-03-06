#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    root=0;//обнуляем адрес корня дерева
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));//настройка для генератора случ. величин
    QStringList taskHeaders;
    taskHeaders<<"r"<<"q"<<"d";
    ui->tasks->setVerticalHeaderLabels(taskHeaders);//установка вертикального имени таблицы заданий
    QStringList treeHeaders;
    treeHeaders<<"Ветви"<<"Штраф";
    ui->tree->setHeaderLabels(treeHeaders);//установка горизонтального имени для дерева вычислений МВГ
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
MainWindow::Tree *MainWindow::getMin(Tree *root)//вернет указатель на узел дерева с минимальным штрафом
{

    Tree *begin = root;
    if (begin->branchs.size()!=0)
    {
        QVector <Tree *> shtraf;
        for (int i=0;i<begin->branchs.size();i++)
        {
            Tree *tmp=getMin(begin->branchs[i]);//рекурсивно вызывает самого себя для нахождения минимальных узлов(у которых штраф минимальный)
            shtraf.append(tmp);
        }
        int min=shtraf[0]->max;
        int min_number=0;
        for (int i=1;i<shtraf.size();i++)
        {
            if (shtraf[i]->max<min)
            {
                min=shtraf[i]->max;//идет сравнение всех штрафов с ветвей
                min_number=i;
            }
        }
        return shtraf[min_number];//вернем узел с минимальным штрафом
    }
    else return begin;
}
int MainWindow::getMaxShtraf(QVector <int> task)//функция вычисления максимального штрафа для определенного расписания
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
bool MainWindow::isContained(int number,QVector <int>task)//функция вернет true, если число содержится в контейнере номеров заданий
{
    for (int i=0;i<task.size();i++)
    {
        if (number==task[i]) return true;
    }
    return false;
}
QVector <int> MainWindow::getOptimalTask()//функция. которая вернет оптимальное расписание
{
    root = new Tree;
    root->level=0;//создаем дерево
    QVector <int> task;
    for (int i=0;i<tasks.size();i++)
    {
        Tree *branch=new Tree;
        root->branchs.append(branch);
        branch->level=1;
        branch->task.append(i);
        branch->max=getMaxShtraf(branch->task);
    }//создаем узлы уровнем 1
    while(1)
    {
        Tree *current = getMin(root);//находим минимальный узел
        if (current->level==tasks.size())//если уровень минимального узла равен количеству заданий
            //то вернем расписание этого узла
        {
            task=current->task;
            break;
        }
        if (current->level<tasks.size())//если уровень меньше, то
        {
            for (int i=0;i<tasks.size();i++)//для минимального узла создаем ветви
            {
                if (!isContained(i,current->task))
                {
                    Tree *branch=new Tree;
                    current->branchs.append(branch);
                    branch->level=current->level+1;
                    branch->task=current->task;
                    branch->task.append(i);
                    branch->max=getMaxShtraf(branch->task);//и заполняем их
                }
            }
        }
    }
    //delTree(root);
    return task;//и в итоге вернем оптимальное расписание

}
void MainWindow::delTree(Tree *root)//функция удаления дерева
{
    for (int i=0;i<root->branchs.size();i++)
    {
        delTree(root->branchs[i]);
    }
    delete root;
}


QVector <int> MainWindow::approximateTaskList(QList <int> taskList,int number)//по заданным начальному расписанию строит полное расписание
//для приближенного алгоритма
{
    QVector <int> tasker;
    tasker.append(number);
    for (int i=taskList.size()-1;i>=0;i--)
    {
        if (!isContained(taskList[i],tasker) && tasker.size()<=taskList.size()) tasker.prepend(taskList[i]);
    }
    return tasker;
}

QList <int> MainWindow::getApproximateTask()//функция, которая вернет приближенное расписание
{
    QList <int> taskList;
    QList <int> result;
    for (int i=0;i<tasks.size();i++)
    {
        taskList.append(i);//забиваем номера расписаний в отдельный список
    }
    while (taskList.size()!=0)
    {
        QVector <int> tasks=approximateTaskList(taskList,taskList[0]);//создает промежуточное расписание
        int min=getMaxShtraf(approximateTaskList(taskList,taskList[0]));//получаем штраф расписания
        ui->textBrowser_2ndAlgWork->append("F("+getTaskList(tasks)+")="+QString::number(min));//печатаем информацию
        int min_number=0;
        for (int i=1;i<taskList.size();i++)//дальше строим другие промежуточные расписания
            //сравниваем их
            //отсеиваем нужные из них
            //и на основе отсекаемого создаем новые промежуточные расписания
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
        result.prepend(taskList[min_number]);//постепенно добавляем отсекаемые части в финальный результат
        taskList.removeAt(min_number);//при этом удаляем номер задания из списка
    }
    return result;//в конце вернем наш приближенное расписание
}
QString MainWindow::getTaskList(QVector<int> list)//функция вывода расписания для контейнера типа QVector
{
    QString tmp="";
    for (int i=0;i<list.size()-1;i++)
    {
        tmp+=QString::number(list[i]+1)+",";
    }
    tmp+=QString::number(list[list.size()-1]+1);
    return tmp;
}

QString MainWindow::getTaskList2(QList<int> list)//функция вывода расписания для контейнера типа QList

{
    QString tmp="";
    for (int i=0;i<list.size()-1;i++)
    {
        tmp+=QString::number(list[i]+1)+",";
    }
    tmp+=QString::number(list[list.size()-1]+1);
    return tmp;
}

void MainWindow::showTree(QTreeWidgetItem *rootItem,Tree *root)//функция вывода дерево решений
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
    ui->tree->expandAll();//пробегаем по всем вершинам дерева решений и на основе этого дерева строим дерево для QTreeWidget
}
int MainWindow::getMaxShtraf2(QList <int> task)//то же самое, что и getMaxShtraf, только для контейнера QList
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
void MainWindow::on_CalculateButton_clicked()//функция, выполняющаяся при нажатии кнопки "выполнить"
{
    ui->tree->clear();
    ui->textBrowser->clear();
    ui->textBrowser_2ndAlg->clear();
    ui->textBrowser_2ndAlgWork->clear();//очищаем все поля вывода
    if (root) delTree(root);//если дерево существует, то его удаляем
    if (tasks.size()!=0) tasks.clear();//очищаем список заказов
    int taskCount=ui->taskCount->text().toInt();//принимаем количество заданий
    ui->tasks->setColumnCount(taskCount);//устанавливаем количество столбцов для таблицы заданий
    int k=ui->lineEditDiapazon->text().toInt();//принимаем значение диапазона
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
    }//если нажата Случайные значения, то в таблицу заносим случаные значения

    for (int i=0;i<ui->tasks->columnCount();i++)
    {
        Task New;
        tasks.append(New);
        tasks[i].begin=ui->tasks->item(0,i)->text().toInt();
        tasks[i].duration=ui->tasks->item(1,i)->text().toInt();
        tasks[i].deadline=ui->tasks->item(2,i)->text().toInt();
    }//с таблицы считываем информацию о заданиях
    QTime time;
    time.start();
    QVector <int> shedule=getOptimalTask();//выполняем алгоритм МВГ, при этом мерим время
    int time11=time.elapsed();

    ui->tree->expandAll();
    ui->textBrowser->append(getTaskList(shedule));//вставим оптимальное расписание
    QTime time2;
    time2.start();
    QList<int> taskList2=getApproximateTask();//выполняем приближенный алгоритм с засеканием времени
    int time22=time2.elapsed();
    ui->textBrowser_2ndAlg->append(getTaskList2(taskList2));//вставим приближенное расписание
    ui->label_time->setText("Время МВГ="+QString::number(time11)+"\nВремя прибл. алг.="
                           +QString::number(time22)+"\nШтраф первого алгоритма="+QString::number(getMaxShtraf(shedule))
                           +"\nШтраф второго алгоритма="+QString::number(getMaxShtraf2(taskList2)));
    //вставляем информацию о штрафах алгоритмов и о их времени выполнения
}
void MainWindow::on_radioButton_2_our_clicked()//функция, устанавливающая значение количества
//столбцов для таблицы с поля количества заданий
{
    int taskCount=ui->taskCount->text().toInt();
    ui->tasks->setColumnCount(taskCount);
}

void MainWindow::on_pushButton_tree_clicked()//функция, выполняющаяся при нажатии кнопки вывод дерева
{
    Tree *r=root;
    showTree(0,r);
}
