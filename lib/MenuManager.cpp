#include "MenuManager.h"

MenuManager::MenuManager()
{
}

void MenuManager::config()
{
	for (u8 i = 0; i <= USED_BOARDS; i++)
	{
		group[i].mRootItem = NULL;
		group[i].mCurrItem = NULL;
	}

	m_pMenuLCD = new IO_7segment(4);

	Flash.spiNum = new softSpi;

	group0Last = NULL;

	PasswordSp = NULL;
	PasswordPv = NULL;
	RootItemNum = NULL;

	m_Speed = 1;

	PasswordOk = false;
	m_EditMode = false;

	pValue = 0;
	pNumber = 0;

	scrollCounter[0] = 0;
	scrollCounter[1] = 0;
	scrollCounter[2] = 0;
	pSymbol = 0;

}

/**
 * Get point position of float
 */
float MenuManager::getPointFactor(float value)
{

	if (value >= 1000) //Thousands
	{
		return 1000;
	}
	if (value >= 100 && value < 1000) //Hundreds
	{

		return 100;
	}
	if (value >= 10 && value < 100) //Tens
	{

		return 10;
	}
	if (value < 10) //Ones
	{
		return 1;
	}

	return 1;

}

/**
 * Get root item
 */
MenuItem * MenuManager::getRoot()
{
	return group[0].mRootItem;
}

/**
 * Move pointer of selected item to last item
 * of root group
 */
void MenuManager::selectGroup(u8 GroupNumber)
{
	static u8 tLast = 0;

	if (tLast != GroupNumber)
	{
		group[GroupNumber].mRootItem->setPrev(group0Last);
		group0Last->setNext(group[GroupNumber].mRootItem);

		tLast = GroupNumber;
	}

}

void MenuManager::selectRoot()
{
	group[0].mCurrItem = group[0].mRootItem;
	pNumber = group[0].mCurrItem->getNumber();
	pSymbol = group[0].mCurrItem->getSymbol();
	selectItemNum();
	m_EditMode = true;
}

void MenuManager::addItem(u8 SubMenuNumber, MenuItem * p_menuItem)
{
	group[SubMenuNumber].mCurrItem->add(p_menuItem);
}

/**
 *add last item in root menu
 */
void MenuManager::addLast(u8 SubMenuNumber, MenuItem * p_menuItem)
{
	addItem(SubMenuNumber, p_menuItem);
	group0Last = p_menuItem;
}

/**
 * add root item (first in group)
 */
void MenuManager::addRoot(u8 SubMenuNumber, MenuItem * p_menuEntry)
{
	group[SubMenuNumber].mRootItem = p_menuEntry;
	group[SubMenuNumber].mCurrItem = p_menuEntry;
}

void MenuManager::Up(bool input)
{
	static bool fpUp = false;

	delayScroll(input, fpUp);
	if (input == true)
	{
		if (!fpUp || (m_Speed > 1))
		{

			if (!m_EditMode)
			{

				//Check password
				if ((group[0].mCurrItem == PasswordPv && PasswordOk) || (PasswordSp->getValue() == 0) || group[0].mCurrItem != PasswordPv)
				{
					MenuItem *next = group[0].mCurrItem->getNext();

					if (next != NULL)
					{
						group[0].mCurrItem = next;
					}

					pNumber = group[0].mCurrItem->getNumber();
					pSymbol = group[0].mCurrItem->getSymbol();
				}
			}

			//Increase
			else
			{
				if (group[0].mCurrItem->getType() == true)
				{
					float x = getPointFactor(pValue);

					if (pValue < group[0].mCurrItem->getHLim())
					{

						if (scrollCounter[0] == 1)
						{
							if (group[0].mCurrItem->m_inc == 0) //default increment 0.001
							{
								pValue += 0.001 * x * m_Speed;
							}
							else
							{
								pValue += group[0].mCurrItem->m_inc * m_Speed;
							}
						}

					}
					else
					{
						pValue = group[0].mCurrItem->getHLim();
					}
				}

			}

		} //(!fpUp || (m_Speed > 1))

		fpUp = true;

	} //(input == true)

}

