#ifndef MENUENTRY_H
#define MENUENTRY_H
#include "stm32f4xx_conf.h"
#include "IO_7segment.h"

class MenuItem
{
public:

	MenuItem(void);
	MenuItem(u8 symbol, u16 numItem, float LowLimit, float HighLimit, float inc, float defValue, u16 flashAddress, bool itParametr);

	void config(u8 symbol, u16 numItem, float LowLimit, float HighLimit, float inc, float defValue, u16 flashAddress, bool itParametr);

	void add(MenuItem* Item);

	void setPrev(MenuItem* prevItem);
	void setNext(MenuItem* nextItem);
	void setParent(MenuItem* parent);

	MenuItem *getNext();
	MenuItem *getPrev();
	MenuItem *getParent();

	float getValue();
	void setValue(float Value);

	u8 getSymbol(void);
	u16 getNumber(void);

	float getHLim(void);
	void setHLim(float NewHLim);
	float getLLim(void);
	u16 getAddr(void);
	float getDefValue(void);
	bool getType();

	float m_incr; //increment value
	float m_decr; //decrement value
	float pValue;
private:


	u8 m_symbol;
	u16 m_number;
	bool m_Parametr; //if true object value don't change extarnally
	float m_defValue;
	float m_hLimit;
	float m_lLimit;

	u16 m_flashAddress;

	MenuItem* m_parent;
	MenuItem* m_next;
	MenuItem* m_prev;

};

#endif // MENUENTRY_H
