#include "progressbar.h"

//
// ProgressBar 创建一个环形进度条，中央有百分制的数字进度显示
// 参数 centerPosRow 表示环形进度条中心所在行的像素坐标，centerPosCol 表示环形进度条中心所在列的像素坐标
//
ProgressBar::ProgressBar(int leftSeconds, int centerPosRow, int centerPosCol, int outsideRadius, int insideRadius)
{
    this->leftSeconds = leftSeconds;
    this->centerPosRow = centerPosRow;
    this->centerPosCol = centerPosCol;
    this->insideRadius = insideRadius;
    this->outsideRadius = outsideRadius;
}

//
// paintProgressBar 绘制环形进度条，中央有百分制的数字进度显示
// 参数 percentage 表示当前进程的百分比，painter 传递paintEvent中的画笔
//
void ProgressBar::paintProgressBar(int leftSeconds, double percentage, QPainter &painter)
{
    painter.setRenderHint(QPainter::Antialiasing);

    rotateAngle = 360 * percentage / 100;

    QRectF outRect(centerPosCol - outsideRadius, centerPosRow - outsideRadius, outsideRadius * 2, outsideRadius * 2);
    QRectF inRect(centerPosCol - insideRadius, centerPosRow - insideRadius, insideRadius * 2, insideRadius * 2);
    QString valueStr = QString("%1%\n%2 s").arg(QString::number(percentage)).arg(QString::number(leftSeconds));

    //画外圆
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(97, 117, 118)));
    painter.drawEllipse(outRect);
    painter.setBrush(QBrush(QColor(255, 107, 107)));
    painter.drawPie(outRect, (90 - rotateAngle) * 16, rotateAngle * 16);
    //画遮罩
    painter.setBrush(palette().window().color());
    painter.drawEllipse(inRect);
    //画文字
    QFont f = QFont("Microsoft YaHei", 15, QFont::Bold);
    painter.setFont(f);
    painter.setPen(QColor("#555555"));
    painter.drawText(inRect, Qt::AlignCenter, valueStr);
}
