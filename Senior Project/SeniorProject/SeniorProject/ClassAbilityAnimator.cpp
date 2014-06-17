#include "ClassAbilityAnimator.h"

ClassAbilityAnimator::ClassAbilityAnimator(){
	animationTimer			=		0.0f;
	posX					=		0.0f;
	posY					=		0.0f;
	scaleX					=		0.0f;
	scaleY					=		0.0f;
	type					=		-1;
	animationRect.top		=		long(0.0f);
	animationRect.left		=		long(0.0f);
	animationRect.right		=		long(0.0f);
	animationRect.bottom	=		long(0.0f);
	animationActive			=		false;
};

void ClassAbilityAnimator::setClassAbilityAnimation( int ability, float x, float y ){
	animationTimer		=	0.0f;
	posX				=	x;
	posY				=	y;
	scaleX				=	1.0f;
	scaleY				=	1.0f;
	animationActive		=	true;
	switch( ability ){
	case BLACKHOLE:
		type					=	BLACKHOLE;
		scaleX					=	0.40f;
		scaleY					=	0.40f;
		animationRect.top		=	0;
		animationRect.left		=	0;
		animationRect.right		=	250;
		animationRect.bottom	=	250;
		break;
	case FLAMESTRIKE:
		type					=	FLAMESTRIKE;
		scaleX					=	0.70f;
		scaleY					=	0.70f;
		animationRect.top		=	0;
		animationRect.left		=	60;
		animationRect.right		=	100;
		//animationRect.left			=	349;
		//animationRect.right			=	400;
		animationRect.bottom	=	150;
		break;
	case CLEAVE:
		type					=	CLEAVE;
		scaleX					=	0.40f;
		scaleY					=	0.40f;
		animationRect.top		=	0;
		animationRect.left		=	0;
		animationRect.right		=	140;
		//animationRect.left			=	349;
		//animationRect.right			=	400;
		animationRect.bottom	=	150;
		break;
	case BOLSTER:
		type					=	BOLSTER;
		scaleX					=	1.3f;
		scaleY					=	1.0f;
		animationRect.top		=	0;
		animationRect.left		=	0;
		animationRect.right		=	25;
		//animationRect.left			=	349;
		//animationRect.right			=	400;
		animationRect.bottom	=	50;
		break;
	case SPLITSHOT:
		type					=	SPLITSHOT;
		scaleX					=	1.2f;
		scaleY					=	1.1f;
		animationRect.top		=	0;
		animationRect.left		=	0;
		animationRect.right		=	35;
		//animationRect.left			=	349;
		//animationRect.right			=	400;
		animationRect.bottom	=	50;
		break;
	case SNIPE:
		type					=	SNIPE;
		scaleX					=	3.0f;
		scaleY					=	3.0f;
		break;
	}
};

