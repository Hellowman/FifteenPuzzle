#include <iostream>
#include <stdio.h>
#include <queue>
#include <set>
#include <sstream>
#include <cstdio>
#include <stdlib.h>
using namespace std;

struct Vertex
{
    int State[4][4];// конфигурация кубиков
    Vertex *Ancestor;// указатель на вершину-родитель
    int Cost, Heuristics;// цена и эвристика(манхэттенское расстояние)
    void RecalculateHeuristics();// вычислить эвристику на основе данных State
    void Print();// вывести на экран значение вершины
    
    
};
//_______________________________________________________________________________________________________

void Vertex::RecalculateHeuristics()
{
    //              0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
    int RowOf[] = { 3, 0, 0, 0, 0, 1, 1, 1,	1, 2, 2, 2, 2, 3, 3, 3 };//координаты кубиков в строке(Xцели)
    int ColOf[] = { 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2 };//координаты кубиков в столбце(Yцели)
    int r = 0;
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            if (State [i][j]!=0)// для нулевой ячейки расстояние не вычисляется
                r+= abs(RowOf[State[i][j]] - i) + abs(ColOf[State[i][j]] - j);//формула для манхэттенского расстояния
    Heuristics = r;//сумма всех расстояний всех вишек до цели(эвристика)
}

void Vertex::Print()//вывод на печать состояния игры на очередном шаге
{
    for(int i=0; i<4; i++)
    {
        copy(&State[i][0], &State[i][3] + 1, ostream_iterator<int>(cout, " "));//вывести очередную строку
        cout << endl;
    }
    cout << endl;
}
//_______________________________________________________________________________________________________
/*Вспомогательная функция для сортировки элементов очереди*/
struct PQSorter
{
    bool operator()( Vertex *lhs, Vertex *rhs)
    {
        return lhs->Cost + lhs->Heuristics > rhs->Cost + rhs->Heuristics;
    }
};

// функция для сравнения содержимого двух вершин, адресуемых указателями
struct CompareVPtrs : public binary_function<Vertex*, Vertex*, bool>
{
    bool operator()(Vertex *lhs, Vertex *rhs) const
    {
        // сравнить содержимое массивов lhs->State и rhs-State
        return equal((int *)lhs->State, (int *)lhs->State + 16, (int *)rhs->State);
    }
}
CompareVP;

priority_queue <Vertex*, deque<Vertex*>, PQSorter> Queue;
set <Vertex*> Vertices;
//______________________________________________________________________________________________________


bool Initialize()// считывает с клавиатуры начальную конфигурацию головоломки и создаёт стартовую вершину, с которой начинается поиск решения
{
    Vertex* StartingState = new Vertex;
    string line;
    for(int i = 0; i < 4; i++)// считать с клавиатуры четыре строки
    {
        getline(cin, line);
        istringstream is(line);// превращаем строку в поток
        for(int j = 0; j < 4; j++)// каждая строка содержит четыре числа
            is >> StartingState->State[i][j];// читаем данные из потока
    }
    
    StartingState->Cost = 0;// цена стартовой вершины равна нулю
    StartingState->RecalculateHeuristics();// вычисляет манхэттенское расстояние для каждой вершины
    StartingState->Ancestor = NULL;// вершины-родителя не существует
    Queue.push(StartingState);// добавить вершину в очередь с приоритетами
    Vertices.insert(StartingState);//и в список известных вершин
    
    //проверка решаема ли изначальная конфигурация
    int nol = 0;
    int sum = 0;
    /*for (int i = 0; i<16;i++)
     {
     if (!StartingState -> State[i]) nol=i/4+1;
     else
     for (int j=i;j<16;j++)
     {
     if (StartingState -> State[i] > StartingState -> State[j] && StartingState -> State[j])
     sum++;
     }
     }*/
    
    for (int i = 0; i<4; i++)
    {
        for (int j = 0; j<4; j++)
        {
            if (!StartingState -> State[i][j]) nol=i+1;
            else
                for (int k = i; k<4; k++)
                {
                    for (int l = j; l<4; l++)
                    {
                        if (StartingState->State[i][j] > StartingState->State[k][l] && StartingState->State[k][l]!=0)
                            sum++;
                    }
                }
        }
    }
    
    if ((sum+nol)%2)
    {
        cout << "No solutions\n";
        return false;
    }
    
    else
    {
        cout << "Have solutions\n";
        return true;
    }
    
}

