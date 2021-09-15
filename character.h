#ifndef CHARACTER_H
#define CHARACTER_H

#include <QObject>
#include <QWidget>


class Character : public QWidget
{
    Q_OBJECT
public:
    int no; //角色序号
    int x,y; //角色行列坐标
    int score = 0; //角色得分
    int numRows; //所在地图的总行数
    int numCols; //所在地图的总列数
    int selectedBoxRow = -1, selectedBoxCol = -1;
    //角色已经激活（选择了）的箱子的坐标，如果角色还没有激活箱子，两个坐标均置为-1

public:
    Character(int m = -1, int n = -1, int no = -1, QWidget *parent = nullptr);
    ~Character(){};

};

#endif // CHARACTER_H
