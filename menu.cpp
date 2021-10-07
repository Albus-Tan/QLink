#include "menu.h"

Menu::Menu(QWidget *parent) : QMainWindow(parent)
{
    std::cout << "FUNC Menu::Menu step in FUNC" << std::endl;
    setWindowTitle(tr("QLink"));
    std::cout << "FUNC Menu::Menu window title set finished" << std::endl;

    // 设置 widget 大小
    setFixedSize(3050, 1800);

    std::cout << "FUNC Menu::Menu window created" << std::endl;

    newGame = new QPushButton("New Game", this);
    newGame->setGeometry(3050 / 2 - kButtonWidth / 2, 100 + 1800 / 2 - kButtonHeight / 2, kButtonWidth, kButtonHeight);
    newGame->setFont(buttonTextFont);
    connect(newGame, SIGNAL(clicked()), this, SLOT(newGameButtonClicked()));
    newGame->show();

    std::cout << "FUNC Menu::Menu newGame Bt shown" << std::endl;

    load = new QPushButton("Load", this);
    load->setGeometry(3050 / 2 - kButtonWidth / 2, 200 + 1800 / 2 + kButtonHeight / 2, kButtonWidth, kButtonHeight);
    load->setFont(buttonTextFont);
    connect(load, SIGNAL(clicked()), this, SLOT(loadButtonClicked()));
    load->show();

    quit = new QPushButton("Quit", this);
    quit->setGeometry(3050 / 2 - kButtonWidth / 2, 220 + 1800 / 2 + 3 * kButtonHeight / 2, kButtonWidth, kButtonHeight);
    quit->setFont(buttonTextFont);
    connect(quit, SIGNAL(clicked()), this, SLOT(quitButtonClicked()));
    quit->show();

    singlePlayer = new QPushButton("Single Player Mode", this);
    singlePlayer->setGeometry(3050 / 2 - kButtonWidth, 30 + 1800 / 2 - kButtonHeight / 2, kButtonWidth * 2, kButtonHeight);
    singlePlayer->setFont(buttonTextFont);
    singlePlayer->hide();
    connect(singlePlayer, SIGNAL(clicked()), this, SLOT(singlePlayerButtonClicked()));

    twoPlayers = new QPushButton("Two Players Battle", this);
    twoPlayers->setGeometry(3050 / 2 - kButtonWidth, 50 + 1800 / 2 + kButtonHeight / 2, kButtonWidth * 2, kButtonHeight);
    twoPlayers->setFont(buttonTextFont);
    twoPlayers->hide();
    connect(twoPlayers, SIGNAL(clicked()), this, SLOT(twoPlayersButtonClicked()));

    std::cout << "FUNC Menu::Menu buttons created" << std::endl;
}

Menu::~Menu()
{
    delete gameWidget;
    delete newGame;
    delete load;
    delete quit;
    delete singlePlayer;
    delete twoPlayers;
}

void Menu::newGameButtonClicked()
{
    // 隐藏无关的button
    newGame->hide();

    // 显示模式选择的按钮
    singlePlayer->show();
    twoPlayers->show();

    // Load Quit 按钮不变

}

void Menu::singlePlayerButtonClicked()
{
    std::cout << "FUNC Menu::singlePlayerButtonClicked: step in" << std::endl;
    gameStatus = PLAYING;
    gameWidget = new Widget(1);
    connect(gameWidget,SIGNAL(exitWidget()),this,SLOT(decideShowMenuOrNot())); //如果gameWidget关闭，检查是否打开menu
    gameWidget->show();
    this->hide();
}

void Menu::twoPlayersButtonClicked()
{
    std::cout << "FUNC Menu::twoPlayersButtonClicked: step in" << std::endl;
    gameStatus = PLAYING;
    std::cout << "FUNC Menu::twoPlayersButtonClicked: ready to create gameWidget" << std::endl;
    gameWidget = new Widget(2);
    std::cout << "FUNC Menu::twoPlayersButtonClicked: gameWidget created" << std::endl;
    connect(gameWidget,SIGNAL(exitWidget()),this,SLOT(decideShowMenuOrNot())); //如果gameWidget关闭，检查是否打开menu
    gameWidget->show();
    this->hide();
}


