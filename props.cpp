#include "props.h"

Props::Props(int type, int row, int col, int appearTimeInSeconds, int maxAppearTimes)
{
    this->type = type;
    this->row = row;
    this->col = col;
    this->appearTimeInSeconds = appearTimeInSeconds;
    this->maxAppearTimes = maxAppearTimes;
    create = clock();
}

