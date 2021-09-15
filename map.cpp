#include "map.h"

//
// Map 生成m行n列的地图
// 完成 boxes 链表的建立，canVisitChecked 矩阵的建立和初始化
//
Map::Map(int numOfPlayer, int m, int n)
{
    this->numOfPlayer = numOfPlayer;
    numRows = m;
    numCols = n;
    gameMap = new int* [numRows];
    for(int i = 0; i < numRows; ++i){
        gameMap[i] = new int [numCols];
    }
    for(int i = 0; i < numRows; ++i){
        for(int j = 0; j < numCols; ++j){
            gameMap[i][j] = 0;
        }
    }
    canVisitChecked = new bool* [numRows];
    for(int i = 0; i < numRows; ++i){
        canVisitChecked[i] = new bool [numCols];
    }
    for(int i = 0; i < numRows; ++i){
        for(int j = 0; j < numCols; ++j){
            canVisitChecked[i][j] = false;
        }
    }
    boxes = new BoxNode* [kBoxTypes + 1]; //boxes[0]闲置，boxes[i]以单链表形式存储序号为i的boxes的行列信息
    for(int i = 0; i < kBoxTypes + 1; ++i){
        boxes[i] = nullptr;
    }
}

//
// ~Map 析构函数
// 删除 boxes 链表，删除canVisitChecked 矩阵，删除gameMap矩阵
//
Map::~Map()
{
    BoxNode* tmp = nullptr;
    for(int i = 0; i < kBoxTypes + 1; ++i){
        while(boxes[i] != nullptr){
            tmp = boxes[i]->next;
            delete boxes[i];
            boxes[i] = tmp;
        }
    }
    delete [] boxes;
    for(int i = 0; i < numRows; ++i){
        delete canVisitChecked[i];
    }
    delete canVisitChecked;
    for(int i = 0; i < numRows; ++i){
        delete gameMap[i];
    }
    delete gameMap;
}

//
// canBeLinkedByStraightLine 接受两个行列坐标(x1,y1)(x2,y2)作为参数
// 返回true表示两个行列坐标间可以通过不折的直线进行连接，返回false则不能
// 此函数不对两行列坐标上物体类型是否相同进行判断
// 如果所给的两个坐标完全一样，返回false
//
bool Map::canBeLinkedByStraightLine(int x1, int y1, int x2, int y2) const
{
    if(x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 || x1 >= numRows || x2 >= numRows || y1 >= numCols || y2 >=numCols) return false;
    if(x1 == x2 && y1 == y2) return false; //如果两个行列坐标一样，不是不同位置，返回false
    //判断是否能通过直线连接
    //此时只能x或y坐标一致，且中间连线上不存在box
    bool canBeLinked = true;
    bool exchanged = false;
    if(x1 == x2){
        //将y1比y2大换做y1比y2小的情况
        if(y1 > y2){
            exchangeNum(y1,y2);
            exchanged = true;
        }
        for(int i = y1 + 1; i < y2; ++i){
            if(gameMap[x1][i] > 0) canBeLinked = false;
        }
        if(canBeLinked) return true;
        if(exchanged){
            exchangeNum(y1,y2);
        }
    }
    canBeLinked = true;
    exchanged = false;
    if(y1 == y2){
        if(x1 > x2){
            exchangeNum(x1,x2);
            exchanged = true;
        }
        for(int i = x1 + 1; i < x2; ++i){
            if(gameMap[i][y1] > 0) canBeLinked = false;
        }
        if(canBeLinked) return true;
        if(exchanged){
            exchangeNum(x1,x2);
        }
    }
    return false;
}


