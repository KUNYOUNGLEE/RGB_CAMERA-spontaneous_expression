#pragma once
#include "igamemodemgr.h"
#include "BackView.h"
#include <windows.h>
//class CGfxMgr;

class PrologueMode :
	public IGameModeMgr
{
public:
	PrologueMode(void);
	virtual ~PrologueMode(void);

public:
	void Init();	
	void Clear();
	void Update(); 
	void Draw();
	void Keyproc();

protected:
//	BackView m_BackView;
//	CGfxMgr* m_PrologueMovie;
	HWND hWndAVI;
};
