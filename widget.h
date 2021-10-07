#ifndef WIDGET_H
#define WIDGET_H

#include "character.h"
#include "map.h"
#include "props.h"
#include "utils.h"
#include "progressbar.h"
#include "Global.h"
#include "gameoverwidget.h"
#include <vector>
#include <time.h>
#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QTimerEvent>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QTextStream>
#include <sstream>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

extern GameStatus gameStatus;

class Widget : public QWidget
{
    Q_OBJECT

    friend QPushButton;
public:
    Widget(int numOfPlayer, QWidget *parent = nullptr);
    ~Widget();

public:
    Ui::Widget *ui;
    Map *map = nullptr;
    Character *player1 = nullptr;
    Character *player2 = nullptr;
    ProgressBar *bar = nullptr;
    QPushButton *pauseButton = nullptr;
    QPushButton *quitButton = nullptr;
    QPushButton *loadButton = nullptr;
    QPushButton *saveButton = nullptr;
    int numOfPlayer; //记录游戏玩家的个数

    //与游戏结束后弹窗有关的变量
    QWidget* shade = nullptr; //半透明遮罩
    GameOverWidget* gameOverWidget = nullptr; //游戏结束后弹出的弹窗

    //与游戏时间有关的变量
    clock_t gameStartTime; //记录游戏的开始时间
    clock_t gameCurrentTime; //记录当前游戏时间
    clock_t gameTotalPauseTime = 0; //记录游戏开始到现在总的暂停时间
    clock_t gameCurrentPauseStartTime; //记录当前暂停开始的时间
    int alreadyUsedSeconds = 0;
    int gameLeftSeconds = kTotalGameTimeInSeconds; //记录当前游戏剩余时间
    double gameTimePercentageLeft = 0.0; //记录游戏剩余时间百分比，如剩60%则值为60

    int updateTimer; //QTimer标签，用于每间隔 kScreenRefreshingInterval 计时，调用 paintevent 的 update 函数
    const int kScreenRefreshingInterval = 200; //每隔多少时间重新绘制一次游戏主界面，单位ms

    int propsGenerateTimer; //QTimer标签，用于每间隔 kPropsGenerateInterval 计时，调用道具随机生成函数
    const int kPropsGenerateInterval = 1000; //每隔多少时间调用一次道具随机生成函数，单位ms

    const int kTotalGameTimeInSeconds = 300; //一场游戏开始时的总时间，单位s
    int TotalGameTimeInSeconds = kTotalGameTimeInSeconds; //某时刻考虑道具的情况下，一场游戏实际的总时间，单位s

    const int kEdgeLength = 80; //地图的格子每格边长为edgeLength
    const int kNumRows = 20; //地图行数
    const int kNumCols = 30; //地图列数

    //游戏名与模式文字参数
    QFont gameNameTextFont = QFont("Microsoft YaHei", 30, QFont::Black); //进行游戏页面游戏名文字字体
    QFont gameModeTextFont = QFont("Microsoft YaHei", 20, QFont::Black, true); //进行游戏页面游戏模式文字字体

    //按钮参数
    const int kButtonWidth = 200; //按钮宽度
    const int kButtonHeight = 80; //按钮高度
    QFont buttonTextFont = QFont("Times", 12, QFont::Bold); //按钮上文字字体

    //与环形进度条有关的常数
    const int kCenterPosRow = kEdgeLength * 4;
    const int kCenterPosCol = kEdgeLength * (kNumCols + 4);
    const int kOutsideRadius = kEdgeLength * 3;
    const int kInsideRadius = kEdgeLength * 2;

    //与文字显示有关的常数
    const int kRectLeftUpRow = kCenterPosRow + kOutsideRadius + kEdgeLength; // 文字外围长方形左上角行坐标
    const int kRectLeftUpCol = kCenterPosCol - kOutsideRadius; // 文字外围长方形左上角列坐标
    const int kRectLength = kEdgeLength * 8; //单行文字外围长方形长度
    const int kRectHeight = kEdgeLength; //单行文字外围长方形高度
    QFont f = QFont("Microsoft YaHei", 15, QFont::Bold); //角色分数显示文字样式
    QFont f2 = QFont("Microsoft YaHei", 10, QFont::Medium); //成功消除显示文字样式

    //与箱体绘制有关的常数，箱子由正方形表示，箱体序号不同颜色不同
    const int kBoxLength = kEdgeLength * 0.8; //箱体边长为0.8倍的edgeLength
    const int kBoxFrameWidth = 10; //箱体被选择之后边框的粗细
    //定义kBoxType个箱体的颜色
    QColor boxColor[6] = { //使用一个数组存储箱体颜色
        QColor(0,0,0), //闲置
        QColor(127,0,127), //序号为1的箱体颜色
        QColor(255,0,0), //序号为2的箱体颜色
        QColor(0,255,0), //序号为3的箱体颜色
        QColor(0,0,255), //序号为4的箱体颜色
        QColor(127,127,0) //序号为5的箱体颜色
    };