//______________________________________________________________________________________________________

/*Получение вершин-потомков для любой заданной вершины*/

void AddNeighbours(Vertex* GameState)//добавить в список вершины, соседние с GameState
{
    int zi=0, zj=0;
    // найти нулевой кубик и получить его координаты zi, zj
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            if(GameState->State[i][j] == 0)
            {
                zi = i; zj = j;// новые координаты нулевого кубика
                break;
            }
    
    int di[] = {-1, 0, 1, 0};// варианты куда сдвинуть нулевую ячейку (влево,вправо)
    int dj[] = {0, -1, 0, 1};// варианты куда сдвинуть нулевую ячейку (вверх, вниз)
    
    for(int k = 0; k < 4; k++) // нулевой кубик можно сдвинуть в любую из четырёх сторон
    {
        int i =	zi + di[k] ;// сдвигаем кубик (влево, вправо)
        int j =	zj + dj[k] ;// сдвигаем кубик (вверх, вниз)
        if(i >= 0 && j >= 0 && i <= 3 && j <= 3)// если нулевой кубик не выходит за пределы коробки
        {
            Vertex* v = new Vertex;// создать новую вершину
            copy((int*)GameState->State, (int*)GameState + 16, (int *)v->State);// скопировать в неё содержимое текущей
            v->State[i][j] = 0;// новая позиция нулевого кубика
            v->State[zi][zj] = GameState->State[i][j];// а на его месте - кубик (i, j) состояния GameState
            v->Cost = GameState->Cost + 1;// цена = цена_вершины-родителя + 1
            v->RecalculateHeuristics();// вычислить эвристику
            v->Ancestor = GameState;
            // если вершина с эквивалентным содержимым не рассмотрена ранее (для сравнения вершин используется функция CompareVP)
            if(find_if(Vertices.begin(), Vertices.end(), bind2nd(CompareVP, v)) == Vertices.end())
            {
                Queue.push(v);	// добавить вершину в очередь
                Vertices.insert(v);	//и	в список известных вершин
            }
            else
                delete v;// удалить уже рассмотренную вершину
        }
    }
}
//______________________________________________________________________________________________________
//Определяем, находится ли уже вершина, эквивалентная текущей, во множестве Vertices
bool IsGoal(Vertex* s)// является ли данная вершина финишной?
{
    int Goal[4][4] = { {1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 0} };//массив правильной конфигурации пятнашек
    return equal((int *)s->State, (int *)s->State + 16, (int *)Goal);//сравнивает с первой до последней позиции с массивом Goal
}

//_____________________________________________________________________________________________________

int main( int argc, char* argv[])
{
    cout << "Enter configuration of puzzle 4x4\n";
    if (Initialize())// считать начальную конфигурацию
    {
        int c=0; // счётчик итераций, сумма всех вариантов куда сдвинуть нулевую ячейку
       
        while(!Queue.empty())
        {
            Vertex* v = Queue.top();// извлечь головной элемент очереди
            Queue.pop();
            c++;
            
            if(IsGoal(v))// если найдено решение
            {
                // вывести все состояния на пути от финальной конфигурации до стартовой
                while(v!= NULL)
                {
                    v->Print();
                    v=v->Ancestor;
                }
                cout << c << "=iteration done\n";//вывод итераций
                break;
            }
            else
                AddNeighbours(v);// решение не найдено, добавить в очередь вершины-потомки
        }
        
        // освобождаем память
        for(set<Vertex*>::iterator p=Vertices.begin(); p!=Vertices.end(); p++)
            delete *p;
    }
    return 0;
}
