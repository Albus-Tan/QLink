#ifndef PROPS_H
#define PROPS_H

#include<limits.h>
#include <time.h>

//道具类
class Props
{
public:
    clock_t create;
    int type; //所代表的道具种类
    // 0 代表 +1s
    // 1 代表 Shuffle
    // 2 代表 Hint
    // 3 代表 Flash

    int row; //所在位置行坐标
    int col; //所在位置列坐标
    int maxAppearTimes = INT_MAX; //道具在一场游戏中最多出现次数
    int appearTimeInSeconds = INT_MAX; //道具在一场游戏中出现一次后显现的时间（出现后多少时间消失），单位s
    Props(int type, int row, int col, int appearTimeInSeconds = INT_MAX, int maxAppearTimes = INT_MAX);
};

#endif // PROPS_H
