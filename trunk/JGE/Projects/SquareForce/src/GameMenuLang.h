#ifndef _GAMEMENULANG_H_
#define _GAMEMENULANG_H_

#include <vector>
#include <string>
#include "IGame.h"

using namespace std;

class GameMenuLang : public IGame
{
	
public:
	GameMenuLang();
	virtual ~GameMenuLang();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();
	virtual void Pause();
	virtual void Resume();

	inline int GetLine() {return mLine;}

private:
	JTTFont* mFont;
	
	bool mIsFadingIn;
	bool mIsFadingOut;
	float mFaderTimer;
	
	int mLine;

	float mCpt;

	vector<string> mLanguages;
	int mNbLanguages;
};

#endif
