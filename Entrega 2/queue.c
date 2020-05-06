#include "queue.h"
#include <stdio.h>

int queue[NUM_POS];
int size;

void initQueue()
{
    size = 0;

    for (int i = 0; i < NUM_POS; i++) queue[i] = -1;
}

void queuePush(int index)
{
    for (int i = 0; i < NUM_POS; i++)
    {
        if (queue[i] == -1)
        {
            for (int i2 = (i - 1); i2 >= 0; i2--)
            {
                queue[i2 + 1] = queue[i2];
            }
            break;
        }
    }
    queue[0] = index;
    size++;
}

int queuePop()
{
    if (size == 0) return -1;

    int res;
    for (int i = 0; i < NUM_POS; i++)
    {
        if (queue[i] == -1)
        {
            res = queue[i - 1];
            queue[i - 1] = -1;
            break;
        }
    }
    size--;
    return res;
}


int queueSize()
{
     return size;
}