//
// canBeLinked 接受两个行列坐标(x1,y1)(x2,y2)以及是否需要存储连接线上的关键点的bool值作为参数（该参数默认为false）
// 返回true表示两个行列坐标间可以通过两次以内的折线进行连接，返回false则不能
// 此函数不对两行列坐标上物体的类型是否相同进行判断
// 如果可以连接并且参数storeDotsInMatchingLine为true
// 此函数会在map中将连线路径上的关键节点信息更新（包括numOfDotsInMatchingLine和dotsInMatchingLine）
//
bool Map::canBeLinked(int x1, int y1, int x2, int y2, bool storeDotsInMatchingLine)
{
    if(x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 || x1 >= numRows || x2 >= numRows || y1 >= numCols || y2 >=numCols) return false;
    //判断是否能通过直线连接
    bool canBeLinked;
    canBeLinked = canBeLinkedByStraightLine(x1,y1,x2,y2);
    if(canBeLinked){
        if(storeDotsInMatchingLine){
            numOfDotsInMatchingLine = 2;
            dotsInMatchingLine[0] = Dot(x1, y1);
            dotsInMatchingLine[1] = Dot(x2, y2);
        }
        return true;
    }

    //判断是否能通过只折一次的折线连接
    //此时折线的拐点(x,y)行列坐标分别与两点相等
    //也即看(x1,y1)(x,y)和(x2,y2)(x,y)之间分别是否能用不折的直线连接
    //归类为一点行列坐标均比另一点小，和行列坐标之一比另一点小两情况
    if(x1 == x2 || y1 == y2){ //行坐标或列坐标相等时不可能通过折一次的直线连接
    }
    else{
        int x = x1, y = y2;
        canBeLinked = canBeLinkedByStraightLine(x1,y1,x,y) && canBeLinkedByStraightLine(x,y,x2,y2) && gameMap[x][y] <= 0;
        if(canBeLinked){
            if(storeDotsInMatchingLine){
                numOfDotsInMatchingLine = 3;
                dotsInMatchingLine[0] = Dot(x1, y1);
                dotsInMatchingLine[1] = Dot(x, y);
                dotsInMatchingLine[2] = Dot(x2, y2);
            }
            return true;
        }
        x = x2, y = y1;
        canBeLinked = canBeLinkedByStraightLine(x1,y1,x,y) && canBeLinkedByStraightLine(x,y,x2,y2) && gameMap[x][y] <= 0;
        if(canBeLinked){
            if(storeDotsInMatchingLine){
                numOfDotsInMatchingLine = 3;
                dotsInMatchingLine[0] = Dot(x1, y1);
                dotsInMatchingLine[1] = Dot(x, y);
                dotsInMatchingLine[2] = Dot(x2, y2);
            }
            return true;
        }
    }

    //判断是否能通过折两次的折线连接
    //拐点行列坐标与两个点分别相等
    int x,y;
    //中间段与水平方向平行时
    x = 0;
    while(x < numRows){
        canBeLinked = gameMap[x][y1] <= 0 && gameMap[x][y2] <= 0 && canBeLinkedByStraightLine(x,y1,x,y2) && canBeLinkedByStraightLine(x1,y1,x,y1) && canBeLinkedByStraightLine(x2,y2,x,y2);
        if(canBeLinked){
            if(storeDotsInMatchingLine){
                numOfDotsInMatchingLine = 4;
                dotsInMatchingLine[0] = Dot(x1, y1);
                dotsInMatchingLine[1] = Dot(x, y1);
                dotsInMatchingLine[2] = Dot(x, y2);
                dotsInMatchingLine[3] = Dot(x2, y2);
            }
            return true;
        }
        ++x;
    }
    //中间段与竖直方向平行时
    y = 0;
    while(y < numCols){
        canBeLinked = gameMap[x1][y] <= 0 && gameMap[x2][y] <= 0 && canBeLinkedByStraightLine(x1,y,x2,y) && canBeLinkedByStraightLine(x1,y,x1,y1) && canBeLinkedByStraightLine(x2,y2,x2,y);
        if(canBeLinked){
            if(storeDotsInMatchingLine){
                numOfDotsInMatchingLine = 4;
                dotsInMatchingLine[0] = Dot(x1, y1);
                dotsInMatchingLine[1] = Dot(x1, y);
                dotsInMatchingLine[2] = Dot(x2, y);
                dotsInMatchingLine[3] = Dot(x2, y2);
            }
            return true;
        }
        ++y;
    }

    //都不满足，则不存在，返回false
    return false;
}