    //与角色绘制有关的常数，角色由圆形表示，不同角色颜色不同
    const int kCharacterRadius = kEdgeLength * 0.4; //代表角色的园半径为0.4倍的edgeLength
    //定义角色的颜色
    QColor characterColor[3] = {
        QColor(0,0,0), //闲置
        QColor(255,255,0), //ch1颜色，黄色
        QColor(255,140,0) //ch2颜色，深橙色
    };

     const int kShowTime = 2000; //配对成功，失败事件发生后有关信息在ui界面上显示的时间，单位ms

    //与将要消除的配对箱子连线的绘制有关的变量
    const int kMatchingLineWidth = 15;
    Qt::BrushStyle matchedBoxBrushStyle = Qt::DiagCrossPattern; //绘制将消除状态箱子所用的brushstyle
    struct MatchEvent //一个箱子配对连线的事件
    {
        clock_t start; //存放进入的时间
        int characterNo; //存放是哪个角色消除的
        int numOfDotsInMatchingLine = 0; //连接两个可消除的箱体的连线的关键点个数，包括端点和转折点（直线即为2，折一次即为3，折两次即为4）
        Dot dotsInMatchingLine[4]; //连接两个可消除的箱体的连线的关键点坐标
        //开头和结尾两点为起始点和终点
    };
    std::vector<MatchEvent>::iterator it;
    std::vector<MatchEvent> vec;
    //在函数eventsAfterKeyPress中加入新元素，在paintMatchingLinesAndBoxes删除到时间的元素

    struct FailToMatchEvent //一个箱子配对连线失败的事件
    {
        clock_t start; //存放进入的时间
        int characterNo; //存放是哪个角色的行为
    };
    std::vector<FailToMatchEvent>::iterator _it;
    std::vector<FailToMatchEvent> _vec;

    //与游戏时间有关的函数
    void updateGameTimePercentageLeft(); //计算剩余时间占总时间百分比的函数

    //与道具绘制有关的变量
    QFont fAddTime = QFont("Microsoft YaHei", 12, QFont::Bold); // +1s 道具所用的文字样式
    QColor cAddTime = QColor("#555555"); // +1s 道具所用的颜色
    const char* hintImagePath = "D:/SJTU/SEP/HW/Project1/QLink/res/image/props/hint.png"; // Hint 道具所用图片存储的绝对路径

    //与道具功能有关的常量
    const int kAddTimeInSeconds = 30; //触发一次 +1s 为游戏增加的时间
    const int kHintTime = 10; //触发一次 Hint 道具所持续的时间，单位s
    const int kFlashTime = 5; //触发一次 Flash 道具所持续的时间，单位s
    QColor kHintBoxColor = QColor(139,69,19);
    Qt::BrushStyle kHintBoxBrushStyle = Qt::Dense1Pattern;
    bool hintFlash = true; //用于制造hint提示的闪烁效果，true时调用paintHintBoxes，否则不调用
    int hintFlashTimer; //QTimer标签，用于每间隔 kHintFlashInterval 计时，转换 hintFlash 的真假
    const int kHintFlashInterval = kScreenRefreshingInterval * 1.5; //每隔多少时间hint提示的箱子闪烁一次

protected:

    //绘制ui界面的函数
    virtual void paintEvent(QPaintEvent *event) override;
    void paintMapFrame(QPainter &painter) const;
    void paintBox(int i, int j, QPainter &painter) const;
    void paintCharacterAndSelectedBox(int i, int j, Character* &player, QPainter &painter) const;
    void paintMatchingLinesAndBoxes(QPainter &painter);
    void paintFailToMatchInfo(QPainter &painter);
    void paintCharacterScore(QPainter &painter);
    void paintProps(QPainter &painter); //绘制道具
    void paintHint(int leftUpColPos, int leftUpRowPos, QPainter &painter); //绘制Hint道具
    void paintShuffle(int leftUpColPos, int leftUpRowPos, QPainter &painter); //绘制Shuffle道具
    void paintFlash(int leftUpColPos, int leftUpRowPos, QPainter &painter); //绘制Flash道具
    void paintAddTime(int leftUpColPos, int leftUpRowPos, QPainter &painter); //绘制AddTime道具
    void paintHintBoxes(QPainter &painter); //判定此时是否在Hint道具时效内，如果是绘制高亮的一对解
    void paintGameNameAndMode(QPainter &painter); //打印游戏名和当前模式

    //处理键盘指令的函数
    virtual void keyPressEvent(QKeyEvent *event) override;
    void eventsAfterKeyPress(int chX, int chY, int itemX, int itemY, Character* &player, Character* &playerAnother);

    //处理鼠标指令的函数
    virtual void mousePressEvent(QMouseEvent *event) override;

    //与完成道具功能有关的函数
    void propsAddTime();
    void propsFlash(int rowEnd, int colEnd); //只针对单人模式有效

    //与游戏时间进程有关的函数
    virtual void timerEvent(QTimerEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
public:
    void gameOver(GameStatus status);
public:
    bool showPause = true; //如果为true，此时PauseButton按钮应该显示的文字为Pause
public slots:
    void quitButtonClicked();
    void loadButtonClicked();
    void saveButtonClicked();
    void pauseButtonClicked();
signals:
    void exitWidget(); //widget关闭则发出该信号
};
#endif // WIDGET_H
