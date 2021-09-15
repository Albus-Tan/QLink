#include "simpletest.h"

SimpleTest::SimpleTest(QObject *parent) : QObject(parent)
{

}

void SimpleTest::case1_testcase(){
    Map map(1, 6, 6);
    for(int i = 0; i < map.numRows; ++i){
        for(int j = 0; j < map.numCols; ++j){
            map.gameMap[i][j] = i;
        }
    }
    //构造矩阵如下
    // [ [ 0, 0, 0, 0, 0, 0 ],
    //   [ 1, 1, 1, 1, 1, 1 ],
    //   [ 2, 2, 2, 2, 2, 2 ],
    //   [ 3, 3, 3, 3, 3, 3 ],
    //   [ 4, 4, 4, 4, 4, 4 ],
    //   [ 5, 5, 5, 5, 5, 5 ] ]

    QVERIFY(map.canBeLinkedByStraightLine(1,1,1,4) == false);
    QVERIFY(map.canBeLinkedByStraightLine(2,3,2,4) == true);
    QVERIFY(map.canBeLinkedByStraightLine(3,2,3,3) == true);
    QVERIFY(map.canBeLinked(1,1,1,4) == true);
    QVERIFY(map.canBeLinked(2,1,2,4) == false);
    QVERIFY(map.canBeLinked(3,2,3,3) == true);
}

void SimpleTest::case2_testcase(){
    Map map(1, 6, 6);
    for(int i = 0; i < map.numRows; ++i){
        for(int j = 0; j < map.numCols; ++j){
            map.gameMap[i][j] = (i + j) % 2;
        }
    }
    map.gameMap[2][2] = 2;
    map.gameMap[1][2] = 0;
    //构造矩阵如下
    // [ [ 0, 1, 0, 1, 0, 1 ],
    //   [ 1, 0, 0, 0, 1, 0 ],
    //   [ 0, 1, 2, 1, 0, 1 ],
    //   [ 1, 0, 1, 0, 1, 0 ],
    //   [ 0, 1, 0, 1, 0, 1 ],
    //   [ 1, 0, 1, 0, 1, 0 ] ]

    QVERIFY(map.canBeLinkedByStraightLine(1,0,1,4) == true);
    QVERIFY(map.canBeLinkedByStraightLine(0,1,2,1) == true);
    QVERIFY(map.canBeLinkedByStraightLine(0,3,4,3) == false);
    QVERIFY(map.canBeLinkedByStraightLine(1,1,2,1) == true);
    QVERIFY(map.canBeLinkedByStraightLine(1,1,1,3) == true);
    QVERIFY(map.canBeLinkedByStraightLine(1,3,2,3) == true);
    QVERIFY(map.canBeLinked(2,1,1,4) == true);
    QVERIFY(map.canBeLinked(2,1,2,3) == true);
    QVERIFY(map.canBeLinked(2,1,0,3) == true);
    QVERIFY(map.canBeLinked(2,1,4,3) == false);
}

