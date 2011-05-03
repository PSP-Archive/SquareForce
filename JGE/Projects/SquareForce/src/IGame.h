//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _IGAME_H_
#define _IGAME_H_

class IGame
{
protected:
	 bool mQuit;

public:
	IGame():mQuit(false) {}
	virtual ~IGame(){}
	virtual void Create() = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void Pause() = 0;
	virtual void Resume() = 0;

	inline bool HasQuit() {return mQuit;}
};



#endif
