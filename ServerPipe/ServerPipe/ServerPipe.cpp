//// SERVER PROGRAM /////

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include "PipeHandle.h"

int main()
{
	std::cout << "Creating an instance of a named pipe..." << std::endl;
	PipeHandel* pipe;
	wchar_t *pipeName = L"\\\\.\\pipe\\MyPipe";
	pipe = new PipeHandel(pipeName, true);
	while (1)
	{
		switch (pipe->getState())
		{
		case PipeHandel::PipeState::ePipeUnknown:
			delete pipe; 
			pipe = new PipeHandel(pipeName, true);
			break;
		case PipeHandel::PipeState::ePipeInitComple:
			if(!pipe->Connect())
				Sleep(1000);
			break;
		case PipeHandel::PipeState::ePipeRxTx:
			char *data; 
			pipe->ReadData(&data);
			Sleep(100);
			break;

		default:
			std::cout << "Error came to default ";
			break;
		}
	}
}