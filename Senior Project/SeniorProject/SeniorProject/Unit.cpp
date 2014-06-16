#include "Unit.h"

Unit::Unit()
{
	type							=	0;
	damage					=	0;
	currentHealth			=	0;
	maxHealth				=	0;
	state						=	IDLE;
	posX						=	0.0f;
	posY							=	0.0f;
	animationTimer			=	0.0f;
	unitRect.top				=	0;
	unitRect.left				=	0;
	unitRect.right			=	0;
	unitRect.bottom		=	0;
	facingRight				=	false;
	isActive					=	false;
	unitCanTakeAction	=	false;
	isRanged					=	false;
};

void Unit::addUnit(int t, int playerNumber)
{
	if( playerNumber == PLAYERTWO )
	{
		facingRight = false;
	}
	else
		facingRight = true;

	isActive			=	true;
	unitCanTakeAction	=	true;
	isRanged			=	false;
	speed				=	0;
	state				=	IDLE;
	whoUnitBelongsTo	=	playerNumber;
	unitCanUseAbility	=	false;
	switch(t)
	{
	case NOUNIT:
		type						=	NOUNIT;
		damage				=	0;
		currentHealth		=	0;
		maxHealth			=	0;
		animationTimer		=	0.0f;
		unitRect.top			=	0;
		unitRect.left			=	0;
		unitRect.right		=	0;
		unitRect.bottom	=	0;
		break;
	case GOLDMINES:
		type							=	GOLDMINES;
		damage					=	0;
		currentHealth			=	140;
		maxHealth				=	140;
		animationTimer			=	0.0f;
		unitRect.top				=	0;
		unitRect.left				=	0;
		unitRect.right			=	0;
		unitRect.bottom		=	0;
		unitCanTakeAction	=	false;
		break;
	case WALL:
		type						=	WALL;
		damage				=	0;
		currentHealth		=	200;
		maxHealth			=	200;
		animationTimer		=	0.0f;
		unitRect.top			=	0;
		unitRect.left			=	0;
		unitRect.right		=	0;
		unitRect.bottom	=	0;
		occupied				=	true;
		break;
	case WARRIORUNIT:
		type				=	WARRIORUNIT;
		damage				=	50;
		currentHealth		=	170;
		maxHealth			=	170;
		animationTimer		=	0.0f;
		unitRect.top		=	0;
		unitRect.left		=	0;
		unitRect.right		=	70;
		unitRect.bottom		=	85;
		isRanged				=	false;
		speed					=	2;
		occupied				=	true;
		break;
	case MARKSMAN:
		type				=	MARKSMAN;
		damage				=	30;
		currentHealth		=	120;
		maxHealth			=	120;
		animationTimer		=	0.0f;
		unitRect.top		=	0;
		unitRect.left		=	0;
		unitRect.right		=	45;
		unitRect.bottom		=	70;
		isRanged			=	true;
		occupied			=	true;
		break;
	case MINOTAUR:
		type						=	MINOTAUR;
		damage				=	70;
		currentHealth		=	200;
		maxHealth			=	200;
		animationTimer		=	0.0f;
		unitRect.top			=	0;
		unitRect.left			=	215;
		unitRect.right		=	355;
		unitRect.bottom	=	180;
		isRanged				=	false;
		speed					=	4;
		occupied				=	true;
		unitCanUseAbility		=	true;
		break;
	case WOLF:
		type						=	WOLF;
		damage				=	50;
		currentHealth		=	120;
		maxHealth			=	120;
		animationTimer		=	0.0f;
		unitRect.top			=	690;
		unitRect.left			=	35;
		unitRect.right		=	160;
		unitRect.bottom	=	800;
		isRanged				=	false;
		speed					=	3;
		occupied				=	true;
		break;
	case ARCHERUNIT:
		type						=	ARCHERUNIT;
		damage						=	25;
		currentHealth				=	100;
		maxHealth					=	100;
		animationTimer				=	0.0f;
		unitRect.top				=	15;
		unitRect.left				=	10;
		unitRect.right				=	70;
		unitRect.bottom				=	70;
		isRanged					=	true;
		occupied					=	true;
		break;
	case THIEF:
		type						=	THIEF;
		damage				=	50;
		currentHealth		=	120;
		maxHealth			=	120;
		animationTimer		=	0.0f;
		unitRect.top			=	0;
		unitRect.left			=	0;
		unitRect.right		=	35;
		unitRect.bottom	=	50;
		isRanged				=	false;
		unitCanUseAbility	=	true;
		speed					=	3;
		occupied				= true;
		break;
	case GOLEM:
		type						=	GOLEM;
		damage				=	40;
		currentHealth		=	160;
		maxHealth			=	160;
		animationTimer		=	0.0f;
		unitRect.top			=	25;
		unitRect.left			=	0;
		unitRect.right		=	50;
		unitRect.bottom	=	180;
		isRanged				=	false;
		speed					=	2;
		occupied				= true;
		break;
	case BLACKMAGEUNIT:
		type						=	BLACKMAGEUNIT;
		damage				=	30;
		currentHealth		=	80;
		maxHealth			=	80;
		animationTimer		=	0.0f;
		unitRect.top			=	75;
		unitRect.left			=	5;
		unitRect.right		=	70;
		unitRect.bottom	=	145;
		isRanged				=	true;
		occupied				= true;
		break;
	case WARLOCK:
		type					=	WARLOCK;
		damage					=	40;
		currentHealth			=	140;
		maxHealth				=	140;
		animationTimer			=	0.0f;
		unitRect.top			=	55;

		//unitRect.top			=	135;

		unitRect.left			=	5;//og

		//	unitRect.left			=	70;
		unitRect.right			=	32;//og
		//	unitRect.right			=	97;
		unitRect.bottom			=	95;

		//	unitRect.bottom			=	175;

		isRanged				=	true;
		unitCanUseAbility		=	true;
		occupied				=	true;
		break;
	};
};

