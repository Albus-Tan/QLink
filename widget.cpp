#include "widget.h"
#include "ui_widget.h"

Widget::Widget(int numOfPlayer, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle(tr("QLink"));
    // 设置 widget 大小
    setFixedSize(3050, 1800);

    this->numOfPlayer = numOfPlayer;

    pauseButton = new QPushButton();
    pauseButton->setParent(this);
    pauseButton->setText("Pause");
    pauseButton->setGeometry(2510, 1680, kButtonWidth, kButtonHeight);
    pauseButton->show();
    pauseButton->setFont(buttonTextFont);
    connect(pauseButton, SIGNAL(clicked()), this, SLOT(pauseButtonClicked()));

    loadButton = new QPushButton();
    loadButton->setParent(this);
    loadButton->setText("Load");
    loadButton->setGeometry(2810, 1580, kButtonWidth, kButtonHeight);
    loadButton->show();
    loadButton->setFont(buttonTextFont);
    loadButton->setDisabled(true);
    connect(loadButton, SIGNAL(clicked()), this, SLOT(loadButtonClicked()));

    saveButton = new QPushButton();
    saveButton->setParent(this);
    saveButton->setText("Save");
    saveButton->setGeometry(2510, 1580, kButtonWidth, kButtonHeight);
    saveButton->show();
    saveButton->setFont(buttonTextFont);
    saveButton->setDisabled(true);
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));


    quitButton = new QPushButton(this);
    quitButton->setText("Quit");
    quitButton->setGeometry(2810, 1680, kButtonWidth, kButtonHeight);
    quitButton->show();
    quitButton->setFont(buttonTextFont);
    connect(quitButton, SIGNAL(clicked()), this, SLOT(quitButtonClicked()));

    this->grabKeyboard(); //注意此时所有的 keyboard event 均由 widget 接受处理

    map = new Map(numOfPlayer, kNumRows, kNumCols);
    player1 = new Character(map->numRows, map->numCols, 1);
    if(numOfPlayer == 2){
        player2 = new Character(map->numRows, map->numCols, 2);
    }
    bar = new ProgressBar(gameLeftSeconds, kCenterPosRow, kCenterPosCol, kOutsideRadius, kInsideRadius);
    //检查内存是否申请成功

    if(numOfPlayer == 2) map->alreadyAppearTimes[3] = map->maxAppearTimes[3]; //双人模式下没有flash道具

    updateTimer = startTimer(kScreenRefreshingInterval); //每间隔 kScreenRefreshingInterval 计时，调用paintevent的update函数
    propsGenerateTimer = startTimer(kPropsGenerateInterval); //每间隔 kPropsGenerateInterval 计时，调用道具随机生成函数
    hintFlashTimer = startTimer(kHintFlashInterval); //每间隔 kHintFlashInterval 计时，改变hintFlash真假

    map->initializeTheMap(player1, player2);
    gameStartTime = clock();
    map->printGameMap();

}

Widget::~Widget()
{
    delete player1;
    if(numOfPlayer == 2) delete player2;
    delete pauseButton;
    delete quitButton;
    delete loadButton;
    delete saveButton;
    delete map;
    delete bar;
    delete ui;
}

void Widget::updateGameTimePercentageLeft()
{
    if(gameStatus == PLAYING){
        gameCurrentTime = clock();
        double timePassed = (gameCurrentTime - gameStartTime - gameTotalPauseTime) / 1000; //单位s
        std::cout << "FUNC Widget::updateGameTimePercentageLeft: timePassed = " << timePassed << std::endl;
        if((timePassed + alreadyUsedSeconds) > TotalGameTimeInSeconds){
            //游戏时间到
            gameOver(OVER);
            return;
        }
        else{
            gameLeftSeconds = (TotalGameTimeInSeconds - alreadyUsedSeconds - timePassed);
            gameTimePercentageLeft = (100 * gameLeftSeconds / TotalGameTimeInSeconds);
        }
    }
    else{
        return;
    }
}