//
// isSameTypeAndCanBeLinked 接受两个箱子分别的行列坐标(x1,y1)(x2,y2)以及是否需要存储连接线上的关键点的bool值作为参数（该参数默认为false）
// 返回true表示两个箱子类型相同，并且可以通过两次以内的折线进行连接
// 返回false则表明两箱子类型不同（或相应坐标处不为箱子），或者两箱子不可以通过两次以内的折线进行连接
// 如果可以连接并且参数storeDotsInMatchingLine为true
// 此函数会在map中将连线路径上的关键节点信息更新（包括numOfDotsInMatchingLine和dotsInMatchingLine）
//
bool Map::isSameTypeAndCanBeLinked(int x1, int y1, int x2, int y2, bool storeDotsInMatchingLine)
{
    if(x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 || x1 >= numRows || x2 >= numRows || y1 >= numCols || y2 >=numCols) return false;
    if(gameMap[x1][y1] <= 0 || gameMap[x2][y2] <= 0){
        //两行列坐标位置处有一个或以上不是箱子
        return false;
    }
    if(gameMap[x1][y1] != gameMap[x2][y2]){
        //两行列坐标位置处箱子类型不同
        return false;
    }
    return canBeLinked(x1, y1, x2, y2, storeDotsInMatchingLine);
    //返回判断这两坐标之间能否通过折两次以内的折线连接函数的bool值
}


//
// findOneSolution 找到当前地图上一对可以通过折两次以内得线连接的箱子
// 如果找到，返回true，并通过参数x1，x2，y1，y2返回这两个箱子的坐标(x1,y1)(x2,y2)
// 如若不存在，返回false，表示当前地图上任意两个箱子间都不能通过两次以内的折线进行连接，四个坐标参数置为-1
// 此函数不会在map中将连线路径上的关键节点信息更新（包括numOfDotsInMatchingLine和dotsInMatchingLine）
//
bool Map::findOneSolution(int &x1, int &y1, int &x2, int &y2, Character* player1, Character* player2)
{
    BoxNode *p = nullptr, *q = nullptr;
    //从type为1的箱子开始找起，一旦找到一个解（需要满足有角色可以到达）就终止函数返回答案
    for(int i = 1; i < kBoxTypes + 1; ++i){
        p = boxes[i];
        while(p != nullptr){
            x1 = p->x, y1 = p->y;
            q = p->next;
            while(q != nullptr){
                x2 = q->x, y2 = q->y;
                if(canBeLinked(x1, y1, x2, y2) && existPlayerCanReachBoxes(x1, y1, x2, y2, player1, player2))return true;
                q = q->next;
            }
            p = p->next;
        }
    }
    //将五个type的箱子都两两配对枚举后仍没有找到答案，返回false和四个-1坐标
    x1 = -1, x2 = -1, y1 = -1, y2 = -1;
    return false;
}


//
// clearAllBoxesAndReturnBoxNum 把gameMap内所有正数变为0，也即在gameMap中删去所有箱子
// 返回当前地图上箱子的总数
// 对角色和道具不作处理
//
int Map::clearAllBoxesAndReturnBoxNum(bool clearAllMapItems)
{
    int cnt = 0;
    for(int i = 0; i < numRows; ++i){
        for(int j = 0; j < numCols; ++j){
            if(gameMap[i][j] > 0){
                gameMap[i][j] = 0;
                ++cnt;
            }
            if(clearAllMapItems){
                gameMap[i][j] = 0;
            }
        }
    }
    return cnt;
}

//
// returnBoxNum 返回当前地图上箱子的总数
// 对角色和道具不作处理
//
int Map::returnBoxNum()
{
    int cnt = 0;
    for(int i = 0; i < numRows; ++i){
        for(int j = 0; j < numCols; ++j){
            if(gameMap[i][j] > 0){
                ++cnt;
            }
        }
    }
    return cnt;
}

//
// randomlyCreateBoxes 随机在map上没有箱子，角色和道具的位置上生成箱子
// 生成箱子数目为boxNum，生成箱子种类为kBoxTypes
//
void Map::randomlyCreateBoxes(int boxNum)
{
    if(boxNum % 2 != 0){
        std::cout << "FUNC Map::randomlyCreateBoxes: illegal parameter, boxNum should be even" << std::endl;
        return;
    }
    //随机生成map上的箱子
    int boxType;
    int x, y;
    for(int k = 0; k < boxNum; k += 2){
        std::cout << "boxes already created: " << k << '\n';
        boxType = generateRandomNumber(1, kBoxTypes);
        std::cout << "current random box type: " << boxType << '\n';
        do{
            x = generateRandomNumber(0, numRows - 1);
            y = generateRandomNumber(0, numCols - 1);
            std::cout << "random pos result 1: " << x << ' ' << y << '\n';
        } while(gameMap[x][y] != 0);
        gameMap[x][y] = boxType;
        do{
            x = generateRandomNumber(0, numRows - 1);
            y = generateRandomNumber(0, numCols - 1);
            std::cout << "random pos result 2: " << x << ' ' << y << '\n';
        } while(gameMap[x][y] != 0);
        gameMap[x][y] = boxType;
        std::cout << "------------" << std::endl;
    }
}