void MenuManager::Down(bool input)
{
	static bool fpDwn = false;

	delayScroll(input, fpDwn);

	if (input == true)
	{
		if (!fpDwn || (m_Speed > 1))
		{
			if (!m_EditMode)
			{
				MenuItem *prev = group[0].mCurrItem->getPrev();

				if (prev != NULL)
				{
					group[0].mCurrItem = prev;
				}

				//Select root menu
				if (prev == group[0].mRootItem) // CREATE comment
				{
					m_EditMode = true;
				}

				pNumber = group[0].mCurrItem->getNumber();
				pSymbol = group[0].mCurrItem->getSymbol();
			}

			//Decrease value
			else
			{
				//Case item with IO type
				if (group[0].mCurrItem->getType() == true)
				{
					float x = getPointFactor(pValue);

					if (pValue > group[0].mCurrItem->getLLim())
					{

						if (scrollCounter[0] == 1)
						{
							if (group[0].mCurrItem->m_inc == 0)
							{
								pValue -= 0.001 * x * m_Speed;
							}
							else
							{
								pValue -= group[0].mCurrItem->m_inc * m_Speed;
							}
						}

					}
					else
					{
						pValue = group[0].mCurrItem->getLLim();
					}

				}
			}

		} //(!fpUp || (m_Speed > 1))

		fpDwn = true;

	} //(input == true)
}

/**
 * step increase of speed scroll (up/down)
 */
void MenuManager::delayScroll(bool input, bool &fp)
{
	if (input == true)
	{
		scrollCounter[0]++;

		if (scrollCounter[0] > 900)
		{
			scrollCounter[1]++;
			scrollCounter[0] = 0;
		}

		//Speed 1
		if (scrollCounter[1] > 5)
		{
			m_Speed = 2;
		}
		//Speed 2
		if (scrollCounter[1] > 25)
		{
			m_Speed = 10;
		}
		//Speed 3
		if (scrollCounter[1] > 50)
		{
			m_Speed = 50;
		}
		//Speed 4
		if (scrollCounter[1] > 75)
		{
			m_Speed = 100;
		}
		//Speed 5
		if (scrollCounter[1] > 100)
		{
			m_Speed = 500;
		}

	}
	else
	{

		//Reset counters
		if (fp == true)
		{
			fp = false;
			scrollCounter[0] = 0;
			scrollCounter[1] = 0;
			m_Speed = 1;
		}
	}
}

void MenuManager::delaySelect(bool input, bool &fp)
{
	if (input)
	{
		scrollCounter[0]++;

		if (scrollCounter[0] > 500)
		{
			scrollCounter[1]++;
			scrollCounter[0] = 0;
		}
	}
	else
	{
		if (fp)
		{
			fp = false;
			scrollCounter[0] = 0;
			scrollCounter[1] = 0;
		}
	}

}

/**
 * Select button
 */
void MenuManager::Select(bool input)
{
	static bool fpSelect = false;

	delaySelect(input, fpSelect);

	if (input == true)
	{
		//signal with delay ok
		if (!fpSelect && (scrollCounter[1] > 5))
		{
			if (!m_EditMode)
			{
				m_EditMode = true;
				pValue = group[0].mCurrItem->getValue();
			}
			else
			{
				m_EditMode = false;
				pNumber = group[0].mCurrItem->getNumber();
				pSymbol = group[0].mCurrItem->getSymbol();

				//write change item to flash if change
				if (pValue != group[0].mCurrItem->getValue() && group[0].mCurrItem->getAddr() != 0)
				{
					Flash.writeFloat(group[0].mCurrItem->getAddr(), pValue);
				}

				group[0].mCurrItem->setValue(pValue);

				//Password check
				if (PasswordPv->getValue() == PasswordSp->getValue())
				{
					PasswordOk = true;
					PasswordPv->setValue(0);
					pValue = 0;
				}

				//Enter the menu
				if (group[0].mCurrItem == group[0].mRootItem) // CREATE comment
				{
					Up(true);
				}

				//Enter the number of output item
				if (group[0].mCurrItem == group[0].mRootItem->getNext()) // CREATE comment
				{
					selectItemNum();
				}

			}

			fpSelect = true;
		}

	}
	else
	{
		//Output the non-flash item
		if (m_EditMode == true && group[0].mCurrItem->getType() == false)
		{
			group[0].mRootItem->setValue(RootItemNum->getValue());
			pValue = group[0].mCurrItem->getValue();
		}

	}

}

