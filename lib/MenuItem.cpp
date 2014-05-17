#include "MenuItem.h"
#include <stddef.h>

MenuItem::MenuItem(void)
{
	config(0, 0, 0, 0, 0, 0, 0, true);
}

MenuItem::MenuItem(u8 symbol, u16 numItem, float LowLimit, float HighLimit, float inc, float defValue, u16 flashAddress, bool itParametr = true)
{
	config(symbol, numItem, LowLimit, HighLimit, inc, defValue, flashAddress, itParametr);
}

/**
 *Configure and init Item
 */
void MenuItem::config(u8 symbol, u16 numItem, float LowLimit, float HighLimit, float inc, float defValue, u16 flashAddress, bool itParametr)
{
	m_incr = inc;
	m_decr = inc;
	m_Parametr = itParametr;
	m_defValue = defValue;
	m_flashAddress = flashAddress;
	m_symbol = symbol;
	m_number = numItem;
	pValue = 0;
	m_hLimit = HighLimit;
	m_lLimit = LowLimit;
	m_next = NULL;
	m_prev = NULL;
	m_parent = NULL;
}

/**
 *Add Item
 */
void MenuItem::add(MenuItem* Item)
{
	Item->setParent(m_parent);
	if (m_next != NULL)
	{
		m_next->add(Item);
	}
	else
	{
		m_next = Item;
		Item->setPrev(this);
	}
}

/**
 * Set pointer to previous Item
 */
void MenuItem::setPrev(MenuItem * prevItem)
{
	m_prev = prevItem;
}

/**
 *Set pointer to next Item
 */
void MenuItem::setNext(MenuItem * nextItem)
{
	m_next = nextItem;
}

/**
 *Set pointer to parent Item
 */
void MenuItem::setParent(MenuItem * parent)
{
	m_parent = parent;
}

/**
 *Return pointer to next Item
 */
MenuItem *MenuItem::getNext()
{
	return m_next;
}

/**
 *Return pointer to previous Item
 */
MenuItem *MenuItem::getPrev()
{
	return m_prev;
}

/**
 *Return pointer to parent Item
 */
MenuItem *MenuItem::getParent()
{
	return m_parent;
}

/**
 *Return symbol of parameter
 */
u8 MenuItem::getSymbol(void)
{
	return m_symbol;
}

/**
 *Return number of parameter
 */
u16 MenuItem::getNumber(void)
{
	return m_number;
}

/**
 *Return high limit value
 */
float MenuItem::getHLim(void)
{
	return m_hLimit;
}

/**
 *Return low limit value
 */
float MenuItem::getLLim(void)
{
	return m_lLimit;
}

void MenuItem::setHLim(float NewHLim)
{
	m_hLimit = NewHLim;
}

/**
 *Return flash address
 */
u16 MenuItem::getAddr(void)
{
	return m_flashAddress;

}

/**
 *Return default value
 */
float MenuItem::getDefValue(void)
{
	return m_defValue;
}

/**
 *Return type of item value (Parametr/IO value)
 */
bool MenuItem::getType()
{
	return m_Parametr;
}


float MenuItem::getValue()
{
	return pValue;
}

void MenuItem::setValue(float Value)
{
	pValue = Value;
}


