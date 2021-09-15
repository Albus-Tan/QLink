#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QWidget>
#include <QTime>
#include <QTimer>
#include <QIcon>
#include <QStyle>
#include "utils.h"
#include "character.h"
#include "props.h"
#include <QPainter>
#include <vector>

//Debug
#include <iostream>

struct Dot{
public:
    int row;
    int col;
    Dot(int x = -1, int y = -1){ row = x, col = y;}
};

class Map : public QWidget
{
    Q_OBJECT
public:
    int numOfPlayer;

    const int kTotalBoxNum = 300; //总共需要随机生成的箱子数目
    const int kBoxTypes = 5; //总共的箱子种类，如果改动需要更改其他地方
    const int kBoxScore[6] = {0, 5, 4, 3, 2, 1}; //kBoxScore[i]为一对boxtype为i的箱子被消除后对应的分数

    int numRows; //地图行数
    int numCols; //地图列数
    bool** canVisitChecked = nullptr; //记录当前地图各位置角色是否可以到达有没有被检查过
    int** gameMap=nullptr;
    //利用int存储标志地图
    //0表示该位置为空
    //正数表示该位置所放置方块的type序号
    //负1至5表示该位置所存在角色序号的相反数
    //负6及之后表示该位置存在道具
    // -6 表示道具0 +1s
    // -7 表示道具1 Shuffle
    // -8 表示道具2 Hint
    // -9 表示道具3 Flash

    //行数范围0至numRows-1，列数范围0至numCols-1

    //与道具有关的常量，数组中i位置对应道具i
    const int kTotalPropTypes = 4;
    const double appearFrequency[4] = {50.0, 50.0, 50.0, 50.0}; //平均1000s游戏中出现的次数
    const int maxAppearTimes[4] = {100, 10, 100, 100}; //一局游戏中该道具最多出现几次
    int alreadyAppearTimes[4] = {0, 0, 0, 0}; //各道具到目前为止已经出现的次数
    const int appearTimeInSeconds[4] = {10, 10, 10, 10}; //道具出现一次最长的持续时间，单位s

    //用于存储box信息的boxNode类
    class BoxNode
    {
    public:
        int x; //所处行数
        int y; //所处列数
        BoxNode* next = nullptr; //同箱体type序号的下一个boxnode
        BoxNode(){};
        BoxNode(int x, int y, BoxNode* next = nullptr)
        {
            this->x = x;
            this->y = y;
            this->next = next;
        }
    };
    BoxNode** boxes = nullptr; //箱体type序号i一样的存在box[i]单链表中

    //记录连接两箱体的连线的成员变量
    int numOfDotsInMatchingLine = 0; //连接两个可消除的箱体的连线的关键点个数，包括端点和转折点（直线即为2，折一次即为3，折两次即为4）
    Dot dotsInMatchingLine[4]; //连接两个可消除的箱体的连线的关键点坐标

    //存储道具的vec
    std::vector<Props> propsVec;
    std::vector<Props>::iterator it;

    //Hint道具
    clock_t hintStart;
    bool hintOn = false;
    Dot hintDots[2];

    //Flash道具
    clock_t flashStart;
    bool flashOn = false;

public:
    Map(int numOfPlayer, int m = 20 , int n = 30);
    ~Map();
    bool canBeLinkedByStraightLine(int x1, int y1, int x2, int y2) const;
    bool canBeLinked(int x1, int y1, int x2, int y2, bool storeDotsInMatchingLine = false);
    bool isSameTypeAndCanBeLinked(int x1, int y1, int x2, int y2, bool storeDotsInMatchingLine = false);
    bool currentMapStateSolvable(Character* player1, Character* player2);
    bool playersSurrounded(Character* player1, Character* player2);
    bool findOneSolution(int &x1, int &y1, int &x2, int &y2, Character* player1, Character* player2);
    void initializeTheMap(Character* player1, Character* player2);
    void randomlyCreateBoxes(int boxNum);
    int clearAllBoxesAndReturnBoxNum(bool clearAllMapItems = false);
    int returnBoxNum();
    void erasePairedBoxes(int x1, int y1, int x2, int y2);
    bool existPlayerCanReachBoxes(int x1, int y1, int x2, int y2, Character* player1, Character* player2); //检测场上的玩家中是否至少存在一名可以到达行列坐标为(x1,y1)(x2,y2)的方块对
    //与道具有关的函数
    void randomlyCreateProps();
    void propsShuffle(Character* player1, Character* player2);
    void propsHint(Character* player1, Character* player2, bool resetStartTime = true);
    bool emptyPlacesLessThan(int n);
    bool checkMapCanVisit(int rowStart, int colStart, int rowEnd, int colEnd, bool changeEndBoxToEmpty = false);
    void checkCurrentAndSurroundCanVisit(int row, int col, bool &canVisit, int rowEnd, int colEnd);

    //Debug
    void printGameMap() const;
    void calculateAndPrintInfo() const;

};

#endif // MAP_H