void ClassAbilityAnimator::updateClassAbilityAnimation( float dt ){
	if( animationActive ){ 
		animationTimer	+= dt;
		activeTimer		+=	dt;
		switch( type ){
		case BLACKHOLE:
			if( animationTimer > 0.1f ){
				animationTimer	-=	0.1f;
				switch( animationRect.left ){
				case 0:
					adjustAnimationRectLeftRight( 255, 255 );
					break;
				case 255:
					adjustAnimationRectLeftRight( 255, 255 );
					break;
				case 510:
					adjustAnimationRectLeftRight( 255, 255 );
					break;
				case 765:
					setAnimationRect( 0, 0, 250, 250 );
					break;
				}
				if( activeTimer > 1.8f ){
					animationActive	=	false;
					activeTimer = 0.0f;
				}
			}
			break;
		case FLAMESTRIKE:
			if( animationTimer > 0.1f ){
				animationTimer	-=	0.1f;
				switch( animationRect.left ){
				case 60:
					adjustAnimationRectLeftRight( 36, 36 );
					break;
				case 96:
					adjustAnimationRectLeftRight( 43, 45 );
					break;
				case 139:
					adjustAnimationRectLeftRight( 48, 58 );
					break;
				case 187:
					adjustAnimationRectLeftRight( 55, 58 );
					break;
				case 242:
					adjustAnimationRectLeftRight( 55, 55 );
					break;
				case 297:
					adjustAnimationRectLeftRight( 52, 48 );
					break;
				case 349:
					adjustAnimationRectLeftRight( 47, 48 );
					break;
				case 396:
					setAnimationRect( 0, 60, 100, 150 );
					
					//setAnimationRect( 0, 0, 250, 250 );
					//if( activeTimer > 1.5f ){
						animationActive	=	false;
						activeTimer = 0.0f;
					//}
					break;
				}
			}
			break;
		case CLEAVE:
			if( animationTimer > 0.12f ){
				animationTimer	-=	0.12f;
				switch( animationRect.left ){
				case 0:
					adjustAnimationRectLeftRight( 140, 140 );
					break;
				case 140:
					adjustAnimationRectLeftRight( 135, 135 );
					break;
				case 275:
					adjustAnimationRectLeftRight( 140, 140 );
					break;
				case 415:
					setAnimationRect( 0, 0, 140, 150 );
					activeTimer	=	0.0f;
					animationActive	=	false;
					break;
				}
				//if( activeTimer > 3.5f ){
				//	animationActive	=	false;
				//	activeTimer = 0.0f;
				//}
			}
			break;
		case BOLSTER:
			if( animationTimer > 0.15f ){
				animationTimer	-=	0.15f;
				switch( animationRect.left ){
				case 0:
					adjustAnimationRectLeftRight( 33, 33 );
					break;
				case 33:
					adjustAnimationRectLeftRight( 33, 33 );
					break;
				case 66:
					adjustAnimationRectLeftRight( 33, 33 );
					break;
				case 99:
					adjustAnimationRectLeftRight( 33, 32 );
					break;
				case 132:
					adjustAnimationRectLeftRight( 33, 32 );
					break;
				case 165:
					adjustAnimationRectLeftRight( 33, 32 );
					break;
				case 198:
					adjustAnimationRectLeftRight( 33, 32 );
					break;
				case 231:
					animationActive		=	false;
					break;
				}
			}
			break;
		case SPLITSHOT:
				if(animationTimer > 0.06f){
					animationTimer -= 0.06f;
					switch(animationRect.left){
					case 0:
						adjustAnimationRectLeftRight( 32, 30 );
						//setUnitRect(0, 32, 65, 50);
						break;
					case 32:
						adjustAnimationRectLeftRight( 36, 42 );
						//setUnitRect(0, 70, 105, 50);
						break;
					case 68:
						adjustAnimationRectLeftRight( 40, 40 );
						//setUnitRect(0, 110, 145, 50);
						break;
					case 108:
						adjustAnimationRectLeftRight( 40, 40 );
						//setUnitRect( 0, 150, 185, 50 );
						break;
					case 148:
						adjustAnimationRectLeftRight( 30, 30 );
						//setUnitRect( 0, 180, 215, 50 );
						break;
					case 178:
						adjustAnimationRectLeftRight( 32, 32 );
						//setUnitRect( 0, 210, 245, 50 );
						break;
					case 210:
						adjustAnimationRectLeftRight( 34, 34 );
						//unitRect.left	+=	30;
						//unitRect.right	+=	30;
						break;
					case 244:
						adjustAnimationRectLeftRight( 32, 32 );
						//unitRect.left	+=	30;
						//unitRect.right	+=	30;
						break;
					case 276:
						adjustAnimationRectLeftRight( 32, 32 );
						//unitRect.left	+=	30;
						//unitRect.right	+=	30;
						break;
					case 308:
						adjustAnimationRectLeftRight( 32, 32 );
						//unitRect.top			=	0;
						//unitRect.left			=	0;
						//unitRect.right		=	35;
						//unitRect.bottom	=	50;
						//state	=	IDLE;
						break;
					case 340:
						adjustAnimationRectLeftRight( 34, 38 );
						break;
					case 374:
						adjustAnimationRectLeftRight( 45, 45 );
						break;
					case 419:
						adjustAnimationRectLeftRight( 45, 45 );
						break;
					case 464:
						animationActive	=	false;
						animationTimer	=	0.0f;
						type			=	-1;
						break;
					}
				}
			break;
		case SNIPE:
			break;
		}
	}
};

void ClassAbilityAnimator::setAnimationTimer( float f ){
	animationTimer	=	f;
};

void ClassAbilityAnimator::adjustAnimationTimer( float f ){
	animationTimer	+=	f;
};

float ClassAbilityAnimator::getAnimationTimer(){
	return animationTimer;
};

void ClassAbilityAnimator::setActiveTimer( float t ){
	activeTimer	=	t;
};

void ClassAbilityAnimator::adjustActiveTimer( float t ){
	activeTimer		=+	t;
};

float ClassAbilityAnimator::getActiveTimer(){
	return activeTimer;
};


void ClassAbilityAnimator::setPosX( float x ){
	posX	=	x;
};

void ClassAbilityAnimator::adjustPosX( float x ){
	posX	+=	x;
};

float ClassAbilityAnimator::getPosX(){
	return posX;
};

void ClassAbilityAnimator::setPosY( float y ){
	posY	=	y;
};

void ClassAbilityAnimator::adjustPosY( float y ){
	posY	+=	y;
};

float ClassAbilityAnimator::getPosY(){
	return posY;
};

void ClassAbilityAnimator::setScaleX( float x ){
	scaleX	=	x;
};

float ClassAbilityAnimator::getScaleX(){
	return scaleX;
};

void ClassAbilityAnimator::setScaleY( float y ){
	scaleY	=	y;
};

float ClassAbilityAnimator::getScaleY(){
	return scaleY;
};

void ClassAbilityAnimator::setType( int t ){
	type = t;
	if( type == NONE )
		animationActive	=	false;
};

int ClassAbilityAnimator::getType(){
	return type;
};

void ClassAbilityAnimator::setAnimationRect( int top, int left, int right, int bottom ){
	animationRect.top		=	top;
	animationRect.left		=	left;
	animationRect.right		=	right;
	animationRect.bottom	=	bottom;
};

void ClassAbilityAnimator::adjustAnimationRectLeftRight( int leftAdjustment, int rightAdjustment ){
	animationRect.left		+=	leftAdjustment;
	animationRect.right		+=	rightAdjustment;
};

RECT ClassAbilityAnimator::getAnimationRect(){
	return animationRect;
};

void ClassAbilityAnimator::setAnimationActive( bool b ){
	animationActive	=	b;
};

bool ClassAbilityAnimator::getAnimationActive(){
	return animationActive;
};