//
// eventsAfterKeyPress 完成在收到按键指令或flash之后应当做出的相应响应，包括进行判断后移动角色位置，或选中箱体、道具，进行消除加分等交互
// 传入参数chX，chY为角色按键之前坐在的行列坐标，itemX，itemY为角色在按键之后将（也即与其交互的物体）处于的行列坐标
// player为指向当前按键指令操作的角色的指针
//
void Widget::eventsAfterKeyPress(int chX, int chY, int itemX, int itemY, Character* &player, Character* &playerAnother)
{
    //map和character里面存的角色位置都要更新！
    //规定角色移动到地图边界不能再向边界方向移动
    //规定角色碰到箱体后不能再向箱体方向移动
    if(map->gameMap[itemX][itemY] == 0){ //如果与player交互的物体位置是空的
        player->x = itemX;
        player->y = itemY;
        map->gameMap[chX][chY] = 0;
        map->gameMap[itemX][itemY] = -(player->no); //注意，player的no为正整数，需要转换为负数
        update(); return;
    }
    if(map->gameMap[itemX][itemY] > 0){ //如果交互的位置是一个箱子
        //如果这个箱子当前被另一名角色选中，消去另一名角色选中状态
        if(playerAnother != nullptr && playerAnother->selectedBoxRow == itemX && playerAnother->selectedBoxCol == itemY){
            playerAnother->selectedBoxRow = -1;
            playerAnother->selectedBoxCol = -1;
        }
        //如果该角色尚未选择箱体，选择该位置的箱体
        if(player->selectedBoxRow == -1 && player->selectedBoxCol == -1){
            player->selectedBoxRow = itemX;
            player->selectedBoxCol = itemY;
            std::cout << "FUNC Widget::eventsAfterKeyPress: player select box " << itemX << ' ' << itemY << std::endl;
            update(); return;
        }
        else{ //该角色已经有选择了箱体，判断这两个箱体是否能被消除，如果两次选择的箱体是同一个不能消除
            //调用isSameTypeAndCanBeLinked函数，并且最后一个参数设置为true更新map中对于路径的记录
            bool canBeErased = map->isSameTypeAndCanBeLinked(player->selectedBoxRow, player->selectedBoxCol, itemX, itemY, true);
            if(canBeErased){ //如果能够被消除，消除方块，增加角色得分，判定余下地图是否可解
                //创建新的MatchEvent并加入vector，vector会在paintMatchingLinesAndBoxes中被不断遍历绘制，直至到时间被删去为止
                MatchEvent event;
                event.start = clock();
                event.characterNo = player->no;
                event.numOfDotsInMatchingLine = map->numOfDotsInMatchingLine;
                for(int i = 0; i < event.numOfDotsInMatchingLine; ++i){
                    event.dotsInMatchingLine[i] = map->dotsInMatchingLine[i];
                }
                map->numOfDotsInMatchingLine = 0; //取出后将点数归零
                vec.push_back(event); //将event加入vector
                //增加角色得分（必须写在消除方块之前）
                player->score += map->kBoxScore[map->gameMap[itemX][itemY]];
                //消除方块
                map->erasePairedBoxes(itemX, itemY, player->selectedBoxRow, player->selectedBoxCol);
                //判定角色是否消除所有方块从而胜利
                if(map->returnBoxNum() == 0){
                    if(numOfPlayer == 1) gameOver(WIN);
                    if(numOfPlayer == 2) gameOver(OVER);
                    return;
                }
                //判定余下地图是否可解
                bool solvable = map->currentMapStateSolvable(player1, player2);
                if(!solvable){
                    gameOver(UNSOLVABLE);
                    return;
                }
            }
            else{
                std::cout << "FUNC Widget::eventsAfterKeyPress: selected box matching failed " << itemX << ' ' << itemY << std::endl;
                //如果不能被消除，输出判定结果“不可消除”显示在界面上
                //创建新的FailToMatchEvent并加入vector，vector会在paintFailToMatchInfo中被不断遍历绘制，直至到时间被删去为止
                FailToMatchEvent event;
                event.start = clock();
                event.characterNo = player->no;
                _vec.push_back(event); //将event加入vector
            }
            //无论如何，都重置方块为未激活状态
            player->selectedBoxRow = -1;
            player->selectedBoxCol = -1;
            update(); return;
        }
    }
    if(map->gameMap[itemX][itemY] < 0){ //如果交互位置是道具或者角色
        //如果交互位置是角色，则两者位置都不发生变化
        if(map->gameMap[itemX][itemY] == -1 || map->gameMap[itemX][itemY] == -2){
            update(); return;
        }
        //如果交互位置是道具
        if(map->gameMap[itemX][itemY] <= -6){
            std::cout << "Interacting with prop type " << (-6) - map->gameMap[itemX][itemY] << std::endl;
            int propType = (-6) - map->gameMap[itemX][itemY];
            //删除道具，并且角色位置变到道具处
            player->x = itemX;
            player->y = itemY;
            map->gameMap[chX][chY] = 0;
            map->gameMap[itemX][itemY] = -(player->no); //注意，player的no为正整数，需要转换为负数
            //调用执行相应道具功能的函数（包括道具效果），必须放在角色吃完道具其位置改变后
            switch(propType){
            case 0: //道具0 +1s
                propsAddTime();
                break;
            case 1: //道具1 Shuffle
                map->propsShuffle(player1, player2);
                break;
            case 2: //道具2 Hint
                map->propsHint(player1, player2, true);
                break;
            case 3: //道具3 Flash
                map->flashOn = true;
                map->flashStart = clock();
                break;
            }
            update(); return;
        }
    }

}

void Widget::keyPressEvent(QKeyEvent *event)
{

    if(gameStatus != PLAYING) return;
    //map和character里面存的角色位置都要更新！
    //规定角色移动到地图边界不能再向边界方向移动
    //规定角色碰到箱体后不能再向箱体方向移动
    int chX, chY; //角色位置，注意是行列坐标而不是x，y坐标！
    int itemX, itemY; //角色这次按照keyPress位移之后，可能与其交互的物体所在的位置

    //角色一用↑↓←→控制向上下左右方向的移动
    chX = player1->x, chY = player1->y;
    if(event->key() == Qt::Key_Up){
        if(chX == 0) { update(); return;} //已经到达最上面一行
        itemX = chX - 1, itemY = chY;
        eventsAfterKeyPress(chX, chY, itemX, itemY, player1, player2);
        return;
    }
    if(event->key() == Qt::Key_Down){
        if(chX == map->numRows - 1) { update(); return;} //已经到达最下面一行
        itemX = chX + 1, itemY = chY;
        eventsAfterKeyPress(chX, chY, itemX, itemY, player1, player2);
        return;
    }
    if(event->key() == Qt::Key_Left){
        if(chY == 0) { update(); return;} //已经到达最左边一列
        itemX = chX, itemY = chY - 1;
        eventsAfterKeyPress(chX, chY, itemX, itemY, player1, player2);
        return;
    }
    if(event->key() == Qt::Key_Right){
        if(chY == map->numCols - 1) { update(); return;} //已经到达最右边一列
        itemX = chX, itemY = chY + 1;
        eventsAfterKeyPress(chX, chY, itemX, itemY, player1, player2);
        return;
    }

    //角色二用WSAD控制向上下左右方向的移动
    if(numOfPlayer == 2 && player2 != nullptr){
        chX = player2->x, chY = player2->y;
        if(event->key() == Qt::Key_W){
            if(chX == 0) { update(); return;} //已经到达最上面一行
            itemX = chX - 1, itemY = chY;
            eventsAfterKeyPress(chX, chY, itemX, itemY, player2, player1);
            return;
        }
        if(event->key() == Qt::Key_S){
            if(chX == map->numRows - 1) { update(); return;} //已经到达最下面一行
            itemX = chX + 1, itemY = chY;
            eventsAfterKeyPress(chX, chY, itemX, itemY, player2, player1);
            return;
        }
        if(event->key() == Qt::Key_A){
            if(chY == 0) { update(); return;} //已经到达最左边一列
            itemX = chX, itemY = chY - 1;
            eventsAfterKeyPress(chX, chY, itemX, itemY, player2, player1);
            return;
        }
        if(event->key() == Qt::Key_D){
            if(chY == map->numCols - 1) { update(); return;} //已经到达最右边一列
            itemX = chX, itemY = chY + 1;
            eventsAfterKeyPress(chX, chY, itemX, itemY, player2, player1);
            return;
        }
    }
}