void Unit::removeUnit()
{
	facingRight				= false;
	isActive					=	false;
	unitCanTakeAction	=	false;
	isRanged					=	false;
	unitCanUseAbility		=	false;
	speed						=	0;
	whoUnitBelongsTo	=	-1;
	type							=	NOUNIT;
	damage					=	0;
	currentHealth			=	0;
	maxHealth				=	0;
	animationTimer			=	0.0f;
	unitRect.top				=	0;
	unitRect.left				=	0;
	unitRect.right			=	0;
	unitRect.bottom		=	0;
	state						=	IDLE;
	occupied					= false;
};

void Unit::swapUnit( Unit copy ){
	type			=		copy.type;
	damage			=		copy.damage;
	currentHealth	=		copy.currentHealth;
	maxHealth		=		copy.maxHealth;
	whoUnitBelongsTo	=		copy.whoUnitBelongsTo;
	speed			=		copy.speed;
	state			=		copy.state;
	posX			=		copy.posX;
	posY			=		copy.posY;
	animationTimer	=		copy.animationTimer;
	unitRect.top	=		copy.unitRect.top;
	unitRect.left	=		copy.unitRect.left;
	unitRect.right	=		copy.unitRect.right;
	unitRect.bottom	=		copy.unitRect.bottom;

	facingRight		=		copy.facingRight;
	isActive		=		copy.isActive;
	unitCanTakeAction	=	false;//copy.unitCanTakeAction;
	isRanged			=	copy.isRanged;
	unitCanUseAbility	=	copy.unitCanUseAbility;
	occupied			=	copy.occupied;
};

