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
		occupied					=	true;
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
		unitRect.left		=	10;
		unitRect.right		=	80;
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
		unitRect.top			=	80;
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
	state						=	-1;
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
					case 10:
						adjustUnitRectLeftRight( 115, 138 );
						break;
					case 125:
						adjustUnitRectLeftRight( 130, 123 );
						break;
					case 255:
						adjustUnitRectLeftRight( 130, 123 );
						break;
					case 385:
						adjustUnitRectLeftRight( 130, 123 );
						break;
					case 515:
						adjustUnitRectLeftRight( 130, 123 );
						break;
					case 645:
						adjustUnitRectLeftRight( 130, 123 );
						break;
					case 775:
						adjustUnitRectLeftRight( 130, 123 );
						break;
						//	case 686:
						//	setUnitRect(0, 784, 847, 85);
						//	break;
					case 905:
						setUnitRect(0, 10, 80, 85);
						break;
					}
				}
				break;
			case MOVING:{
				if( animationTimer > 0.10f )
				{
					animationTimer -= 0.10f;
					//setState( IDLE );
					switch(unitRect.left)
					{
					case 45:
						adjustUnitRectLeftRight( 155, 158 );
						break;
					case 200:
						adjustUnitRectLeftRight( 155, 158 );
						break;
					case 355:
						adjustUnitRectLeftRight( 155, 158 );
						break;
					case 510:
						adjustUnitRectLeftRight( 155, 158 );
						break;
					case 665:
						adjustUnitRectLeftRight( 155, 158 );
						break;
					case 820:
						adjustUnitRectLeftRight( 155, 158 );
						break;
					case 975:
						adjustUnitRectLeftRight( 155, 158 );
						break;
					case 1130:
						adjustUnitRectLeftRight( 155, 158 );
						break;
					case 1285:
						adjustUnitRectLeftRight( 155, 158 );
						break;
					case 1440:

						unitRect.top		=	85;
						unitRect.left		=	45;
						unitRect.right		=	160;
						unitRect.bottom		=	170;
						break;
					}
				}
						}
						break;
			case ATTACKING:
				if( animationTimer > 0.05f )
				{
					animationTimer -= 0.05f;
					//setState( IDLE );
					switch(unitRect.left){
					case 0:
						adjustUnitRectLeftRight( 280, 290 );
						break;
					case 280:
						adjustUnitRectLeftRight( 285, 285 );
						break;
					case 565:
						adjustUnitRectLeftRight( 285, 285 );
						break;
					case 850:
						adjustUnitRectLeftRight( 285, 285 );
						break;
					case 1135:
						adjustUnitRectLeftRight( 255, 315 );
						break;
					case 1390:
						adjustUnitRectLeftRight( 240, 330 );
						break;
					case 1630:
						adjustUnitRectLeftRight( 270, 300 );
						break;
					case 1900:
						adjustUnitRectLeftRight( 285, 285 );
						break;
					case 2185:
						adjustUnitRectLeftRight( 285, 285 );
						break;
					case 2470:
						adjustUnitRectLeftRight( 285, 285 );
						break;
					case 2755:
						adjustUnitRectLeftRight( 285, 285 );
						break;
					case 3040:
						adjustUnitRectLeftRight( 285, 285 );
						break;
					case 3325:
						adjustUnitRectLeftRight( 285, 285 );
						break;
					case 3610:
						adjustUnitRectLeftRight( 285, 285 );
						break;
					case 3895:
						setState( IDLE );
						break;
					}
				}
				   
				break;
			case HIT:
			if( animationTimer > 0.1f )
			{
				animationTimer -= 0.1f;
				//setState( IDLE );
				switch(unitRect.left){
				case 0:
					adjustUnitRectLeftRight( 150, 135 );
					break;
				case 150:
					adjustUnitRectLeftRight( 150, 135 );
					break;
				case 300:
					adjustUnitRectLeftRight( 150, 135 );
					break;
				case 450:
					adjustUnitRectLeftRight( 150, 135 );
					break;
				case 600:
					setState( IDLE );
					break;
				}
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
			case ATTACKING:
				if( animationTimer > 0.08f ){
					animationTimer	-=	0.08f;
					switch( unitRect.left ){
					case 5:
						adjustUnitRectLeftRight( 50, 53 );
						break;
					case 55:
						adjustUnitRectLeftRight( 50, 50 );
						break;
					case 105:
						adjustUnitRectLeftRight( 45, 45 );
						break;
					case 150:
						adjustUnitRectLeftRight( 45, 44 );
						break;
					case 195:
						adjustUnitRectLeftRight( 44, 45 );
						break;
					case 239:
						adjustUnitRectLeftRight( 48, 46 );
						break;
					case 287:
						adjustUnitRectLeftRight( 50, 50 );
						break;
					case 337:
						adjustUnitRectLeftRight( 50, 50 );
						break;
					case 387:
						adjustUnitRectLeftRight( 45, 45 );
						break;
					case 432:
						setState( IDLE );//adjustUnitRectLeftRight(
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
				if( animationTimer > 0.2f )
				{
					animationTimer -= 0.2f;
					//setState( IDLE );
					switch(unitRect.left){
					case 273:
						adjustUnitRectLeftRight( 157, 152 );
						break;
					case 430:
						adjustUnitRectLeftRight( 170, 190 );
						break;
					case 600:
						adjustUnitRectLeftRight( 170, 187 );
						break;
					case 770:
						setState( IDLE );
						break;
					}
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
			case HIT:
				if( animationTimer > 0.12f ){
					animationTimer	-=	0.12f;
					switch( unitRect.left ){
					case 30:
						adjustUnitRectLeftRight( 110, 90 );
						break;
					case 140:
						adjustUnitRectLeftRight( 90, 100 );
						break;
					case 230:
						setState( IDLE );
						break;
					}
				}
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
			switch( state ){
			case IDLE:
				if( animationTimer > 0.15f ){
					animationTimer -= 0.15f;
					switch(unitRect.left){
					case 0:
						adjustUnitRectLeftRight( 105, 104 );
						break;
					case 105:
						adjustUnitRectLeftRight( 105, 104 );
						break;
					case 210:
						adjustUnitRectLeftRight( 105, 104 );
						break;
					case 315:
						unitRect.top			=	25;
						unitRect.left			=	0;
						unitRect.right			=	50;
						unitRect.bottom			=	180;
						break;
					}
				}
			break;
			case MOVING:
				if( animationTimer > 0.1f ){
					animationTimer	-=	0.1f;
					switch( unitRect.left ){
					case 0:
						adjustUnitRectLeftRight( 105, 120 );
						break;
					case 105:
						adjustUnitRectLeftRight( 105, 113 );
						break;
					case 210:
						adjustUnitRectLeftRight( 102, 120 );
						break;
					case 312:
						adjustUnitRectLeftRight( 101, 118 );
						break;
					case 413:
						adjustUnitRectLeftRight( 105, 114 );
						break;
					case 518:
						adjustUnitRectLeftRight( 105, 118 );
						break;
					case 623:
						adjustUnitRectLeftRight( 105, 118 );
						break;
					case 728:
						unitRect.top			=	190;
						unitRect.left			=	0;
						unitRect.right			=	60;
						unitRect.bottom			=	345;
						break;
					}
				}
				break;
			case ATTACKING:
				if( animationTimer > 0.15f ){
					animationTimer	-=	0.15f;
					switch( unitRect.left ){
					case 0:
						adjustUnitRectLeftRight( 105, 120 );
						break;
					case 105:
						adjustUnitRectLeftRight( 105, 104 );
						break;
					case 210:
						adjustUnitRectLeftRight( 102, 108 );
						break;
					case 312:
						adjustUnitRectLeftRight( 101, 108 );
						break;
					case 413:
						setState( IDLE );
						break;
					}
				}
				break;
			case HIT:
				if( animationTimer > 0.15f ){
					animationTimer -= 0.15f;
					switch(unitRect.left){
					case 500:
						adjustUnitRectLeftRight( 105, 104 );
						break;
					case 605:
						adjustUnitRectLeftRight( 105, 104 );
						break;
					case 710:
						setState( IDLE );
						break;
					}
				}
				break;
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
						setUnitRect(80, 5, 70, 145);
						break;
					}
					
				}
				break;
			case ATTACKING:{
				if( animationTimer > 0.07f ){
					animationTimer	-=	0.07f;
					switch( unitRect.left ){
					case 15:
						adjustUnitRectLeftRight( 70, 70 );
						break;
					case 85:
						adjustUnitRectLeftRight( 70, 51 );
						break;
					case 155:
						adjustUnitRectLeftRight( 60, 61 );
						break;
					case 215:
						adjustUnitRectLeftRight( 60, 72 );
						break;
					}
				}
						   }
				break;
			case HIT:
				if( animationTimer > 0.10f )
				{
					animationTimer -= 0.1f;
					switch(unitRect.left)
					{
					case 15:
						adjustUnitRectLeftRight( 80, 70 );
						break;
					case 95:
						adjustUnitRectLeftRight( 85, 85 );
						break;
					case 180:
						setState( IDLE );
						break;
					}
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
		switch( state ){
		case IDLE:
			animationTimer		=	0.0f;
			unitRect.top		=	0;
			unitRect.left		=	10;
			unitRect.right		=	80;
			unitRect.bottom		=	85;
			break;
		case MOVING:
			animationTimer		=	0.0f;
			unitRect.top		=	85;
			unitRect.left		=	45;
			unitRect.right		=	160;
			unitRect.bottom		=	170;
			break;
		case ATTACKING:
			animationTimer		=	0.0f;
			unitRect.top		=	185;
			unitRect.left		=	0;
			unitRect.right		=	85;
			unitRect.bottom		=	282;
			break;
		case HIT:
			animationTimer		=	0.0f;
			unitRect.top		=	280;
			unitRect.left		=	0;
			unitRect.right		=	150;
			unitRect.bottom		=	370;
			break;
		}
		break;
	case MARKSMAN:
		switch( state ){
		case IDLE:
			animationTimer		=	0.0f;
			unitRect.top		=	0;
			unitRect.left		=	0;
			unitRect.right		=	45;
			unitRect.bottom		=	70;
			break;
		case ATTACKING:
			animationTimer		=	0.0f;
			unitRect.top		=	470;
			unitRect.left		=	5;
			unitRect.right		=	50;
			unitRect.bottom		=	540;
			break;
		}
		break;
	case MINOTAUR:
		switch( state ){
		case IDLE:
			setUnitRect(0, 215, 352, 180 );
			break;
		case ATTACKING:
			setUnitRect( 205, 273, 424, 430 );
			break;
		}
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
		case HIT:
			setUnitRect( 820, 30, 135, 950 );
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
		switch( state ){
		case IDLE:
			animationTimer			=	0.0f;
			unitRect.top			=	25;
			unitRect.left			=	0;
			unitRect.right			=	50;
			unitRect.bottom			=	180;
			break;
		case MOVING:
			animationTimer			=	0.0f;
			unitRect.top			=	190;
			unitRect.left			=	0;
			unitRect.right			=	60;
			unitRect.bottom			=	345;
			break;
		case ATTACKING:
			animationTimer			=	0.0f;
			unitRect.top			=	695;
			unitRect.left			=	0;
			unitRect.right			=	65;
			unitRect.bottom			=	895;
			break;
		case HIT:
			animationTimer			=	0.0f;
			unitRect.top			=	25;
			unitRect.left			=	500;
			unitRect.right			=	610;
			unitRect.bottom			=	180;
			break;
		}
		break;
	case BLACKMAGEUNIT:
		switch( state ){
		case IDLE:
			animationTimer		=	0.0f;
			unitRect.top			=	80;
			unitRect.left			=	5;
			unitRect.right		=	70;
			unitRect.bottom	=	145;
			break;
		case ATTACKING:
			animationTimer			=	0.0f;
			unitRect.top			=	445;
			unitRect.left			=	15;
			unitRect.right			=	85;
			unitRect.bottom			=	510;
			break;
		case HIT:
			animationTimer	=	0.0f;
			unitRect.top			=	300;
			unitRect.left			=	15;
			unitRect.right		=	88;
			unitRect.bottom	=	358;
			break;
		}
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