//
// paintHint 完成道具 Hint 的绘制
// 传入参数painter为paintEvent中的画笔，leftUpRowPos和leftUpColPos分别为绘制的道具外围矩形左上角行列坐标
//
void Widget::paintHint(int leftUpColPos, int leftUpRowPos, QPainter &painter)
{
    QRect Rect(leftUpColPos, leftUpRowPos, kEdgeLength, kEdgeLength);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPixmap(Rect, QPixmap(hintImagePath));
}

//
// paintHint 完成道具 Shuffle 的绘制
// 传入参数painter为paintEvent中的画笔，leftUpRowPos和leftUpColPos分别为绘制的道具外围矩形左上角行列坐标
//
void Widget::paintShuffle(int leftUpColPos, int leftUpRowPos, QPainter &painter)
{
    QRect Rect(leftUpColPos, leftUpRowPos, kEdgeLength, kEdgeLength);
    QStyle* style = QApplication::style();
    QIcon icon = style->standardIcon(QStyle::SP_BrowserReload);
    QPixmap pixmap = icon.pixmap(QSize(kEdgeLength,kEdgeLength));
    painter.drawPixmap(Rect, pixmap);
}

//
// paintHint 完成道具 Flash 的绘制
// 传入参数painter为paintEvent中的画笔，leftUpRowPos和leftUpColPos分别为绘制的道具外围矩形左上角行列坐标
//
void Widget::paintFlash(int leftUpColPos, int leftUpRowPos, QPainter &painter)
{
    QRect Rect(leftUpColPos + kEdgeLength / 4, leftUpRowPos + kEdgeLength / 4, kEdgeLength / 2, kEdgeLength / 2);
    QStyle* style = QApplication::style();
    QIcon icon = style->standardIcon(QStyle::SP_ToolBarHorizontalExtensionButton);
    QPixmap pixmap = icon.pixmap(QSize(kEdgeLength, kEdgeLength));
    painter.drawPixmap(Rect, pixmap);
}

//
// paintHint 完成道具 +1s 的绘制
// 传入参数painter为paintEvent中的画笔，leftUpRowPos和leftUpColPos分别为绘制的道具外围矩形左上角行列坐标
//
void Widget::paintAddTime(int leftUpColPos, int leftUpRowPos, QPainter &painter)
{
    painter.setFont(fAddTime);
    painter.setPen(cAddTime);
    QRectF Rect(leftUpColPos, leftUpRowPos, kEdgeLength, kEdgeLength);
    QString valueStr = QString("+1s");
    painter.drawText(Rect, Qt::AlignCenter, valueStr);
}

//
// paintProps 完成道具的绘制
// 传入参数painter为paintEvent中的画笔
//
void Widget::paintProps(QPainter &painter)
{
    //道具不再被绘制有两种可能
    // 1.被角色使用
    // 2.出现时间达到appearTimeInSeconds
    //此函数处理两种情况下propsVec中的道具删除，以及情况2下gameMap数组中的道具删除（把对应位置存储的值由道具序号变为0）
    //情况1下gameMap数组中的道具删除在eventsAfterKeyPress中进行
    clock_t current = clock();
    //遍历propsVec
    for(map->it = map->propsVec.begin(); map->it != map->propsVec.end(); ){
        if(map->gameMap[map->it->row][map->it->col] > (-6)){ //将gameMap数组中该位置已经不是道具
            //说明道具被角色使用
            //将propsVec中的道具删除
            std::cout << "FUNC Widget::paintProps: Detecting prop type " << map->it->type << " in pos " << map->it->row <<' '<< map->it->col << " being used by character, now delete it in propsVec" << std::endl;
            map->it = map->propsVec.erase(map->it); //注意不能直接删，否则it变为野指针
            //返回的是删除后的下一个
            continue;
        }
        else if((current - map->it->create) >= (1000 * map->it->appearTimeInSeconds)){ //出现时间达到appearTimeInSeconds的道具
            std::cout << "FUNC Widget::paintProps: Detecting prop type " << map->it->type << " in pos " << map->it->row <<' '<< map->it->col << " appear time's up, now delete it" << std::endl;
            map->gameMap[map->it->row][map->it->col] = 0; //将gameMap数组中的道具删除（把对应位置存储的值由道具序号变为0）
            map->it = map->propsVec.erase(map->it); //注意不能直接删，否则it变为野指针
            //返回的是删除后的下一个
            continue;
        }
        else{
            //绘制道具
            int leftUpRowPos = map->it->row * kEdgeLength; //道具所在格左上角行坐标
            int leftUpColPos = map->it->col * kEdgeLength; //道具所在格左上角列坐标
            switch(map->it->type){
                case 0: //绘制 +1s 道具
                    paintAddTime(leftUpColPos, leftUpRowPos, painter);
                    break;
                case 1: //绘制 Shuffle 道具
                    paintShuffle(leftUpColPos, leftUpRowPos, painter);
                    break;
                case 2: //绘制 Hint 道具
                    paintHint(leftUpColPos, leftUpRowPos, painter);
                    break;
                case 3: //绘制 Flash 道具
                    paintFlash(leftUpColPos, leftUpRowPos, painter);
                    break;
            }
            ++(map->it);
        }
    }
}

