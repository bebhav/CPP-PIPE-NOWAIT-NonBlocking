///// CLIENT PROGRAM /////

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include "PipeHandle.h"
using namespace std;

int main()
{
	std::cout << "Connecting to pipe..." << std::endl;
	PipeHandel *pipe; 
	wchar_t* pipeName = L"\\\\.\\pipe\\MyPipe";
	pipe = new PipeHandel(pipeName, false); //client 

	int packetCnt = 0;
	while (1)
	{
		switch (pipe->getState())
		{
		case PipeHandel::PipeState::ePipeUnknown:
			delete pipe;
			pipe = new PipeHandel(pipeName, false); //client 
			break;
		case PipeHandel::PipeState::ePipeInitComple:
			if (!pipe->Connect())
				Sleep(1000); // try after 1 Sec
			break;
		case PipeHandel::PipeState::ePipeRxTx:
			char data[100];
			sprintf_s(data, sizeof(data), "This Is Test Packet : %d", packetCnt++);
			if (pipe->SendData(data))
				Sleep(10); // Send Next Packet After Some time  
			break;

		default:
			std::cout << "Error: Unknowd state in switch case \n";
			break;
		}
	}
	return 0;
}

//int main(int argc, const char **argv)
//{
//	cout << "Connecting to pipe..." << endl;
//	HANDLE pipe;
//	while (1)
//	{
//		// Open the named pipe
//		pipe = CreateFile(
//			L"\\\\.\\pipe\\my_pipe",
//			GENERIC_READ | GENERIC_WRITE, 
//			FILE_SHARE_READ | FILE_SHARE_WRITE,
//			NULL,
//			OPEN_EXISTING,
//			FILE_ATTRIBUTE_NORMAL,
//			NULL
//		);
//		//ERROR_PIPE_NOT_CONNECTED
//		if (pipe == INVALID_HANDLE_VALUE) {
//			cout << "Failed to connect to pipe. " << GetLastError() << GetLastErrorAsString() <<endl;
//		}
//		else
//		{
//			// creat nowait pipe 
//			DWORD dwMode = PIPE_NOWAIT | PIPE_TYPE_BYTE;
//			DWORD dwCollectionSize = 512;
//			if (SetNamedPipeHandleState(pipe, &dwMode, NULL, NULL) == 0)
//			{
//				cout << "Failed to connect to pipe. " << GetLastError() << GetLastErrorAsString() << endl;
//			}
//			else
//			{
//				int cnt = 0;
//				while (1) {
//					char str[100];
//					sprintf_s(str, sizeof(str), "This is test str %d", cnt++);
//					//const wchar_t* data = GetWC(str);
//					if (sendData(pipe, str))
//						break;
//					Sleep(10);
//				}
//			}
//		}
//		Sleep(1000);
//	}
//	// Close our pipe handle
//	CloseHandle(pipe);
//
//	cout << "Done." << endl;
//
//	return 0;
//}