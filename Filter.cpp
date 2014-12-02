/*
 * Filter.cpp
 *
 *  Created on: Oct 21, 2014
 *      Author: temmka
 */

#include <Filter.h>

Filter::Filter (u16 imaxSize)
{
  maxSize = imaxSize;
  arr = new u16[maxSize];
  avrValue=1;

}

u16
Filter::fl (u16 Value)
{
  u32 tAvr = 0;
  for (u16 i = avrValue - 1; i > 0; i--)
    {
      arr[i] = arr[i - 1];
      tAvr += arr[i];
    }
  arr[0] = Value;
  tAvr += arr[0];

  return tAvr / avrValue;
}

void
Filter::setAvrValue (u16 Value)
{

  if (Value < maxSize && Value > 0)
    {
      avrValue = Value;

    }
  else
    {
      avrValue = 1;
    }
}