//
// paintFailToMatchInfo 完成连接失败提示信息的绘制
// 传入参数painter为paintEvent中的画笔
//
void Widget::paintFailToMatchInfo(QPainter &painter)
{
    clock_t current = clock();
    //遍历vector
    for(_it = _vec.begin(); _it != _vec.end(); ){
        if((current - _it->start) >= kShowTime){ //删去到时间的事件
            _it = _vec.erase(_it); //注意不能直接删，否则it变为野指针
            //返回的是删除后的下一个
            continue;
        }
        else{
            //在界面上显示消除失败
            painter.setFont(f2);
            painter.setPen(QColor("#555555"));
            QRectF Rect1(kRectLeftUpCol, kRectLeftUpRow + kEdgeLength * 5, kRectLength, kRectHeight);
            QString valueStr1 = QString("Player1:   Link Failed !!!");
            QRectF Rect2(kRectLeftUpCol, kRectLeftUpRow + kEdgeLength * 6, kRectLength, kRectHeight);
            QString valueStr2 = QString("Player2:   Link Failed !!!");
            if(_it->characterNo == 1)
                painter.drawText(Rect1, Qt::AlignLeft, valueStr1);
            if(_it->characterNo == 2)
                painter.drawText(Rect2, Qt::AlignLeft, valueStr2);
            ++_it; //不要忘记
        }
    }
}

//
// paintMatchingLinesAndBoxes 完成箱子成功连接后，连线，即将消除的箱子样式和消除成功信息的绘制
// 传入参数painter为paintEvent中的画笔
//
void Widget::paintMatchingLinesAndBoxes(QPainter &painter)
{
    clock_t current = clock();
    //遍历vector
    for(it = vec.begin(); it != vec.end(); ){
        if((current - it->start) >= kShowTime){ //删去到时间的事件
            it = vec.erase(it); //注意不能直接删，否则it变为野指针
            //返回的是删除后的下一个
            continue;
        }
        else{
            //在界面上显示成功消除
            painter.setFont(f2);
            painter.setPen(QColor("#555555"));
            QRectF Rect1(kRectLeftUpCol, kRectLeftUpRow + kEdgeLength * 3, kRectLength, kRectHeight);
            QString valueStr1 = QString("Player1: Boxes Successfully Linked!");
            QRectF Rect2(kRectLeftUpCol, kRectLeftUpRow + kEdgeLength * 4, kRectLength, kRectHeight);
            QString valueStr2 = QString("Player2: Boxes Successfully Linked!");
            if(it->characterNo == 1)
                painter.drawText(Rect1, Qt::AlignLeft, valueStr1);
            if(it->characterNo == 2)
                painter.drawText(Rect2, Qt::AlignLeft, valueStr2);

            painter.setPen(QPen(characterColor[it->characterNo], kMatchingLineWidth));
            int x1, y1, x2, y2;
            for(int i = 0; i < it->numOfDotsInMatchingLine - 1; ++i){ //绘制连线
                x1 = it->dotsInMatchingLine[i].col * kEdgeLength + kEdgeLength / 2;
                y1 = it->dotsInMatchingLine[i].row * kEdgeLength + kEdgeLength / 2;
                x2 = it->dotsInMatchingLine[i + 1].col * kEdgeLength + kEdgeLength / 2;
                y2 = it->dotsInMatchingLine[i + 1].row * kEdgeLength + kEdgeLength / 2;
                painter.drawLine(x1, y1, x2, y2);
            }
            //绘制将消除的箱子
            painter.setBrush(QBrush(characterColor[it->characterNo], matchedBoxBrushStyle));
            int boxCenterRow = it->dotsInMatchingLine[it->numOfDotsInMatchingLine - 1].row * kEdgeLength + kEdgeLength / 2;
            int boxCenterCol = it->dotsInMatchingLine[it->numOfDotsInMatchingLine - 1].col * kEdgeLength + kEdgeLength / 2;
            QPoint boxSquare1[4] = { //注意此处传递的是x，y坐标，而不是行列坐标！！
                QPoint(boxCenterCol - kBoxLength / 2, boxCenterRow - kBoxLength / 2),
                QPoint(boxCenterCol + kBoxLength / 2, boxCenterRow - kBoxLength / 2),
                QPoint(boxCenterCol + kBoxLength / 2, boxCenterRow + kBoxLength / 2),
                QPoint(boxCenterCol - kBoxLength / 2, boxCenterRow + kBoxLength / 2)
            };
            painter.drawPolygon(boxSquare1,4);
            boxCenterRow = it->dotsInMatchingLine[0].row * kEdgeLength + kEdgeLength / 2;
            boxCenterCol = it->dotsInMatchingLine[0].col * kEdgeLength + kEdgeLength / 2;
            QPoint boxSquare2[4] = { //注意此处传递的是x，y坐标，而不是行列坐标！！
                QPoint(boxCenterCol - kBoxLength / 2, boxCenterRow - kBoxLength / 2),
                QPoint(boxCenterCol + kBoxLength / 2, boxCenterRow - kBoxLength / 2),
                QPoint(boxCenterCol + kBoxLength / 2, boxCenterRow + kBoxLength / 2),
                QPoint(boxCenterCol - kBoxLength / 2, boxCenterRow + kBoxLength / 2)
            };
            painter.drawPolygon(boxSquare2,4);
            ++it; //不能写在for第二个分号后
        }

    }

}

//
// paintMapFrame 完成地图框架横纵框架线的绘制
// 传入参数painter为paintEvent中的画笔
//
void Widget::paintMapFrame(QPainter &painter) const
{
    QColor lineColor(0,0,0);
    painter.setPen(lineColor);
    painter.setBrush(lineColor);
    int x1 = 0, x2 = map->numCols, y1 = 0, y2 = 0;
    for(; y1 <= map->numRows; ++y1, ++y2)
        painter.drawLine(x1 * kEdgeLength, y1 * kEdgeLength, x2 * kEdgeLength, y2 * kEdgeLength);
    x1 = 0, x2 = 0, y1 = 0, y2 = map->numRows;
    for(; x1 <= map->numCols; ++x1, ++x2)
        painter.drawLine(x1 * kEdgeLength, y1 * kEdgeLength, x2 * kEdgeLength, y2 * kEdgeLength);
    return;
}

