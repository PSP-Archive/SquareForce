//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _IPANEL_H_
#define _IPANEL_H_

class IPanel
{
	
public:
	IPanel():mQuit(false), mNextPanel(NULL), mSkipControls(false) {}
	virtual ~IPanel() {}
	virtual void Create() = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

	inline bool HasQuit() {return mQuit;}
	inline IPanel* GetNextPanel() {return mNextPanel;}

	inline void EnableSkipControls(bool enable) {mSkipControls = enable;}
protected:
	bool mQuit;

	IPanel* mNextPanel;

	bool mSkipControls;
};

#endif
