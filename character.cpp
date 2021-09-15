#include "character.h"

//
// Character 接受所在地图行数(m)，所在地图列数(n)，角色序号(no)作为参数，构造相应的角色并随机设定其初始位置
//
Character::Character(int m, int n, int no, QWidget *parent) : QWidget(parent)
{
    numRows = m;
    numCols = n;
    this->no = no;
}

