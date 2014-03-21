#pragma once
#include "DefinesAndEnums.h"
class Character{
	int characterType, playerNumber, currentHealth, maxHealth, currentSpecial, maxSpecial, gold;
	bool activePlayer;
public:
	Character();
	~Character();

	////////////////////////////////////////////////////////////////////
	//  INFO:  setCharacterType function also sets character stats
	void setCharacterType(int type);
	int  getCharacterType();

	void setPlayerNumber(int number);
	int  getPlayerNumber();

	void setCurrentHealth(int health);
	void adjustCurrentHealth(int adjustment);
	int  getCurrentHealth();

	void setMaxHealth(int health);
	int  getMaxHealth();

	void setCurrentSpecial(int special);
	void adjustCurrentSpecial(int adjustment);
	int  getCurrentSpecial();

	void setMaxSpecial(int special);
	int  getMaxSpecial();

	void setGold(int gold);
	void adjustGold(int gold);
	int  getGold();

	void setActivePlayer(bool b);
	bool checkIfActivePlayer();
};