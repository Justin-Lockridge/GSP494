#include "ClassAbilityAnimator.h"

ClassAbilityAnimator::ClassAbilityAnimator(){
	animationTimer			=		0.0f;
	posX					=		0.0f;
	posY					=		0.0f;
	scaleX					=		0.0f;
	scaleY					=		0.0f;
	type					=		-1;
	animationRect.top		=		0.0f;
	animationRect.left		=		0.0f;
	animationRect.right		=		0.0f;
	animationRect.bottom	=		0.0f;
	animationActive			=		false;
};

void ClassAbilityAnimator::setClassAbilityAnimation( int ability, float x, float y ){
	animationTimer		=	0.0f;
	posX				=	x;
	posY				=	y;
	scaleX				=	1.0f;
	scaleY				=	1.0f;
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
				if( activeTimer > 1.5f ){
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