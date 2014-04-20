#include "Unit.h"

Unit::Unit(){
	type				=	0;
	damage				=	0;
	currentHealth		=	0;
	maxHealth			=	0;
	posX				=	0.0f;
	posY				=	0.0f;
	animationTimer		=	0.0f;
	unitRect.top		=	0;
	unitRect.left		=	0;
	unitRect.right		=	0;
	unitRect.bottom		=	0;
	facingRight			=	false;
	isActive			=	false;
	unitCanTakeAction	=	false;
	isRanged			=	false;
};

void Unit::addUnit(int t, int playerNumber){
	if( playerNumber == PLAYERTWO ){
		facingRight = false;
	}
	else
		facingRight = true;
	isActive			=	true;
	unitCanTakeAction	=	true;
	isRanged			=	false;
	speed				=	0;
	whoUnitBelongsTo	=	playerNumber;
	switch(t){
	case NOUNIT:
		type				=	NOUNIT;
		damage				=	0;
		currentHealth		=	0;
		maxHealth			=	0;
		animationTimer		=	0.0f;
		unitRect.top		=	0;
		unitRect.left		=	0;
		unitRect.right		=	0;
		unitRect.bottom		=	0;
		break;
	case GOLDMINES:
		type				=	GOLDMINES;
		damage				=	0;
		currentHealth		=	140;
		maxHealth			=	140;
		animationTimer		=	0.0f;
		unitRect.top		=	0;
		unitRect.left		=	0;
		unitRect.right		=	0;
		unitRect.bottom		=	0;
		break;
	case WALL:
		break;
	case WARRIORUNIT:
		break;
	case MARKSMAN:
		break;
	case CAVALRY:
		break;
	case WOLF:
		break;
	case ARCHERUNIT:
		type				=	ARCHERUNIT;
		damage				=	25;
		currentHealth		=	100;
		maxHealth			=	100;
		animationTimer		=	0.0f;
		unitRect.top		=	15;
		unitRect.left		=	10;
		unitRect.right		=	70;
		unitRect.bottom		=	70;
		isRanged			=	true;
		break;
	case THIEF:
		break;
	case GOLEM:
		type				=	GOLEM;
		damage				=	25;
		currentHealth		=	160;
		maxHealth			=	160;
		animationTimer		=	0.0f;
		unitRect.top		=	25;
		unitRect.left		=	0;
		unitRect.right		=	50;
		unitRect.bottom		=	180;
		isRanged			=	false;
		speed				=	2;
		break;
	case BLACKMAGEUNIT:
		type				=	BLACKMAGEUNIT;
		damage				=	30;
		currentHealth		=	80;
		maxHealth			=	80;
		animationTimer		=	0.0f;
		unitRect.top		=	75;
		unitRect.left		=	5;
		unitRect.right		=	70;
		unitRect.bottom		=	145;
		isRanged			=	true;
		break;
	case WARLOCK:
		break;
	};
};

void Unit::removeUnit(){

	facingRight = false;
	isActive			=	false;
	unitCanTakeAction	=	false;
	isRanged			=	false;
	speed				=	0;
	whoUnitBelongsTo	=	-1;
	type				=	NOUNIT;
	damage				=	0;
	currentHealth		=	0;
	maxHealth			=	0;
	animationTimer		=	0.0f;
	unitRect.top		=	0;
	unitRect.left		=	0;
	unitRect.right		=	0;
	unitRect.bottom		=	0;

};

void Unit::updateAnimations( float dt ){
	if(isActive){
		switch(type){
		case NOUNIT:
			break;
		case GOLDMINES:
			break;
		case WALL:
			break;
		case WARRIORUNIT:
			break;
		case MARKSMAN:
			break;
		case CAVALRY:
			break;
		case WOLF:
			break;
		case ARCHERUNIT:
			animationTimer += dt;
			if(animationTimer > 0.15f){
				unitRect.left	+=	64;
				unitRect.right	+=	64;
				animationTimer	-=	0.15f;
			}
			if( unitRect.right > 250 ){
				setUnitRect( 15, 10, 70, 70 );
			}
			break;
		case THIEF:
			break;
		case GOLEM:
			break;
		case BLACKMAGEUNIT:
			animationTimer	+=	dt;
			if(animationTimer > 0.15f){
				animationTimer	-=	0.15f;
				switch(unitRect.left){
				case 5:
					unitRect.left	+=	70;
					unitRect.right	+=	70;
					break;
				case 75:
					unitRect.left	+=	65;
					unitRect.right	+=	65;
					break;
				case 140:
					unitRect.left	+=	65;
					unitRect.right	+=	65;
					break;
				case 205:
					setUnitRect(75, 5, 70, 145);
					break;
				}

			}
			break;
		case WARLOCK:
			break;
		}
	}
};
	
void Unit::setType( int t ){
	type	=	t;
};

int Unit::getType(){
	return type;
};

void Unit::setDamage( int d ){
	damage	=	d;
}

int Unit::getDamage(){
	return damage;
}

void Unit::setCurrentHealth( int health ){
	currentHealth	=	health;
}

int Unit::getCurrentHealth(){
	return currentHealth;
}

void Unit::adjustCurrentHealth( int health ){
	currentHealth	+=	health;
}

void Unit::setMaxHealth( int health ) {
	maxHealth	=	health;
}

int Unit::getMaxHealth(){
	return maxHealth;
}

void Unit::setWhoUnitBelongsTo( int player ){
	whoUnitBelongsTo	=	player;
}

int Unit::getWhoUnitBelongsTo(){
	return whoUnitBelongsTo;
}

void Unit::setSpeed( int s ){
	speed = s;
}

int Unit::getSpeed(){
	return speed;
}

void Unit::setPosX( float x ){
	posX	=	x;
}

void Unit::adjustPosX( float x ){
	posX += x;
}

float Unit::getPosX(){
	return posX;
}

void Unit::setPosY( float y ){
	posY	=	y;
}

void Unit::adjustPosY( float y ){
	posY	=+	y;
}

float Unit::getPosY(){
	return posY;
}

void Unit::setUnitRect(int top, int left, int right, int bottom){
	unitRect.top		=	top;
	unitRect.left		=	left;
	unitRect.right		=	right;
	unitRect.bottom		=	bottom;
}

RECT Unit::getUnitRect(){
	return unitRect;
}

void Unit::setFacingRight( bool b ){
	facingRight	=	b;
}

bool Unit::isFacingRight(){
	return facingRight;
}

void Unit::setIsActive( bool b ){
	isActive	=	b;
}

bool Unit::checkIfActive(){
	return isActive;
}

void Unit::setUnitCanTakeAction( bool b ){
	unitCanTakeAction	=	b;
}

bool Unit::canUnitTakeAction(){
	return unitCanTakeAction;
}

void Unit::setRanged( bool b ){
	isRanged = b;
};

bool Unit::checkIfRanged(){
	return isRanged;
};