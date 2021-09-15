#include "gameoverwidget.h"

GameOverWidget::GameOverWidget(int numOfPlayer, int player1Score, int player2Score, QWidget *parent) : QWidget(parent)
{
    this->numOfPlayer = numOfPlayer;
    this->player1Score = player1Score;
    this->player2Score = player2Score;

    setWindowTitle(tr("QLink: Game Over"));
    setFixedSize(3050, 1800);

    gameOverMenuButton = new QPushButton(this);
    gameOverMenuButton->setText("Menu");
    gameOverMenuButton->setGeometry(3050 / 2 - kButtonWidth / 2, 1130, kButtonWidth, kButtonHeight);
    gameOverMenuButton->show();
    gameOverMenuButton->setFont(buttonTextFont);
    connect(gameOverMenuButton, SIGNAL(clicked()), this, SLOT(gameOverMenuButtonClicked()));

    gameOverQuitButton = new QPushButton(this);
    gameOverQuitButton->setText("Quit");
    gameOverQuitButton->setGeometry(3050 / 2 - kButtonWidth / 2, 1300, kButtonWidth, kButtonHeight);
    gameOverQuitButton->show();
    gameOverQuitButton->setFont(buttonTextFont);
    connect(gameOverQuitButton, SIGNAL(clicked()), this, SLOT(gameOverQuitButtonClicked()));
}

GameOverWidget::~GameOverWidget()
{
    delete gameOverMenuButton;
    delete gameOverQuitButton;
}

void GameOverWidget::gameOverQuitButtonClicked()
{
    std::cout << "FUNC GameOverWidget::gameOverQuitButtonClicked: step in" << std::endl;
    gameStatus = QUIT;
    this->close();
}

void GameOverWidget::gameOverMenuButtonClicked()
{
    std::cout << "FUNC GameOverWidget::gameOverMenuButtonClicked: step in" << std::endl;
    gameStatus = MENU;
    this->close();
}

void GameOverWidget::closeEvent(QCloseEvent *event)
{
    emit exitGameOverWidget();
    std::cout << "FUNC GameOverWidget::closeEvent" << std::endl;
    event->accept();
}


void GameOverWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QFont gameOverTextFont = QFont("Microsoft YaHei", 70, QFont::Black, true);
    painter.setFont(gameOverTextFont);
    painter.setPen(QColor(255,255,255));
    //页面上显示游戏结束
    QRect rect1(0, 0, 3050, 850);
    QString str1;
    if(gameStatus == OVER) str1 = QString("Game Over");
    if(gameStatus == WIN) str1 = QString("You Win!");
    if(gameStatus == UNSOLVABLE) str1 = QString("Map Unsolvable");
    painter.drawText(rect1, Qt::AlignCenter, str1);

    //打印游戏结果
    QString strResult;
    QString strScore, strScore2;
    bool strAllUsed = false;
    if(numOfPlayer == 1){ //单人模式
        if(gameStatus == OVER) strResult = QString("Opps! Time Out!");
        if(gameStatus == WIN) strResult = QString("Excellent! All Boxes Linked!");
        if(gameStatus == UNSOLVABLE) strResult = QString("Try Again For Better Luck ~");
        strScore = QString("Your Final Score: %1").arg(player1Score);
    }
    else{ //双人模式
        //判断玩家输赢
        if(player1Score == player2Score){
            //平局
            strResult = QString("TIE");
            strScore = QString("Player 1  Final Score: %1").arg(player1Score);
            strScore2 = QString("Player 2  Final Score: %1").arg(player2Score);
            strAllUsed = true;
        }
        else if(player1Score > player2Score){
            //玩家一胜利
            strResult = QString("Player 1 WIN !");
            strScore = QString("Player 1  Final Score: %1").arg(player1Score);
            strScore2 = QString("Player 2  Final Score: %1").arg(player2Score);
            strAllUsed = true;
        }
        else{
            //玩家二胜利
            strResult = QString("Player 2 WIN !");
            strScore = QString("Player 1  Final Score: %1").arg(player1Score);
            strScore2 = QString("Player 2  Final Score: %1").arg(player2Score);
            strAllUsed = true;
        }
    }
    QFont gameResultTextFont = QFont("Times", 33, QFont::Black);
    painter.setFont(gameResultTextFont);
    QRect rect2(0, 410, 3050, 550);
    painter.drawText(rect2, Qt::AlignCenter, strResult);
    QFont gameScoreTextFont = QFont("Microsoft YaHei", 20, QFont::Black);
    painter.setFont(gameScoreTextFont);
    QRect rect3(0, 550, 3050, 600);
    painter.drawText(rect3, Qt::AlignCenter, strScore);
    if(strAllUsed){
        QRect rect4(0, 600, 3050, 750);
        painter.drawText(rect4, Qt::AlignCenter, strScore2);
    }
}
