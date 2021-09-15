#ifndef GLOBAL_H
#define GLOBAL_H

// ------------ 全局变量 ------------ //
//记录游戏状态
enum GameStatus
{
    PLAYING, //游戏进行中
    PAUSE, //暂停
    WIN, //单人模式因将游戏页面方块全部消除完而结束
    OVER, //单人模式因游戏时间到而结束，双人模式正常游戏结束（方块消除完或时间到）
    UNSOLVABLE, //因地图无解而结束
    MENU, //菜单页
    QUIT //即将退出
};

extern GameStatus gameStatus;

// -------------------------------- //

#endif // GLOBAL_H
