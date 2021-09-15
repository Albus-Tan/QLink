#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QTextStream>
#include "widget.h"

extern GameStatus gameStatus;

// 开始菜单类
class Menu : public QMainWindow
{
    Q_OBJECT
public:
    Menu(QWidget *parent = nullptr);
    ~Menu();

    Widget *gameWidget = nullptr;

    const int kButtonWidth = 300; //按钮宽度
    const int kButtonHeight = 120; //按钮高度
    QFont buttonTextFont = QFont("Times", 15, QFont::Bold); //按钮上文字字体

    QPushButton *newGame = nullptr;
    QPushButton *load = nullptr;
    QPushButton *quit = nullptr;
    QPushButton *singlePlayer = nullptr;
    QPushButton *twoPlayers = nullptr;

public slots:
    void newGameButtonClicked();
    void singlePlayerButtonClicked();
    void twoPlayersButtonClicked();
    void loadButtonClicked();
    void quitButtonClicked();
    void decideShowMenuOrNot();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
};

#endif // MENU_H
