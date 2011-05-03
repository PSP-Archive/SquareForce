#ifndef _GAMELOGOGENESIS_H_
#define _GAMELOGOGENESIS_H_

#include "IGame.h"

class JTexture;

struct Vector2D;

class GameLogoGenesis : public IGame
{
	
public:
	GameLogoGenesis();
	virtual ~GameLogoGenesis();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();
	virtual void Pause();
	virtual void Resume();

private:
	JTexture* mTexBG;
	JQuad* mBGMesh;

	bool mIsFadingIn;
	bool mIsFadingOut;
	float mFaderTimer;

	float mCpt;

};

#endif