/**
 * read saved to flash value of item  on startup
 */
void MenuManager::readFlash(void)
{
	//Increment group type
	for (u8 i = 0; i <= USED_BOARDS; i++)
	{
		//Select root item of board
		MenuItem *next = group[i].mRootItem;

		//Increment item number from root to last
		while (next != NULL)
		{
			if (next->getAddr() != 0)
			{
				next->setValue(Flash.readFloat(next->getAddr()));
			}
			next = next->getNext();
		}
	}
}

/**
 * Set all item to default value
 */
void MenuManager::setDefaultValue(float input)
{
	if (input)
	{
		//Increment board type
		for (u8 i = 0; i <= USED_BOARDS; i++)
		{
			//Select root item of board
			MenuItem *next = group[i].mRootItem;

			//Increment item number from root to last
			while (next != NULL)
			{
				next->setValue(next->getDefValue());
				if (next->getType() && next->getAddr() != 0)
				{
					Flash.writeFloat(next->getAddr(), next->getDefValue());
				}
				next = next->getNext();
			}
		}
		selectRoot();
	}
}

/**
 *Display data
 */
void MenuManager::Display()
{

	if (!m_EditMode)
	{

		m_pMenuLCD->write(pSymbol, pNumber);
	}
	else
	{

		m_pMenuLCD->write(pValue);
	}
}

/**
 * Reset current menu view and password if button non use
 */
void MenuManager::TimerReset(bool input)
{

	static bool p = 0;
	static u32 ResetTime = 0;

	if (!input == true) //input signal on
	{
		if (p == false) //first cycle
		{
			ResetTime = RTC->TR + MENU_RESET_TIME; //set on-value
			p = true;

		} //first cycle
		else //other cycle
		{
			//Reset password and move to root menu
			if (group[0].mCurrItem != group[0].mRootItem || PasswordOk == true)
			{
				if (RTC->TR >= ResetTime)
				{
					PasswordOk = false;
					selectRoot();
				}
			}

		} //other cycle

	} //input signal on
	else
	{
		p = false;
	}

}

/**
 * search item by selected number and move to RootItemNum
 */
void MenuManager::selectItemNum(void)
{

	MenuItem *next = group0Last->getNext();

	//Get number of selected output Item
	MenuItem * SelectNumItem = group[0].mRootItem->getNext();
	u16 cnt = 0;

	//Get max num of item in selected group
	while (next != NULL)
	{
		cnt++;
		next = next->getNext();
	}

	//reinstal Hlim
	SelectNumItem->setHLim((float) cnt);

	next = group0Last->getNext();

	//Search item by number
	for (u16 i = 1; i <= cnt; i++)
	{
		if (i == SelectNumItem->getValue())
		{
			RootItemNum = next;
			break;
		}

		next = next->getNext();

	}

}

void MenuManager::changeItem(MenuItem &Trg, MenuItem &Dst, MenuItem Src)
{

	if (Trg.getValue())
	{

		Dst.pValue = Src.pValue;

		Trg.setValue(0.0);
		Flash.writeFloat(Trg.getAddr(), pValue);
		Flash.writeFloat(Dst.getAddr(), pValue);

	}
}
void MenuManager::changeItem(MenuItem &Dst, MenuItem Src)
{

	if (Dst.getValue() == 0.0)
	{
		Dst.pValue = Src.pValue;
		Flash.writeFloat(Dst.getAddr(), pValue);
	}

}
