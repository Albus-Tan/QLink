#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>
#include <QPainter>

class ProgressBar : public QWidget
{
    Q_OBJECT
public:
    int centerPosCol; // 圆环中心列坐标（也即x位置坐标）
    int centerPosRow; // 圆环中心行坐标（也即y位置坐标）
    int outsideRadius; // 外圆半径
    int insideRadius; // 内圆半径
    double rotateAngle; // 标志进度的半圆弧旋转的角度
    int leftSeconds; // 倒计时剩余秒数
    ProgressBar(int leftSeconds, int centerPosRow, int centerPosCol, int outsideRadius, int insideRadius);
    void paintProgressBar(int leftSeconds, double percentage, QPainter &painter);
};

#endif // PROGRESSBAR_H