void SimpleTest::case3_testcase(){
    Map map(1, 6, 6);
    for(int i = 0; i < map.numRows; ++i){
        for(int j = 0; j < map.numCols; ++j){
            map.gameMap[i][j] = 0;
        }
    }
    map.gameMap[0][0] = 2;
    map.gameMap[1][3] = 2;
    map.gameMap[2][0] = 1;
    map.gameMap[2][2] = 2;
    map.gameMap[2][3] = -3;
    map.gameMap[3][3] = 2;
    map.gameMap[3][4] = 1;
    map.gameMap[4][1] = 2;
    map.gameMap[4][3] = 1;
    map.gameMap[4][4] = -1;
    map.gameMap[4][5] = 1;
    map.gameMap[5][0] = 2;

    //构造矩阵如下
    // [ [ 2, 0, 0, 0, 0, 0 ],
    //   [ 0, 0, 0, 2, 0, 0 ],
    //   [ 1, 0, 2, -3, 0, 0 ],
    //   [ 0, 0, 0, 2, 1, 0 ],
    //   [ 0, 2, 0, 1, -1, 1 ],
    //   [ 2, 0, 0, 0, 0, 0 ] ]

    QVERIFY(map.canBeLinkedByStraightLine(2,0,4,3) == false); //最少折三次
    QVERIFY(map.canBeLinked(2,0,4,3) == false); //最少折三次
    QVERIFY(map.isSameTypeAndCanBeLinked(2,0,4,3) == false); //最少折三次
    QVERIFY(map.canBeLinkedByStraightLine(5,0,1,3) == false); //最少折三次
    QVERIFY(map.canBeLinked(5,0,1,3) == false); //最少折三次
    QVERIFY(map.isSameTypeAndCanBeLinked(5,0,1,3) == false); //最少折三次
    QVERIFY(map.canBeLinkedByStraightLine(4,3,4,5) == true); //中间相隔角色
    QVERIFY(map.canBeLinked(4,3,4,5) == true); //中间相隔角色
    QVERIFY(map.isSameTypeAndCanBeLinked(4,3,4,5) == true); //中间相隔角色
    QVERIFY(map.canBeLinkedByStraightLine(3,3,1,3) == true); //中间相隔道具
    QVERIFY(map.canBeLinked(3,3,1,3) == true); //中间相隔道具
    QVERIFY(map.isSameTypeAndCanBeLinked(3,3,1,3) == true); //中间相隔道具

    QVERIFY(map.canBeLinkedByStraightLine(2,0,2,2) == true); //不同的箱子折一次
    QVERIFY(map.canBeLinked(2,0,2,2) == true); //不同的箱子折一次
    QVERIFY(map.isSameTypeAndCanBeLinked(2,0,2,2) == false); //不同的箱子折一次
    QVERIFY(map.canBeLinkedByStraightLine(4,1,3,4) == false); //不同的箱子折两次
    QVERIFY(map.canBeLinked(4,1,3,4) == true); //不同的箱子折两次
    QVERIFY(map.isSameTypeAndCanBeLinked(4,1,3,4) == false); //不同的箱子折两次

    map.gameMap[0][1] = -3;
    QVERIFY(map.canBeLinkedByStraightLine(0,1,2,3) == false); //相同的道具折两次
    QVERIFY(map.canBeLinked(0,1,2,3) == true); //相同的道具折两次
    QVERIFY(map.isSameTypeAndCanBeLinked(0,1,2,3) == false); //相同的道具折两次

    map.gameMap[0][1] = 0;
    map.gameMap[2][3] = 1;
    QVERIFY(map.canBeLinkedByStraightLine(1,3,3,3) == false); //折两次
    QVERIFY(map.canBeLinked(1,3,3,3) == true); //折两次
    QVERIFY(map.isSameTypeAndCanBeLinked(1,3,3,3) == true); //折两次
}

void SimpleTest::case4_testcase(){
    Map map(1, 6, 6);
    for(int i = 0; i < map.numRows; ++i){
        for(int j = 0; j < map.numCols; ++j){
            map.gameMap[i][j] = 0;
        }
    }
    map.gameMap[0][0] = 2;
    map.gameMap[1][3] = 2;
    map.gameMap[2][0] = 1;
    map.gameMap[2][2] = 2;
    map.gameMap[2][3] = -3;
    map.gameMap[3][3] = 2;
    map.gameMap[3][4] = 1;
    map.gameMap[4][1] = 2;
    map.gameMap[4][3] = 1;
    map.gameMap[4][4] = -1;
    map.gameMap[4][5] = 1;
    map.gameMap[5][0] = 2;

    //构造矩阵如下
    // [ [ 2, 0, 0, 0, 0, 0 ],
    //   [ 0, 0, 0, 2, 0, 0 ],
    //   [ 1, 0, 2, -3, 0, 0 ],
    //   [ 0, 0, 0, 2, 1, 0 ],
    //   [ 0, 2, 0, 1, -1, 1 ],
    //   [ 2, 0, 0, 0, 0, 0 ] ]

    QVERIFY(map.canBeLinkedByStraightLine(4,3,4,3) == false); //同一个箱子自己连自己
    QVERIFY(map.canBeLinked(4,3,4,3) == false); //同一个箱子自己连自己
    QVERIFY(map.isSameTypeAndCanBeLinked(4,3,4,3) == false); //同一个箱子自己连自己
    QVERIFY(map.canBeLinkedByStraightLine(-1,3,4,3) == false); //非法的坐标
    QVERIFY(map.canBeLinked(-1,3,4,3) == false); //非法的坐标
    QVERIFY(map.isSameTypeAndCanBeLinked(-1,3,4,3) == false); //非法的坐标
    QVERIFY(map.canBeLinkedByStraightLine(2,3,6,3) == false); //非法的坐标
    QVERIFY(map.canBeLinked(2,3,6,3) == false); //非法的坐标
    QVERIFY(map.isSameTypeAndCanBeLinked(2,3,6,3) == false); //非法的坐标

}


// QTEST_MAIN(SimpleTest)
