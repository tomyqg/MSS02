/*
 * Filter.h
 *
 *  Created on: Oct 21, 2014
 *      Author: temmka
 */

#ifndef FILTER_H_
#define FILTER_H_
#include "stm32f4xx_conf.h"

class Filter
    {
public:
    Filter(u16 maxSize);

    u16 fl(u16 Value);
    void setAvrValue(u16 Value);
    u16 *arr;

private:
    u16 avrValue;
    u16 maxSize;
    };

#endif /* FILTER_H_ */
