#ifndef GAMEOVERWIDGET_H
#define GAMEOVERWIDGET_H

#include "Global.h"
#include <QWidget>
#include <QPushButton>
#include <iostream>
#include <QCloseEvent>
#include <QPainter>

extern GameStatus gameStatus;

class GameOverWidget : public QWidget
{
    Q_OBJECT
private:
    //按钮参数
    const int kButtonWidth = 400; //按钮宽度
    const int kButtonHeight = 120; //按钮高度
    QFont buttonTextFont = QFont("Times", 16, QFont::Bold); //按钮上文字字体
    int numOfPlayer;
    int player1Score = -1;
    int player2Score = -1;
public:
    explicit GameOverWidget(int numOfPlayer, int player1Score, int player2Score, QWidget *parent = nullptr);
    ~GameOverWidget();
    QPushButton* gameOverQuitButton = nullptr;
    QPushButton* gameOverMenuButton = nullptr;
public slots:
    void gameOverQuitButtonClicked();
    void gameOverMenuButtonClicked();
signals:
    void exitGameOverWidget();
protected:
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
};

#endif // GAMEOVERWIDGET_H