void Unit::updateAnimations( float dt )
{
	if(isActive)
	{
		animationTimer += dt;
		switch(type)
		{
		case NOUNIT:
			break;
		case GOLDMINES:
			break;
		case WALL:
			break;
		case WARRIORUNIT:{
			switch( state )
			{
			case IDLE:
				if( animationTimer > 0.08f )
				{
					animationTimer -= 0.08f;
					switch(unitRect.left)
					{
					case 0:
						setUnitRect( 0, 98, 167, 85 );
						break;
					case 98:
						setUnitRect(0, 196, 265, 85);
						break;
					case 196:
						setUnitRect(0, 294, 362, 85);
						break;
					case 294:
						setUnitRect(0, 392, 459, 85);
						break;
					case 392:
						setUnitRect( 0, 490, 556, 85 );
						break;
					case 490:
						setUnitRect(0, 588, 653, 85);
						break;
					case 588:
						setUnitRect(0, 686, 750, 85);
						break;
						//	case 686:
						//	setUnitRect(0, 784, 847, 85);
						//	break;
					case 686:
						setUnitRect(0, 0, 70, 85);
						break;
					}
				}
				break;
			case MOVING:{
				if( animationTimer > 1.10f )
				{
					animationTimer -= 1.10f;
					setState( IDLE );
					//switch(unitRect.left)
					//{
					//}
				}
						}
						break;
			case ATTACKING:
								if( animationTimer > 1.10f )
				{
					animationTimer -= 1.10f;
					setState( IDLE );
					//switch(unitRect.left)
					//{
					//}
				}
				   
				break;
			case HIT:
							if( animationTimer > 1.10f )
			{
				animationTimer -= 1.10f;
				setState( IDLE );
				//switch(unitRect.left)
				//{
				//}
			}
				   
				break;

			}
						 }
						 break;
		case MARKSMAN:{
			switch( state )
			{
			case IDLE:
				if( animationTimer > 0.12f )
				{
					animationTimer -= 0.12f;
					switch(unitRect.left)
					{
					case 0:
						setUnitRect( 0, 44, 88, 70 );
						break;
					case 44:
						setUnitRect(0, 88, 131, 70 );
						break;
					case 88:
						setUnitRect(0, 132, 174, 70 );
						break;
					case 132:
						setUnitRect(0, 0, 45, 70 );
						break;
					}
				}
				break;
			case HIT:
							if( animationTimer > 1.10f )
			{
				animationTimer -= 1.10f;
				setState( IDLE );
				//switch(unitRect.left)
				//{
				//}
			}
				   
				break;
			}
					  }
					  break;
		case MINOTAUR:{
			switch( state )
			{
			case IDLE:
				if( animationTimer > 0.10f )
				{
					animationTimer -= 0.10f;
					switch(unitRect.left)
					{
					case 215:
						setUnitRect( 0, 355, 494, 180 );
						break;
					case 355:
						setUnitRect(0, 495, 636, 180 );
						break;
					case 495:
						setUnitRect(0, 635, 778, 180 );

						break;
					case 635:
						setUnitRect(0, 774, 920, 180 );
						//setUnitRect(0, 215, 352, 180 );
						break;
					case 774:
						setUnitRect(0, 215, 352, 180 );
						break;
					}
				}
			case ATTACKING:{
				if( animationTimer > 1.50f )
				{
					animationTimer -= 1.50f;
					setState( IDLE );
					//switch(unitRect.left)
					//{
					//}
				}
						   }
						   break;
			case HIT:
				if( animationTimer > 1.10f )
				{
					animationTimer -= 1.10f;
					setState( IDLE );
					//switch(unitRect.left)
					//{
					//}
				}

				break;
				//break;
			}
					  }
					  break;
		case WOLF:{
			switch( state ){
			case IDLE:
				if( animationTimer > 0.08f )
				{
					animationTimer -= 0.08f;
					switch(unitRect.left)
					{
					case 35:
						adjustUnitRectLeftRight( 122, 121 );
						break;
					case 157:
						adjustUnitRectLeftRight( 127, 125 );
						break;
					case 284:
						adjustUnitRectLeftRight( 137, 130 );
						break;
					case 421:
						adjustUnitRectLeftRight( 138, 135 );
						break;
					case 559:
						adjustUnitRectLeftRight( 125, 130 );
						break;
					case 684:
						unitRect.top			=	690;
						unitRect.left			=	35;
						unitRect.right		=	160;
						unitRect.bottom	=	800;
						break;
					}
				}
				break;
			case MOVING:
			if( animationTimer > 0.11f )
				{
					animationTimer -= 0.11f;
					switch(unitRect.left)
					{
					case 35:
						adjustUnitRectLeftRight( 160, 160 );
						break;
					case 195:
						adjustUnitRectLeftRight( 160, 175 );
						break;
					case 355:
						adjustUnitRectLeftRight( 180, 190 );
						break;
					case 535:
						adjustUnitRectLeftRight( 190, 170 );
						break;
					case 725:
						setUnitRect( 160, 35, 180, 260 );
						break;
					}
				}
				break;
			case ATTACKING:
				if( animationTimer > 0.11f )
				{
					animationTimer -= 0.11f;
					switch(unitRect.left)
					{
					case 35:
						adjustUnitRectLeftRight( 160, 160 );
						break;
					case 195:
						adjustUnitRectLeftRight( 160, 175 );
						break;
					case 355:
						adjustUnitRectLeftRight( 180, 190 );
						break;
					case 535:
						adjustUnitRectLeftRight( 190, 170 );
						break;
					case 725:
						setState( IDLE );//setUnitRect( 160, 35, 180, 260 );
						break;
					}
				}
			/*	if( animationTimer > 1.0f )
					setState( IDLE );
						   }*/
				break;
			}
				  }
				  break;
		case ARCHERUNIT:{
			switch(state)
			{
			case IDLE:
				if(animationTimer > 0.15f)
				{
					unitRect.left	+=	64;
					unitRect.right	+=	64;
					animationTimer	-=	0.15f;
				}

				if( unitRect.right > 250 )
					setUnitRect( 15, 10, 70, 70 );

				break;
			case ATTACKING:
			if( animationTimer > 0.12f )
			{
				animationTimer -= 0.12f;
				switch(unitRect.left){
				case 15:
					adjustUnitRectLeftRight( 65, 85 );
					break;
				case 80:
					adjustUnitRectLeftRight( 94, 95 );
					break;
				case 174:
					adjustUnitRectLeftRight( 93, 75 );
					break;
				case 267:
					setState( IDLE );
					break;

				}
			}
				break;
			case HIT:
			if( animationTimer > 0.15f )
			{
				animationTimer -= 0.15f;
				//setState( IDLE );
				switch(unitRect.left){
				case 340:
					adjustUnitRectLeftRight( 100, 90 );
					//adjustUnitRectLeftRight( 80, 80 );
					break;
				//case 420:
					//adjustUnitRectLeftRight( 100, 100 );
				case 440:
					adjustUnitRectLeftRight( 100, 90 );
					break;
				//case 520:
				case 540:
					adjustUnitRectLeftRight( 100,  90 );
					break;
				case 640:
					setState( IDLE );
					break;

				}
			}
				   break;
			}
						}
						break;
		case THIEF:{
			switch( state )
			{
			case IDLE:
				if( animationTimer > 0.25f )
				{
					animationTimer -= 0.25f;
					switch(unitRect.left)
					{
					case 0:
						setUnitRect( 0, 32, 65, 50 );
						break;
					case 32:
						setUnitRect(0, 0, 35, 50);
						break;
					}
				}
				break;
			case ABILITY:
				if(animationTimer > 0.12f){
					animationTimer -= 0.12f;
					switch(unitRect.left){
					case 0:
						setUnitRect(0, 32, 65, 50);
						break;
					case 32:
						setUnitRect(0, 70, 105, 50);
						break;
					case 70:
						setUnitRect(0, 110, 145, 50);
						break;
					case 110:
						setUnitRect( 0, 150, 185, 50 );
						break;
					case 150:
						setUnitRect( 0, 180, 215, 50 );
						break;
					case 180:
						setUnitRect( 0, 210, 245, 50 );
						break;
					case 210:
						setUnitRect(0, 0, 35, 50);
						state = IDLE;
						break;
					}
				}
				break;
			case MOVING:
				if(animationTimer > 0.2f){
					animationTimer -= 0.2f;
					switch(unitRect.left){
					case 110:
						setUnitRect( 0, 150, 185, 50 );
						break;
					case 150:
						setUnitRect( 0, 180, 215, 50 );
						break;
					case 180:
						setUnitRect(0, 110, 145, 50);
						break;
					}
				}
				break;
			case ATTACKING:
				if(animationTimer > 0.06f){
					animationTimer -= 0.06f;
					switch(unitRect.left){
					case 0:
						adjustUnitRectLeftRight( 32, 30 );
						//setUnitRect(0, 32, 65, 50);
						break;
					case 32:
						adjustUnitRectLeftRight( 36, 42 );
						//setUnitRect(0, 70, 105, 50);
						break;
					case 68:
						adjustUnitRectLeftRight( 40, 40 );
						//setUnitRect(0, 110, 145, 50);
						break;
					case 108:
						adjustUnitRectLeftRight( 40, 40 );
						//setUnitRect( 0, 150, 185, 50 );
						break;
					case 148:
						adjustUnitRectLeftRight( 30, 30 );
						//setUnitRect( 0, 180, 215, 50 );
						break;
					case 178:
						adjustUnitRectLeftRight( 32, 32 );
						//setUnitRect( 0, 210, 245, 50 );
						break;
					case 210:
						adjustUnitRectLeftRight( 34, 34 );
						//unitRect.left	+=	30;
						//unitRect.right	+=	30;
						break;
					case 244:
						adjustUnitRectLeftRight( 32, 32 );
						//unitRect.left	+=	30;
						//unitRect.right	+=	30;
						break;
					case 276:
						adjustUnitRectLeftRight( 32, 32 );
						//unitRect.left	+=	30;
						//unitRect.right	+=	30;
						break;
					case 308:
						adjustUnitRectLeftRight( 32, 32 );
						//unitRect.top			=	0;
						//unitRect.left			=	0;
						//unitRect.right		=	35;
						//unitRect.bottom	=	50;
						//state	=	IDLE;
						break;
					case 340:
						adjustUnitRectLeftRight( 34, 38 );
						break;
					case 374:
						adjustUnitRectLeftRight( 45, 45 );
						break;
					case 419:
						adjustUnitRectLeftRight( 45, 45 );
						break;
					case 464:
						setState( IDLE );
						break;
					}
				}
				break;
			case HIT:
							if( animationTimer > 1.10f )
			{
				animationTimer -= 1.10f;
				setState( IDLE );
				//switch(unitRect.left)
				//{
				//}
			}
				   break;
			}
				   }
				   break;
		case GOLEM:{
			if( animationTimer > 1.10f )
			{
				animationTimer -= 1.10f;
				setState( IDLE );
				//switch(unitRect.left)
				//{
				//}
			}
				   }
				   break;
		case BLACKMAGEUNIT:{
			switch( state ) {
			case IDLE:
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
					break;
				}
			case HIT:
				if( animationTimer > 1.10f )
				{
					animationTimer -= 1.10f;
					setState( IDLE );
					//switch(unitRect.left)
					//{
					//}
				}

				break;
			}
						   }
						   break;
		case WARLOCK:{
			switch( state )
			{
			case IDLE:
				if( animationTimer > 0.2f )
				{
					animationTimer -= 0.2f;
					switch(unitRect.left)
					{
					case 5:
						setUnitRect( 55, 32, 59, 95 );
						break;
					case 32:
						setUnitRect(55, 5, 32, 95 );
						break;
					}
				}
				break;
			case ABILITY:
				if( animationTimer > 0.16f )
				{
					animationTimer -= 0.16f;
					switch(unitRect.left)
					{
					case 5:
						setUnitRect( 95, 32, 59, 135 );
						break;
					case 32:
						setUnitRect( 95, 59, 86, 135 );
						break;
					case 59:
						setUnitRect( 95, 86, 113, 135 );
						break;
					}
				}
				break;
			case ATTACKING:
				if( animationTimer > 0.05f )
				{
					animationTimer -= 0.05f;
					switch(unitRect.left)
					{
					case 5:
						setUnitRect( 95, 32, 59, 135 );
						break;
					case 32:
						setUnitRect( 95, 59, 86, 135 );
						break;
					case 59:
						setUnitRect( 95, 86, 113, 135 );
						break;
					case 86:
						setUnitRect( 95, 113, 140, 135 );
						break;
					}
				}
				break;
			case HIT:
				if( animationTimer > 0.12f )
				{
					animationTimer -= 0.12f;
					switch(unitRect.left)
					{
					case 70:
						setUnitRect( 135, 98, 125, 175 );
						break;
					case 98:
						setUnitRect( 135, 126, 153, 175 );
						break;
					case 126:
						setUnitRect( 135, 154, 181, 175 );
						break;
					case 154:
						setUnitRect( 135, 181, 208, 175 );
						break;
					case 181:
						setState( IDLE );//setUnitRect( 135, 70, 97, 175 );
						break;
					}
				}
				break;
			}
			break;
					 }
		}
	}
};