//
// paintBox 完成地图中行列坐标为(i,j)的箱体的绘制
// 传入参数painter为paintEvent中的画笔，整型数i，j代表行列坐标为(i,j)
//
void Widget::paintBox(int i, int j, QPainter &painter) const
{
    //所绘制箱体中心的行列坐标
    int boxCenterRow = (kEdgeLength / 2) + i * kEdgeLength;
    int boxCenterCol = (kEdgeLength / 2) + j * kEdgeLength;
    painter.setPen(boxColor[map->gameMap[i][j]]);
    painter.setBrush(boxColor[map->gameMap[i][j]]);
    QPoint boxSquare[4] = { //注意此处传递的是x，y坐标，而不是行列坐标！！
        QPoint(boxCenterCol - kBoxLength / 2, boxCenterRow - kBoxLength / 2),
        QPoint(boxCenterCol + kBoxLength / 2, boxCenterRow - kBoxLength / 2),
        QPoint(boxCenterCol + kBoxLength / 2, boxCenterRow + kBoxLength / 2),
        QPoint(boxCenterCol - kBoxLength / 2, boxCenterRow + kBoxLength / 2)
    };
    painter.drawPolygon(boxSquare,4);
}

//
// paintCharacterAndSelectedBox 完成地图中行列坐标为(i,j)的角色，以及如果该角色有已选择的箱子的绘制
// 传入参数painter为paintEvent中的画笔，整型数i，j代表行列坐标为(i,j)
//
void Widget::paintCharacterAndSelectedBox(int i, int j, Character* &player, QPainter &painter) const
{
    if(player == nullptr) return;
    painter.setPen(characterColor[player->no]);
    painter.setBrush(characterColor[player->no]);
    //注意此处传递的是x，y坐标，而不是行列坐标！！
    QPoint chCenterPos = QPoint((kEdgeLength / 2) + j * kEdgeLength, (kEdgeLength / 2) + i * kEdgeLength);
    painter.drawEllipse(chCenterPos, kCharacterRadius, kCharacterRadius);
    if(player->selectedBoxRow != -1 && player->selectedBoxCol != -1){
        //用ch对应的颜色绘制表示箱体被选择的外框
        painter.setPen(QPen(characterColor[player->no], kBoxFrameWidth));
        painter.setBrush(boxColor[map->gameMap[player->selectedBoxRow][player->selectedBoxCol]]);
        //所绘制箱体中心的行列坐标
        int boxCenterRow = (kEdgeLength / 2) + player->selectedBoxRow * kEdgeLength;
        int boxCenterCol = (kEdgeLength / 2) + player->selectedBoxCol * kEdgeLength;
        QPoint boxSquare[4] = { //注意此处传递的是x，y坐标，而不是行列坐标！！
            QPoint(boxCenterCol - kBoxLength / 2, boxCenterRow - kBoxLength / 2),
            QPoint(boxCenterCol + kBoxLength / 2, boxCenterRow - kBoxLength / 2),
            QPoint(boxCenterCol + kBoxLength / 2, boxCenterRow + kBoxLength / 2),
            QPoint(boxCenterCol - kBoxLength / 2, boxCenterRow + kBoxLength / 2)
        };
        painter.drawPolygon(boxSquare,4);
    }
}

//
// paintCharacterScore 完成角色分数的绘制
// 传入参数painter为paintEvent中的画笔
//
void Widget::paintCharacterScore(QPainter &painter)
{
    //绘制角色得分
    painter.setFont(f);
    painter.setPen(QColor("#555555"));
    //定义确定角色得分文字显示位置的矩形
    QRectF Rect1(kRectLeftUpCol, kRectLeftUpRow, kRectLength, kRectHeight);
    QString valueStr1 = QString("Score of player1:   %1").arg(QString::number(player1->score));
    painter.drawText(Rect1, Qt::AlignLeft, valueStr1);
    if(numOfPlayer == 2 && player2 != nullptr){
        QRectF Rect2(kRectLeftUpCol, kRectLeftUpRow + kEdgeLength, kRectLength, kRectHeight);
        QString valueStr2 = QString("Score of player2:   %1").arg(QString::number(player2->score));
        painter.drawText(Rect2, Qt::AlignLeft, valueStr2);
    }
}

//绘制游戏名和当前模式
void Widget::paintGameNameAndMode(QPainter &painter)
{
    painter.setFont(gameNameTextFont);
    painter.setPen("#555555");
    QRect rect1(240, 1610, 1400, 180);
    QString str1 = QString("QLink");
    painter.drawText(rect1, Qt::AlignCenter, str1);
    painter.setFont(gameModeTextFont);
    QRect rect2(970, 1610, 1000, 180);
    if(numOfPlayer == 1){
        QString str2 = QString("Single Player Mode");
        painter.drawText(rect2, Qt::AlignCenter, str2);
    }
    else{
        QString str3 = QString("Two Players Battle");
        painter.drawText(rect2, Qt::AlignCenter, str3);
    }
}

//
// paintEvent 完成整个游戏中交互界面的绘制
// 传入参数event为事件
//
void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //绘制游戏名和当前模式
    paintGameNameAndMode(painter);
    //绘制时间进度条
    updateGameTimePercentageLeft();
    bar->paintProgressBar(gameLeftSeconds, int(gameTimePercentageLeft), painter);
    //绘制角色得分
    paintCharacterScore(painter);
    //绘制地图的格子
    paintMapFrame(painter);

    //如果当前存在将要消除的两方块之间的连线，绘制连线
    //这一绘制操作必须在角色和箱体的绘制之前，从而可以被角色或者生成的道具覆盖
    paintMatchingLinesAndBoxes(painter);

    //绘制连接失败的信息
    paintFailToMatchInfo(painter);

    //绘制道具
    paintProps(painter);

    //绘制箱子，箱体被激活外框和角色
    for(int i = 0; i < map->numRows; ++i){
        for(int j = 0; j < map->numCols; ++j){
            if(map->gameMap[i][j] == 0) continue; //地图的此处为空
            if(map->gameMap[i][j] < 0){ //绘制角色和道具
                if(map->gameMap[i][j] == -1){ //角色1
                    paintCharacterAndSelectedBox(i, j, player1, painter);
                }
                if(numOfPlayer == 2 && map->gameMap[i][j] == -2){ //角色2
                   paintCharacterAndSelectedBox(i, j, player2, painter);
                }
            }
            if(map->gameMap[i][j] > 0){ //绘制箱子
                //如果该位置的箱子已经被选择，则交给绘制角色选择的箱体部分来绘制，此处跳过
                if(i == player1->selectedBoxRow && j == player1->selectedBoxCol) continue;
                if(player2 != nullptr && i == player2->selectedBoxRow && j == player2->selectedBoxCol) continue;
                paintBox(i, j, painter);
            }
        }
    }

    //绘制Hint道具的提示内容，一定要放在绘制箱子函数之后覆盖前面的绘制
    if(hintFlash) paintHintBoxes(painter);
}

