#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "stm32f4xx_conf.h"
#include "user_conf.h"



/*Structure of menu:            Used method:
				   RootItem     //Up/Increase
					  |         //Down/Decrease
				    Item C1     //Select/Exit
					  |
				    Item C2
					  |
					 ...
					  |
				    Item C7
					  |
				   Selector     - switch submenu
				 /	  |    \
Item A1   Item B1   Item C1   Item X1 ... Item N1
    |        |        |          |          |
Item A2   Item B2   Item C2   Item X2 ... Item N2
    |        |        |          |          |
   ...      ...      ...        ...        ...
    |        |        |          |          |
Item An   Item Bn   Item Cn   Item Xn ... Item Nn

*/


class MenuManager
{
public:



	MenuItem * PasswordSp;  //memory value pass
	MenuItem * PasswordPv;  //input value pass
	MenuItem * RootItemNum; //selected subitem for display in root
	MenuManager();
	void config();

	void addItem(u8 SubMenuNumber, MenuItem * p_menuItem);
	void addRoot(u8 SubMenuNumber, MenuItem * p_menuItem);
	void addLast(u8 SubMenuNumber, MenuItem * p_menuItem);
	void selectItemNum();
	void selectRoot();

	void selectGroup(u8 SubMenuNumber);
	MenuItem * getRoot();


	//Navigation methods
	void Up(bool input);
	void Down(bool input);
	void Select(bool input);
	void Display();

	//Auxiliary methods
	void readFlash(void);
	void setDefaultValue(float input);
	void delaySelect(bool input, bool &fp);
	void delayScroll(bool input, bool &fp);
	void TimerReset(bool input);
	float getPointFactor(float value);

	void changeItem(MenuItem &Trg , MenuItem &Dst, MenuItem Src);
	void changeItem(MenuItem &Dst, MenuItem Src);
	IO_7segment* m_pMenuLCD;        //Output driver pointer
	eeprom Flash;

private:

	struct grMenu
	{
		MenuItem* mRootItem;        //Root item pointer
		MenuItem* mCurrItem;        //Current item pointer
	};

	struct grMenu group[USED_BOARDS + 1];



	MenuItem * group0Last;          //last item in root group[0]

	//Displayed value
	u16 pNumber;                    //Selected parametr number
	float pValue;                   //Selected parametr value
	u8 pSymbol;                     //Selected parametr symbol

	bool m_EditMode;                //Edit mode
	bool PasswordOk;                //pasword correct or non use

	u32 scrollCounter[3];           //counter for scroll data
	float m_Speed = 0;              //Scroll speed

};
#endif // MENUMANAGE_H