//
// randomlyCreateProps 随机在map上没有箱子，角色和道具的位置上生成道具
// 同时将所生成的道具加入propsVec
// 此函数需要每一秒钟调用一次
//
void Map::randomlyCreateProps()
{
    if(emptyPlacesLessThan(5)) return; //如果此时地图上空余位置小于五个，为节省运算资源，防止随机数生成时崩溃不生成道具
    int random;
    for(int i = 0; i < kTotalPropTypes; ++i){
        random = generateRandomNumber(1,1000);
        if(random > appearFrequency[i]) continue;
        else{
            if(alreadyAppearTimes[i] >= maxAppearTimes[i]) continue; //如果已经达到一局中最大生成次数，不生成
            if(numOfPlayer == 2 && i == 3) continue; //双人模式下不生成flash道具
            //生成相应标号的道具
            int x, y;
            do{
                x = generateRandomNumber(0, numRows - 1);
                y = generateRandomNumber(0, numCols - 1);
                std::cout << "Props type " << i << " creating, random pos result: " << x << ' ' << y << '\n';
            } while(gameMap[x][y] != 0);
            std::cout << "Props type " << i << " created, finally pos result: " << x << ' ' << y << '\n';
            gameMap[x][y] = (-6) - i; //将道具种类转换为地图中存储的数字
            Props item(i, x, y, appearTimeInSeconds[i], maxAppearTimes[i]);
            propsVec.push_back(item);
            ++alreadyAppearTimes[i]; //增加本局游戏中该道具的生成计数
        }
    }
}

//
// propsShuffle 实现Shuffle道具效果
// 保持现有方块总数不变，将所有方块重新排列
//
void Map::propsShuffle(Character* player1, Character* player2)
{
    do{
        int currentBoxNum = clearAllBoxesAndReturnBoxNum();
        randomlyCreateBoxes(currentBoxNum);
    } while((!currentMapStateSolvable(player1, player2)) || playersSurrounded(player1, player2));
    //运行检查是否有解、角色是否被困在箱子围成的圈里的函数，无解则重新生成

    //将player激活的位置还原为未激活（否则会出现原位置变空，仍有外围黄色框情况）
    player1->selectedBoxRow = -1;
    player1->selectedBoxCol = -1;
    if(numOfPlayer == 2){
        player2->selectedBoxRow = -1;
        player2->selectedBoxCol = -1;
    }
}

//
// propsHint 实现Hint道具效果 传入参数resetStartTime（默认为true）表示是否需要重设Hint道具的开始时间，true则更新hintStart
// 无论传入参数如何，将一组可连接的箱子行列坐标存入hintDots，将hintOn置为true
// 被Hint高亮的boxes绘制，以及如果Hint的箱子中有一个被消除更新Hint的操作在Widget::paintHintBoxes中实现
//
void Map::propsHint(Character* player1, Character* player2, bool resetStartTime)
{
    int x1 = -1, y1 = -1, x2 = -1, y2 = -1;
    if(!findOneSolution(x1, y1, x2, y2, player1, player2)){
        std::cout << "FUNC Map::propsHint: No solution for hint! current map unsolvable!" << std::endl;
    }
    else{
        if(resetStartTime) hintStart = clock();
        hintOn = true;
        hintDots[0] = Dot(x1, y1);
        hintDots[1] = Dot(x2, y2);
        std::cout << "FUNC Map::propsHint: Hint Boxes Found " << x1 << ' ' << y1 << " , " << x2 << ' ' << y2 << std::endl;
        std::cout << "FUNC Map::propsHint: Set hintOn true" << std::endl;
    }
}