//
// timerEvent 每次对应timer计时时间到后，执行相应的命令
// 传入参数painter为paintEvent中的画笔
//
void Widget::timerEvent(QTimerEvent *event)
{
    if(gameStatus != PLAYING){
        return;
    }
    if(event->timerId() == updateTimer){ //重绘界面
        update();
    }
    if(event->timerId() == propsGenerateTimer){ //执行遍历界面随机生成道具的函数一次
        map->randomlyCreateProps();
    }
    if(event->timerId() == hintFlashTimer){ //hint道具闪烁效果
        hintFlash = !hintFlash;
    }
}

//
// propsAddTime 实现道具 +1s 的功能
// 将游戏实际的总时间增加 kAddTimeInSeconds 秒
//
void Widget::propsAddTime()
{
    TotalGameTimeInSeconds += kAddTimeInSeconds;
}

//
// paintHintBoxes 判定此时是否在Hint道具时效内，如果是绘制高亮的一对解
// 同时如此时高亮的那一对其中有一个箱子已经被消除，或此时该二位置已经不再是一对（期间作用了Shuffle），更新map提示的箱子对，但不更新Hint道具的开始时间
//
void Widget::paintHintBoxes(QPainter &painter)
{
    if(!map->hintOn) return;
    clock_t current = clock();
    if((current - map->hintStart) >= kHintTime * 1000){ //Hint时间到
        map->hintOn = false;
        std::cout << "FUNC Widget::paintHintBoxes: times up, set hintOn false" << std::endl;
        return;
    }
    //判断此时高亮的那一对是否其中有已经被消除的，或此时该二位置已经不再是一对（期间作用了Shuffle）
    int row1 = map->hintDots[0].row;
    int col1 = map->hintDots[0].col;
    int row2 = map->hintDots[1].row;
    int col2 = map->hintDots[1].col;
    if(map->gameMap[row1][col1] <= 0 || map->gameMap[row2][col2] <= 0 || map->gameMap[row1][col1] != map->gameMap[row2][col2]){ //存在其中一个箱子已经被消除，或此时该二位置已经不再是一对（期间作用了Shuffle）
        map->propsHint(player1, player2, false); //更新map提示的箱子对，但不更新Hint道具的开始时间
        std::cout << "FUNC Widget::paintHintBoxes: update hint boxes" << std::endl;
    }
    //绘制高亮的提示箱子对
    int rowLeftUp1 = map->hintDots[0].row * kEdgeLength;
    int colLeftUp1 = map->hintDots[0].col * kEdgeLength;
    int rowLeftUp2 = map->hintDots[1].row * kEdgeLength;
    int colLeftUp2 = map->hintDots[1].col * kEdgeLength;
    QBrush brush;
    brush.setColor(kHintBoxColor);
    brush.setStyle(kHintBoxBrushStyle);
    painter.setBrush(brush);
    QRect Rect1(QPoint(colLeftUp1, rowLeftUp1),QSize(kEdgeLength, kEdgeLength));
    QRect Rect2(QPoint(colLeftUp2, rowLeftUp2),QSize(kEdgeLength, kEdgeLength));
    painter.drawRect(Rect1);
    painter.drawRect(Rect2);
}