void Menu::loadButtonClicked()
{
    std::cout << "FUNC Menu::loadButtonClicked: step in" << std::endl;
    //获取文件名称
    QString fileName = QFileDialog::getOpenFileName(this,"OpenFile",QDir::currentPath());
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

              gameStatus = PLAYING;
              gameWidget = new Widget(n);
              connect(gameWidget,SIGNAL(exitWidget()),this,SLOT(decideShowMenuOrNot())); //如果gameWidget关闭，检查是否打开menu
              gameWidget->show();
              this->hide();

              gameWidget->numOfPlayer = n; //numOfPlayer
              ss.clear();
              in.readLine();
              in.readLine();
              ss << (in.readLine()).toStdString();
              ss >> s >> nRows;
              gameWidget->map->numRows = nRows; //numRows
              gameWidget->player1->numRows = nRows;
              if(gameWidget->player2 != nullptr) gameWidget->player2->numRows = nRows;
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> nCols;
              gameWidget->map->numCols = nCols; //numCols
              gameWidget->player1->numCols = nCols;
              if(gameWidget->player2 != nullptr) gameWidget->player2->numCols = nCols;
              ss.clear();
              in.readLine();
              gameWidget->map->propsVec.clear(); //清空道具vec
              for(int i = 0; i < nRows; ++i){
                  ss << (in.readLine()).toStdString();
                  for(int j = 0; j < nCols; ++j){
                      ss >> n;
                      gameWidget->map->gameMap[i][j] = n;
                      if(n < -5){ //全部道具重新入vec，恢复至重新生成时状态
                          int type = (-6) - n;
                          std::cout << "LOAD: Props type " << type << " loaded and in vec, pos: " << i << ' ' << j << '\n';
                          Props item(type, i, j, gameWidget->map->appearTimeInSeconds[type], gameWidget->map->maxAppearTimes[type]);
                          gameWidget->map->propsVec.push_back(item);
                      }
                  }
                  ss.clear();
              }
              ss << (in.readLine()).toStdString();
              ss >> s;
              for(int i = 0; i < 4; ++i){
                  ss >> n;
                  gameWidget->map->alreadyAppearTimes[i] = n; //alreadyAppearTimes
              }
              ss.clear();
              in.readLine();
              in.readLine();

              //player1
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              gameWidget->player1->no = n; //no
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              gameWidget->player1->x = n; //x
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              gameWidget->player1->y = n; //y
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              gameWidget->player1->score = n; //score
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              gameWidget->player1->selectedBoxRow = n; //selectedBoxRow
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> n;
              gameWidget->player1->selectedBoxCol = n; //selectedBoxCol
              ss.clear();

              //player2
              if(gameWidget->numOfPlayer == 2){
                  ss << (in.readLine()).toStdString();
                  ss >> s >> n;
                  gameWidget->player2->no = n; //no
                  ss.clear();
                  ss << (in.readLine()).toStdString();
                  ss >> s >> n;
                  gameWidget->player2->x = n; //x
                  ss.clear();
                  ss << (in.readLine()).toStdString();
                  ss >> s >> n;
                  gameWidget->player2->y = n; //y
                  ss.clear();
                  ss << (in.readLine()).toStdString();
                  ss >> s >> n;
                  gameWidget->player2->score = n; //score
                  ss.clear();
                  ss << (in.readLine()).toStdString();
                  ss >> s >> n;
                  gameWidget->player2->selectedBoxRow = n; //selectedBoxRow
                  ss.clear();
                  ss << (in.readLine()).toStdString();
                  ss >> s >> n;
                  gameWidget->player2->selectedBoxCol = n; //selectedBoxCol
                  ss.clear();
              }

              long m;
              in.readLine();
              in.readLine();
              ss << (in.readLine()).toStdString();
              ss >> s >> m;
              gameWidget->alreadyUsedSeconds = m; //alreadyUsedSeconds
              ss.clear();
              ss << (in.readLine()).toStdString();
              ss >> s >> m;
              gameWidget->TotalGameTimeInSeconds = m; //TotalGameTimeInSeconds
              ss.clear();
              gameWidget->gameStartTime = clock();
              gameWidget->gameTotalPauseTime = 0;
              gameWidget->gameCurrentPauseStartTime = gameWidget->gameStartTime;

              file->close(); //关闭文件
              delete file; //释放文件进程
          }
          else
          {
              QMessageBox::information(this,"Error Box","FileOpen Error"+file->errorString());
              delete file; //释放文件进程
          }
    }
}

void Menu::quitButtonClicked()
{
    std::cout << "FUNC Menu::quitButtonClicked: step in" << std::endl;
    int choose;
    choose = QMessageBox::question(this, tr("quit QLink"),
                                   QString(tr("Are you sure to quit ?")),
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (choose == QMessageBox::No){
        // 回到原来界面
    }
    else if (choose == QMessageBox::Yes){
        gameStatus = QUIT;
        this->close();
    }
}

void Menu::paintEvent(QPaintEvent *)
{
    // 绘制游戏标题
    QPainter painter(this);
    QFont titleFont = QFont("Microsoft YaHei", 150, QFont::Black, true);
    QRect rect(25, 90, 3000, 700);
    QString str = QString("QLink");
    painter.setFont(titleFont);
    painter.drawText(rect, Qt::AlignCenter, str);
}

void Menu::decideShowMenuOrNot()
{
    if(gameStatus == MENU) this->show();
    if(gameStatus == QUIT) this->close();
}
