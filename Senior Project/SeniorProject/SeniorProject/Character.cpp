#include "Character.h"

Character::Character(){
	//////////////////////////////////////////////////////////
	//  INFO:  All character data initialized to -1.
	//			Stats will change after player chooses
	//			a character to play.
	characterType		=		-1;
	playerNumber		=		-1;
	currentHealth		=		-1;
	maxHealth			=		-1;
	currentSpecial		=		-1;
	maxSpecial			=		-1;
	gold				=		-1;
	activePlayer		=		false;
};

Character::~Character(){

};

void Character::setCharacterType(int type){
	characterType = type;		
	currentSpecial = 0;
	maxSpecial = 100;
	gold = 150;
	switch(characterType){
	case WARRIOR:
		currentHealth	=		500;
		maxHealth		=		500;
		break;
	case ARCHER:
		currentHealth	=		400;
		maxHealth		=		400;
		break;
	case BLACKMAGE:
		currentHealth	=		300;
		maxHealth		=		300;
		break;
	case WHITEMAGE:
		currentHealth	=		300;
		maxHealth		=		300;
		break;
	};
};

int Character::getCharacterType(){
	return characterType;
};

void Character::setPlayerNumber(int number){
	playerNumber = number;
};

int Character::getPlayerNumber(){
	return playerNumber;
};

void Character::setCurrentHealth(int health){
	currentHealth = health;
};

void Character::adjustCurrentHealth(int adjustment){
	currentHealth += adjustment;
	if(currentHealth > maxHealth)
		currentHealth = maxHealth;
};

int Character::getCurrentHealth(){
	return currentHealth;
};

void Character::setMaxHealth(int health){
	maxHealth = health;
};

int Character::getMaxHealth(){
	return maxHealth;
};

void Character::setCurrentSpecial(int special){
	currentSpecial = special;
};

void Character::adjustCurrentSpecial(int adjustment){
	currentSpecial += adjustment;
	if(currentSpecial > maxSpecial)
		currentSpecial = maxSpecial;
};

int Character::getCurrentSpecial(){
	return currentSpecial;
};

void Character::setMaxSpecial(int special){
	maxSpecial = special;
};

int Character::getMaxSpecial(){
	return maxSpecial;
};

void Character::setGold(int amount){
	gold = amount;
};

void Character::adjustGold(int adjustment){
	gold += adjustment;
};

int Character::getGold(){
	return gold;
};

void Character::setActivePlayer(bool b){
	activePlayer = b;
};

bool Character::checkIfActivePlayer(){
	return activePlayer;
};