void Unit::setType( int t )
{
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

int Unit::getCurrentHealth()
{
	return currentHealth;
}

void Unit::adjustCurrentHealth( int health )
{
	currentHealth	+=	health;
}

void Unit::setMaxHealth( int health ) 
{
	maxHealth	=	health;
}

int Unit::getMaxHealth()
{
	return maxHealth;
}

void Unit::setWhoUnitBelongsTo( int player )
{
	whoUnitBelongsTo	=	player;
}

int Unit::getWhoUnitBelongsTo()
{
	return whoUnitBelongsTo;
}

void Unit::setSpeed( int s )
{
	speed = s;
}

int Unit::getSpeed()
{
	return speed;
}

void Unit::setState( int s )
{
	state = s;
	animationTimer		=	0.0f;
	switch( type ){
	case WARRIORUNIT:
		break;
	case MARKSMAN:
		break;
	case MINOTAUR:
		break;
	case WOLF:
		switch( state ){
		case IDLE:
					animationTimer		=	0.0f;
		unitRect.top			=	690;
		unitRect.left			=	35;
		unitRect.right		=	160;
		unitRect.bottom	=	800;
		break;
		case MOVING:
			setUnitRect( 160, 35, 180, 260 );
			break;
		case ATTACKING:
			setUnitRect( 160, 35, 180, 260 );
			break;
		}
		break;
	case ARCHERUNIT:
		switch( state ){
		case IDLE:
			unitRect.top				=	15;
			unitRect.left				=	10;
			unitRect.right				=	70;
			unitRect.bottom				=	70;
			break;
		case ATTACKING:
			setUnitRect( 135, 15, 80, 190 );
			break;
		case HIT:
			setUnitRect( 15, 340, 420, 70 );
			break;
		}
		break;
	case THIEF:
		switch( state ){
		case IDLE:
			unitRect.top			=	0;
			unitRect.left			=	0;
			unitRect.right			=	35;
			unitRect.bottom			=	50;
			break;
		case MOVING:
			setUnitRect(0, 110, 145, 50);
			break;
		case ATTACKING:
			unitRect.top			=	0;
			unitRect.left			=	0;
			unitRect.right			=	35;
			unitRect.bottom			=	50;
			break;
		}
		break;
	case GOLEM:
		break;
	case BLACKMAGEUNIT:
		break;
	case WARLOCK:
		switch( state ){
		case IDLE:
			unitRect.top			=	55;
			unitRect.left			=	5;
			unitRect.right			=	32;
			unitRect.bottom			=	95;
			break;
		case ABILITY:
			unitRect.top			=	95;
			unitRect.left			=	5;
			unitRect.right			=	32;
			unitRect.bottom			=	135;
			break;
		case ATTACKING:
			unitRect.top			=	95;
			unitRect.left			=	5;
			unitRect.right			=	32;
			unitRect.bottom			=	135;
			break;
		case HIT:
			unitRect.top			=	135;
			unitRect.left			=	70;
			unitRect.right			=	97;
			unitRect.bottom			=	175;
			break;
		}
		break;
	}
}

int Unit::getState(){
	return state;
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

void Unit::adjustUnitRectLeftRight( int leftAdjustment, int rightAdjustment ){
	unitRect.left	+=	leftAdjustment;
	unitRect.right	+=	rightAdjustment;
};

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

void Unit::setUnitCanUseAbility( bool b ){
	unitCanUseAbility	=	b;
};

bool Unit::canUnitUseAbility()
{
	return unitCanUseAbility;
};