//
// initializeTheMap 完成游戏刚开始时对map的初始化操作
// 包括随机生成map上的箱子，随机生成角色位置
//
void Map::initializeTheMap(Character* player1, Character* player2)
{
    do{
        //清空map
        clearAllBoxesAndReturnBoxNum(true);

        //随机生成map上的箱子
        randomlyCreateBoxes(kTotalBoxNum);

        int x, y;
        //随机生成map上的角色位置
        do{ //为playersSurrounded函数判定方便以及游戏体验，角色初始化位置不允许出现在四个角以及四条边上
            x = generateRandomNumber(1, numRows - 2);
            y = generateRandomNumber(1, numCols - 2);
        } while(gameMap[x][y] != 0);
        gameMap[x][y] = -1;
        player1->x = x; //将行列坐标存储到character类的实例player中
        player1->y = y;
        if(numOfPlayer == 2 && player2 != nullptr){
            do{ //为playersSurrounded函数判定方便以及游戏体验，角色初始化位置不允许出现在四个角以及四条边上
                x = generateRandomNumber(1, numRows - 2);
                y = generateRandomNumber(1, numCols - 2);
            } while(gameMap[x][y] != 0);
            gameMap[x][y] = -2;
            player2->x = x; //将行列坐标存储到character类的实例player中
            player2->y = y;
        }
        calculateAndPrintInfo();
    } while((!currentMapStateSolvable(player1, player2)) || playersSurrounded(player1, player2));
    //运行检查是否有解、角色是否被困在箱子围成的圈里的函数，无解则重新生成
}

//
// playersSurrounded 判定当前所有角色是否被四周完全不同的箱子围住导致无解
// 如若被围住且导致无解，返回true，否则返回false
//
bool Map::playersSurrounded(Character* player1, Character* player2)
{
    bool player1Surrounded = true;
    bool player2Surrounded = true;
    //角色初始化位置不允许出现在四个角以及四条边上，因而无论如何gameMap中角色位置上下左右均为合法访问
    int left = gameMap[player1->x][player1->y - 1];
    int right = gameMap[player1->x][player1->y + 1];
    int up = gameMap[player1->x - 1][player1->y];
    int down = gameMap[player1->x + 1][player1->y];
    if(left == 0 || right == 0 || up == 0 || down == 0) player1Surrounded = false; //四周有任意一边是空的，则有解
    if(player1Surrounded && left != right && left != up && left != down && right != up && right != down && up != down){
        std::cout << "FUNC Map::playersSurrounded: surrounded, return true" << std::endl;
        return true;
    }
    if(numOfPlayer == 2){
        left = gameMap[player2->x][player2->y - 1];
        right = gameMap[player2->x][player2->y + 1];
        up = gameMap[player2->x - 1][player2->y];
        down = gameMap[player2->x + 1][player2->y];
        if(left == 0 || right == 0 || up == 0 || down == 0) player2Surrounded = false; //四周有任意一边是空的，则有解
        if(player2Surrounded && left != right && left != up && left != down && right != up && right != down && up != down){
            std::cout << "FUNC Map::playersSurrounded: surrounded, return true" << std::endl;
            return true;
        }
    }
    std::cout << "FUNC Map::playersSurrounded: not surrounded, return false" << std::endl;
    return false;
}

//
// currentMapStateSolvable 判定当前地图是否可解
// 如若当前地图可解，返回true，否则返回false
//
bool Map::currentMapStateSolvable(Character* player1, Character* player2)
{
    //清空上一次进行的构造
    BoxNode* tmp = nullptr;
    for(int i = 0; i < kBoxTypes + 1; ++i){
        while(boxes[i] != nullptr){
            tmp = boxes[i]->next;
            delete boxes[i];
            boxes[i] = tmp;
        }
    }
    //遍历图，将boxes位置信息存入数组
    for(int i = 0; i < numRows; ++i){
        for(int j = 0; j < numCols; ++j){
            if(gameMap[i][j] == 0){
                continue;
            }
            if(gameMap[i][j] == 1){
                tmp = boxes[1];
                boxes[1] = new BoxNode(i,j);
                boxes[1]->next = tmp;
                continue;
            }
            if(gameMap[i][j] == 2){
                tmp = boxes[2];
                boxes[2] = new BoxNode(i,j);
                boxes[2]->next = tmp;
                continue;
            }
            if(gameMap[i][j] == 3){
                tmp = boxes[3];
                boxes[3] = new BoxNode(i,j);
                boxes[3]->next = tmp;
                continue;
            }
            if(gameMap[i][j] == 4){
                tmp = boxes[4];
                boxes[4] = new BoxNode(i,j);
                boxes[4]->next = tmp;
                continue;
            }
            if(gameMap[i][j] == 5){
                tmp = boxes[5];
                boxes[5] = new BoxNode(i,j);
                boxes[5]->next = tmp;
                continue;
            }

        }

    }
    std::cout << "current map solvable checking" << std::endl;
    int x1, y1, x2, y2; //用于存储有解时的坐标
    return findOneSolution(x1, y1, x2, y2, player1, player2);
}

