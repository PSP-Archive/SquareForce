#ifndef _GAMEMENUCREDITS_H_
#define _GAMEMENUCREDITS_H_

#include <list>
#include "IGame.h"

using namespace std;

class JTexture;
class JTTFont;

struct Vector2D;

class GameMenuCredits : public IGame
{
	
public:
	GameMenuCredits();
	virtual ~GameMenuCredits();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();
	virtual void Pause();
	virtual void Resume();

private:
	JTexture* mTexBG;
	JQuad* mBGMesh;

	JTTFont* mFont;
	JTTFont* mFont2;
	JTTFont* mFont3;
 	
	bool mIsFadingIn;
	bool mIsFadingOut;
	float mFaderTimer;

	float mCpt;

};

#endif
