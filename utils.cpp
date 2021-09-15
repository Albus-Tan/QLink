#include "utils.h"

//
// exchangeNum 函数参数引用传递a与b
// 函数调用后a与b的值相互交换
//
void exchangeNum(int &a, int &b)
{
    int tmp;
    tmp = a;
    a = b;
    b = tmp;
}

//
// generateRandomNumber 函数参数为整型数low和high
// 返回一个介于low与high（包括low和high）之间的整数的随机值
//
int generateRandomNumber(int low, int high)
{
    int rand= QRandomGenerator::global()->bounded(low,high+1);
    return rand;
}