//
// erasePairedBoxes 函数参数为两个行列坐标(x1,y1)(x2,y2)
// 函数在gameMap中把这两个行列坐标位置对应的正数变为零，也即消去这一对箱子
// 如果这两个位置不全为箱子，输出错误信息并不进行任何操作
//
void Map::erasePairedBoxes(int x1, int y1, int x2, int y2)
{
    if(gameMap[x1][y1] <= 0 || gameMap[x2][y2] <= 0){
        std::cout << "FUNC Map::erasePairedBoxes : illegal parameter!" << std::endl;
        return;
    }
    if(gameMap[x1][y1] != gameMap[x2][y2]){
        std::cout << "FUNC Map::erasePairedBoxes : erasing different types of box!" << std::endl;
        return;
    }
    gameMap[x1][y1] = 0;
    gameMap[x2][y2] = 0;
    return;
}

//Debug
void Map::printGameMap() const
{
    for(int i = 0; i < numRows; ++i){
        for(int j = 0; j < numCols; ++j){
            std::cout << gameMap[i][j] << ' ';
        }
        std::cout << std::endl;
    }
}

//Debug
void Map::calculateAndPrintInfo() const
{
    int zero = 0;
    int one = 0, two = 0, three = 0, four = 0, five = 0;
    int ch1 = 0, ch2 = 0;
    int tool1 = 0, tool2 = 0, tool3 = 0;
    int unknown = 0;
    for(int i = 0; i < numRows; ++i){
        for(int j = 0; j < numCols; ++j){
            if(gameMap[i][j] == 0){
                ++zero;
                continue;
            }
            if(gameMap[i][j] == 1){
                ++one;
                continue;
            }
            if(gameMap[i][j] == 2){
                ++two;
                continue;
            }
            if(gameMap[i][j] == 3){
                ++three;
                continue;
            }
            if(gameMap[i][j] == 4){
                ++four;
                continue;
            }
            if(gameMap[i][j] == 5){
                ++five;
                continue;
            }
            if(gameMap[i][j] == -1){
                ++ch1;
                continue;
            }
            if(gameMap[i][j] == -2){
                ++ch2;
                continue;
            }
            if(gameMap[i][j] == -6){
                ++tool1;
                continue;
            }
            if(gameMap[i][j] == -7){
                ++tool2;
                continue;
            }
            if(gameMap[i][j] == -8){
                ++tool3;
                continue;
            }
            ++unknown;
        }
    }
    std::cout << std::endl;
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Map Info: " << std::endl;
    std::cout << "num of empty 0: " << zero << std::endl;
    std::cout << "num of box type 1: " << one << std::endl;
    std::cout << "num of box type 2: " << two << std::endl;
    std::cout << "num of box type 3: " << three << std::endl;
    std::cout << "num of box type 4: " << four << std::endl;
    std::cout << "num of box type 5: " << five << std::endl;
    std::cout << "num of ch1: " << ch1 << std::endl;
    std::cout << "num of ch2: " << ch2 << std::endl;
    std::cout << "num of tool1: " << tool1 << std::endl;
    std::cout << "num of tool2: " << tool2 << std::endl;
    std::cout << "num of tool3: " << tool3 << std::endl;
    std::cout << "num of unknown items: " << unknown << std::endl;
    std::cout << "---------------------------------" << std::endl;
}

//
// emptyPlacesLessThan 函数参数为int类型n
// 函数遍历gameMap，如果其中空位（0）个数小于n个返回true，否则返回false
//
bool Map::emptyPlacesLessThan(int n)
{
    int empty = 0;
    for(int i = 0; i < numRows; ++i){
        for(int j = 0; j < numCols; ++j){
            if(gameMap[i][j] == 0) ++empty;
        }
    }
    if(empty < n) return true;
    else return false;
}

