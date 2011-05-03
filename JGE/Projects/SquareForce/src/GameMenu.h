#ifndef _GAMEMENU_H_
#define _GAMEMENU_H_

#include <list>
#include "IGame.h"

using namespace std;

class JTexture;
class hgeDistortionMesh;
class JTTFont;

struct Vector2D;

class GameMenu : public IGame
{
	
public:
	GameMenu();
	virtual ~GameMenu();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();
	virtual void Pause();
	virtual void Resume();

	inline int GetLine() {return mLine;}

private:
	JTexture* mTexBG;
	JQuad* mBGMesh;

	JTTFont* mFont;
	JTTFont* mFont2;
 	//hgeDistortionMesh* mDistortionMesh;

	bool mIsFadingIn;
	bool mIsFadingOut;
	float mFaderTimer;
	
	int mLine;

	float mCpt;

	list<Vector2D> mTiles;

	JTexture *mSquareTilesTex;
	JTexture *mLightningTex;
	JQuad** mLightningQuads;
	JQuad* mSquareTileQuad;

	JMusic* mMusic;
};

#endif
