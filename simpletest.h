#ifndef SIMPLETEST_H
#define SIMPLETEST_H

#include <QtTest/QtTest>
#include <QObject>
#include "map.h"
#include "widget.h"

class SimpleTest : public QObject
{
    Q_OBJECT
private slots:
    void case1_testcase();
    void case2_testcase();
    void case3_testcase();
    void case4_testcase();
public:
    explicit SimpleTest(QObject *parent = nullptr);

signals:

};

#endif // SIMPLETEST_H