//
// checkMapCanVisit 包裹函数 函数参数起始点 (rowStart, colStart) 和终止点 (rowEnd, colEnd) 行列坐标
// 函数检查行列坐标从起始点 (rowStart, colStart) 到终止点 (rowEnd, colEnd) 是否有一条通路（中间连接的路上为空、角色或者道具，也即两个点不被箱子完全分隔开），如果有返回true
// 如果检查的 (rowEnd,colEnd) 位置为箱子，使用函数时需要将最后一个参数 changeEndBoxToEmpty 设为true（默认值为false）以保证递归函数 checkCurrentAndSurroundCanVisit 可以正确运行
//
bool Map::checkMapCanVisit(int rowStart, int colStart, int rowEnd, int colEnd, bool changeEndBoxToEmpty)
{
    int endBoxType = gameMap[rowEnd][colEnd];
    for(int i = 0; i < numRows; ++i){
        for(int j = 0; j < numCols; ++j){
            canVisitChecked[i][j] = false;
        }
    }
    if(changeEndBoxToEmpty) gameMap[rowEnd][colEnd] = 0;
    bool canVisit = false; //记录是否可以从起始点位到终止
    checkCurrentAndSurroundCanVisit(rowStart, colStart, canVisit, rowEnd, colEnd);
    if(changeEndBoxToEmpty) gameMap[rowEnd][colEnd] = endBoxType;
    return canVisit;
}

//
// checkCurrentAndSurroundCanVisit 检查当前行列坐标 (row, col) 位置是否为目标位置，且该位置不是箱子
// 满足条件函数将 canVisit 置为 true ，否则不改变 canVisit ，转而检查四周（如果不是边界，也不是箱子）位置是否为目标位置
// 传入参数canVisit用于记录是否可以到达目标点位，当前需要检查位置行列坐标 (row, col)，目标位置行列坐标 (rowEnd, colEnd)
//
void Map::checkCurrentAndSurroundCanVisit(int row, int col, bool &canVisit, int rowEnd, int colEnd)
{
    if(canVisitChecked[row][col] == true){
        return;
    }
    canVisitChecked[row][col] = true;
    //检查当前的位置
    if(row == rowEnd && col == colEnd && gameMap[row][col] <= 0){
        canVisit = true;
        return;
    }
    if(row != 0 && gameMap[row - 1][col] <= 0){ //检查上边的框
        checkCurrentAndSurroundCanVisit(row - 1, col, canVisit, rowEnd, colEnd);
    }
    if(col != 0 && gameMap[row][col - 1] <= 0){ //检查左边的框
        checkCurrentAndSurroundCanVisit(row, col - 1, canVisit, rowEnd, colEnd);
    }
    if(row != numRows - 1  && gameMap[row + 1][col] <= 0){ //检查下边的框
        checkCurrentAndSurroundCanVisit(row + 1, col, canVisit, rowEnd, colEnd);
    }
    if(col != numCols - 1  && gameMap[row][col + 1] <= 0){ //检查右边的框
        checkCurrentAndSurroundCanVisit(row, col + 1, canVisit, rowEnd, colEnd);
    }
}

//
// existPlayerCanReachBoxes 函数参数 x1,y1,x2,y2 为接受检验的箱子对行列坐标，player1与player2为指向角色的指针
// 函数检查行列坐标(x1,y1)(x2,y2)的箱子对是否都同时能被场上某一角色访问到，如果存在一名角色可以通过空地到达这两个箱子，返回true，否则返回false
//
bool Map::existPlayerCanReachBoxes(int x1, int y1, int x2, int y2, Character* player1, Character* player2)
{

    bool existPlayerCanReachBoxes = false;
    int player1Row = player1->x, player1Col = player1->y;
    bool player1CanReachBoxes = checkMapCanVisit(player1Row, player1Col, x1, y1, true) && checkMapCanVisit(player1Row, player1Col, x2, y2, true);
    existPlayerCanReachBoxes = player1CanReachBoxes;
    if(player2 != nullptr && !existPlayerCanReachBoxes){
        int player2Row = player2->x, player2Col = player2->y;
        bool player2CanReachBoxes = checkMapCanVisit(player2Row, player2Col, x1, y1, true) && checkMapCanVisit(player2Row, player2Col, x2, y2, true);
        existPlayerCanReachBoxes = player2CanReachBoxes;
    }
    return existPlayerCanReachBoxes;
}
