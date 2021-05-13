#pragma once
#include <iostream>
#include <windows.h>

class PipeHandel
{

public:
	enum class PipeState
	{
		ePipeUnknown,
		ePipeInitComple,
		ePipeRxTx,
	};
	PipeHandel(wchar_t* name,bool isServer);
	~PipeHandel();
	bool Connect();
	bool SendData(char* data);
	bool ReadData(char** data);
	PipeState getState();
private:
	PipeState state = PipeState::ePipeUnknown;
	HANDLE pipe = NULL;
	wchar_t *PipeName;
	bool isServer;
};