void Widget::closeEvent(QCloseEvent *event) //widget关闭调用的函数
{
    if(gameStatus != PLAYING && gameStatus != PAUSE) {
        emit exitWidget();
        event->accept();
        return;
    }
    int choose;
    choose = QMessageBox::question(this, tr("quit QLink"),
                                   QString(tr("Are you sure to quit ?")),
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    std::cout << "FUNC Widget::closeEvent choose == "<< choose << std::endl;
    if (choose == QMessageBox::No){
        std::cout << "FUNC Widget::closeEvent choose == QMessageBox::No" << std::endl;
        event->ignore();
    }
    else if (choose == QMessageBox::Yes){
        std::cout << "FUNC Widget::closeEvent choose == QMessageBox::Yes" << std::endl;
        emit exitWidget();
        gameStatus = QUIT;
        event->accept();
    }
}

void Widget::quitButtonClicked()
{
    std::cout << "FUNC Widget::quitButtonClicked: step in" << std::endl;
    this->close();
}

void Widget::loadButtonClicked()
{
    std::cout << "FUNC Widget::loadButtonClicked: step in" << std::endl;
    //获取文件名称
    QString fileName = QFileDialog::getOpenFileName(this, "OpenFile", QDir::currentPath(), "QLink (*.txt)");
    if(fileName.isEmpty())
    {
         QMessageBox::information(this,"Error","Please select a txt");
         return;
    }
    else
    {
         QFile *file = new QFile;
         file->setFileName(fileName); //新建一个文件对象，并且把它设置为上面获取的文件
         bool ok = file->open(QIODevice::ReadOnly); //设置打开模式

         if(ok)//如果可以打开
         {
              //将文件与文本流相关联
              QTextStream in(file);

              //load游戏内容
              std::stringstream ss;
              std::string s;
              int n, nRows, nCols;
              in.seek(0);
              in.readLine();
              in.readLine();
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              if(n != 1) std::cout << "Load ERROR: GameStatus not PAUSE (1)!" << std::endl; //GameStatus
              ss.clear();
              in.readLine();
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              numOfPlayer = n; //numOfPlayer
              if(numOfPlayer == 2){ //单人变双人模式
                  if(player2 == nullptr) player2 = new Character();
              }
              else{ //双人变单人模式
                  if(player2 != nullptr) delete player2;
              }
              ss.clear();
              in.readLine();
              in.readLine();
              ss << (in.readLine()).toStdString();
              ss >> s >> nRows;
              map->numRows = nRows; //numRows
              player1->numRows = nRows;
              if(player2 != nullptr) player2->numRows = nRows;
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> nCols;
              map->numCols = nCols; //numCols
              player1->numCols = nCols;
              if(player2 != nullptr) player2->numCols = nCols;
              ss.clear();
              in.readLine();
              map->propsVec.clear(); //清空道具vec
              for(int i = 0; i < nRows; ++i){
                  ss << (in.readLine()).toStdString();
                  for(int j = 0; j < nCols; ++j){
                      ss >> n;
                      map->gameMap[i][j] = n;
                      if(n < -5){ //全部道具重新入vec，恢复至重新生成时状态
                          int type = (-6) - n;
                          std::cout << "LOAD: Props type " << type << " loaded and in vec, pos: " << i << ' ' << j << '\n';
                          Props item(type, i, j, map->appearTimeInSeconds[type], map->maxAppearTimes[type]);
                          map->propsVec.push_back(item);
                      }
                  }
                  ss.clear();
              }
              ss << (in.readLine()).toStdString();
              ss >> s;
              for(int i = 0; i < 4; ++i){
                  ss >> n;
                  map->alreadyAppearTimes[i] = n; //alreadyAppearTimes
              }
              ss.clear();
              in.readLine();
              in.readLine();

              //player1
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              player1->no = n; //no
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              player1->x = n; //x
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              player1->y = n; //y
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              player1->score = n; //score
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              player1->selectedBoxRow = n; //selectedBoxRow
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              player1->selectedBoxCol = n; //selectedBoxCol
              ss.clear();

              //player2
              if(numOfPlayer == 2){
                  ss << (in.readLine()).toStdString();
                  ss >> s >> n;
                  player2->no = n; //no
                  ss.clear();
                  ss << (in.readLine()).toStdString();
                  ss >> s >> n;
                  player2->x = n; //x
                  ss.clear();
                  ss << (in.readLine()).toStdString();
                  ss >> s >> n;
                  player2->y = n; //y
                  ss.clear();
                  ss << (in.readLine()).toStdString();
                  ss >> s >> n;
                  player2->score = n; //score
                  ss.clear();
                  ss << (in.readLine()).toStdString();
                  ss >> s >> n;
                  player2->selectedBoxRow = n; //selectedBoxRow
                  ss.clear();
                  ss << (in.readLine()).toStdString();
                  ss >> s >> n;
                  player2->selectedBoxCol = n; //selectedBoxCol
                  ss.clear();
              }

              long m;
              in.readLine();
              in.readLine();
              ss << (in.readLine()).toStdString();
              ss >> s >> m;
              alreadyUsedSeconds = m; //alreadyUsedSeconds
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> m;
              TotalGameTimeInSeconds = m; //TotalGameTimeInSeconds
              ss.clear();
              gameStartTime = clock();
              gameTotalPauseTime = 0;
              gameCurrentPauseStartTime = gameStartTime;

              file->close(); //关闭文件
              delete file; //释放文件进程

              repaint();
              pauseButtonClicked(); //load后直接继续游戏
          }
          else
          {
              QMessageBox::information(this, "Error Box", "FileOpen Error"+file->errorString());
              delete file; //释放文件进程
          }
    }

}

void Widget::saveButtonClicked()
{
    std::cout << "FUNC Widget::saveButtonClicked: step in" << std::endl;
    QString filename = QFileDialog::getSaveFileName(this, "Save File", QDir::currentPath(), "QLink (*.txt)");//获取需要保存成的文件名
    if(filename.isEmpty()){
        QMessageBox::information(this, "ErrorBox", "Please input the filename");
        return;
    }
    else{
        QFile *file = new QFile;
        file->setFileName(filename);
        bool ok = file->open(QIODevice::WriteOnly);
        if(ok){
            QTextStream out(file);

            // save游戏信息
            out << "QLink\n\n";
            out << "GameStatus: " << gameStatus << "\n\n";
            out << "numOfPlayer: " << numOfPlayer << "\n\n";
            out << "MAP\n"
                << "numRows: " << map->numRows << '\n'
                << "numCols: " << map->numCols << '\n'
                << "gameMap:\n";
            for(int i = 0; i < map->numRows; ++i){
                for(int j = 0; j < map->numCols; ++j){
                    out << map->gameMap[i][j] << ' ';
                }
                out << '\n';
            }
            out << "alreadyAppearTimes: ";
            for(int i = 0; i < 4; ++i){
                out << map->alreadyAppearTimes[i] << ' ';
            }
            out << "\n\n";

            out << "CHARACTER\n"
                << "no: " << player1->no << '\n'
                << "x: " << player1->x << '\n'
                << "y: " << player1->y << '\n'
                << "score: " << player1->score << '\n'
                << "selectedBoxRow: " << player1->selectedBoxRow << '\n'
                << "selectedBoxCol: " << player1->selectedBoxCol << '\n';

            if(numOfPlayer == 2){
                out << "no: " << player2->no << '\n'
                    << "x: " << player2->x << '\n'
                    << "y: " << player2->y << '\n'
                    << "score: " << player2->score << '\n'
                    << "selectedBoxRow: " << player2->selectedBoxRow << '\n'
                    << "selectedBoxCol: " << player2->selectedBoxCol << '\n';
            }

            alreadyUsedSeconds += (gameCurrentPauseStartTime - gameStartTime - gameTotalPauseTime) / 1000;
            gameTotalPauseTime = 0;
            gameStartTime = clock();
            gameCurrentPauseStartTime = gameStartTime;
            out << '\n';
            out << "TIME\n"
                << "alreadyUsedSeconds: " << alreadyUsedSeconds << '\n'
                << "TotalGameTimeInSeconds: " << TotalGameTimeInSeconds << '\n';

            file->close();
            delete(file);
        }
        else{
            QMessageBox::information(this,"ErrorBox","file fail to save");
        }
    }
}

void Widget::pauseButtonClicked()
{
    std::cout << "PauseButton::pauseButtonClicked: pause button clicked !" << std::endl;
    //执行被按了之后的响应
    if(showPause){ //表明按的时候是Pause
        pauseButton->setText("Continue");
        gameStatus = PAUSE;
        //更新最近一次pause开始时间
        gameCurrentPauseStartTime = clock();
        //设置按钮 save 和 load 为可用状态
        saveButton->setEnabled(true);
        loadButton->setEnabled(true);
    }
    else{ //表明按的时候是Continue
        pauseButton->setText("Pause");
        gameStatus = PLAYING;
        //更新总暂停时间
        gameTotalPauseTime = gameTotalPauseTime + clock() - gameCurrentPauseStartTime;
        //设置按钮 save 和 load 为不可用状态
        saveButton->setDisabled(true);
        loadButton->setDisabled(true);
    }
    showPause = !showPause; //将当前文字显示状态改变
}

//
// gameOver 游戏结束后调用此函数，完成相应的任务
// 传入参数 GameStatus status 传入相应游戏结束的原因，其中
// WIN 因将游戏页面方块全部消除完而结束
// OVER 因游戏时间到而结束
// UNSOLVABLE 因地图无解而结束
//
void Widget::gameOver(GameStatus status)
{
    //设置游戏状态
    gameStatus = status;

    //为主游戏界面添加阴影遮罩
    shade = new QWidget(this);
    QString str("QWidget{background-color:rgba(0,0,0,0.80);}");
    shade->setStyleSheet(str);
    shade->setGeometry(0, 0, this->width(), this->height());
    shade->show();

    //禁用主游戏界面按钮
    pauseButton->setDisabled(true);
    saveButton->setDisabled(true);
    loadButton->setDisabled(true);
    quitButton->setDisabled(true);

    //新建游戏结束弹窗
    if(numOfPlayer == 1)
        gameOverWidget = new GameOverWidget(numOfPlayer, player1->score, -1, this);
    if(numOfPlayer == 2)
        gameOverWidget = new GameOverWidget(numOfPlayer, player1->score, player2->score, this);
    gameOverWidget->show();

    //gameOverWidget关闭后发出exitGameOverWidget()信号，之后shade与this接受信号也关闭
    connect(gameOverWidget, SIGNAL(exitGameOverWidget()), shade, SLOT(close()));
    connect(gameOverWidget, SIGNAL(exitGameOverWidget()), this, SLOT(close()));
}


void Widget::mousePressEvent(QMouseEvent *event)
{
    std::cout << "FUNC Widget::mousePressEvent: step in" << std::endl;
    if(map->flashOn == false){
        event->ignore();
        return;
    }
    clock_t current = clock();
    if((current - map->flashStart) >= (kFlashTime * 1000)){
        map->flashOn = false;
        event->ignore();
        return;
    }
    std::cout << "FUNC Widget::mousePressEvent: mouse pressed and flash on" << std::endl;
    int x = event->x();
    int y = event->y();
    bool mapClicked = true;
    if(x < 0 || x > kEdgeLength * map->numCols) mapClicked = false;
    if(y < 0 || y > kEdgeLength * map->numRows) mapClicked = false;
    if(mapClicked){
        int row = y / kEdgeLength;
        int col = x / kEdgeLength;
        std::cout << "FUNC Widget::mousePressEvent: click in map, pos " << row << ' ' << col << std::endl;
        propsFlash(row, col);
    }
}

void Widget::propsFlash(int rowEnd, int colEnd)
{
    int rowStart = player1->x;
    int colStart = player1->y;
    if(map->gameMap[rowEnd][colEnd] > 0){ //如果目标点位是箱子，检查四周是否可达
        bool mapCanVisit = false;
        int row = rowEnd, col = colEnd;
        if(!mapCanVisit && rowEnd != 0){ //检查上边的框
            mapCanVisit = (map->gameMap[rowEnd - 1][colEnd] <= 0) && map->checkMapCanVisit(rowStart, colStart, rowEnd - 1, colEnd);
            row = rowEnd - 1;
            col = colEnd;
        }
        if(!mapCanVisit && colEnd != 0){ //检查左边的框
            mapCanVisit = (map->gameMap[rowEnd][colEnd - 1] <= 0) && map->checkMapCanVisit(rowStart, colStart, rowEnd, colEnd - 1);
            row = rowEnd;
            col = colEnd - 1;
        }
        if(!mapCanVisit && rowEnd != map->numRows - 1){ //检查下边的框
            mapCanVisit = (map->gameMap[rowEnd + 1][colEnd] <= 0) && map->checkMapCanVisit(rowStart, colStart, rowEnd + 1, colEnd);
            row = rowEnd + 1;
            col = colEnd;
        }
        if(!mapCanVisit && colEnd != map->numCols - 1){ //检查右边的框
            mapCanVisit = (map->gameMap[rowEnd][colEnd + 1] <= 0) && map->checkMapCanVisit(rowStart, colStart, rowEnd, colEnd + 1);
            row = rowEnd;
            col = colEnd + 1;
        }
        if(mapCanVisit){
            std::cout << "FUNC Map::propsFlash: clicked box pos around checkMapCanVisit true" << std::endl;
            eventsAfterKeyPress(rowStart, colStart, row, col, player1, player2); //先让角色移动到箱子旁边，完成相应交互
            eventsAfterKeyPress(row, col, rowEnd, colEnd, player1, player2); //再看角色触碰箱子，发生的效果
        }
    }
    else{ //目标点位为空或道具，检查目标点位是否可以到达
        if(map->checkMapCanVisit(rowStart, colStart, rowEnd, colEnd)){
            std::cout << "FUNC Map::propsFlash: clicked pos checkMapCanVisit true" << std::endl;
            eventsAfterKeyPress(rowStart, colStart, rowEnd, colEnd, player1, player2);
        }
    }
}
