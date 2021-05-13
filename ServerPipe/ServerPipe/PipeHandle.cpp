#include "stdafx.h"
#include "PipeHandle.h"


std::string GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) {
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}

PipeHandel::PipeHandel(wchar_t* name, bool isServer)
{
	this->isServer = isServer;
	PipeName = new wchar_t[wcslen(name)+1];
	wcscpy_s(PipeName,wcslen(name)+1, name);
	pipe = NULL;
	if (isServer)
	{
		// Create a pipe to send data
		pipe = CreateNamedPipe(
			PipeName, // name of the pipe
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_BYTE, // send data as a byte stream and dont wait 
			1, // only allow 1 instance of this pipe
			2000, // no outbound buffer
			2000, // no inbound buffer
			0, // use default wait time
			NULL // use default security attributes
		);

		if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
			std::cout << "Failed to create pipe instance. " << GetLastError() << GetLastErrorAsString();
			state = PipeState::ePipeUnknown;
		}
		else
		{
			DWORD dwMode = PIPE_NOWAIT | PIPE_TYPE_BYTE;
			if (SetNamedPipeHandleState(pipe, &dwMode, NULL, NULL) == 0)
			{
				std::cout << "Failed to create NoWait Pipe"
					<< GetLastError() << GetLastErrorAsString() << std::endl;
			}
			else
				state = PipeState::ePipeInitComple;
		}
	}
	else // client 
	{
		state = PipeState::ePipeInitComple;
	}
}

PipeHandel::~PipeHandel()
{
	if (state != PipeState::ePipeUnknown && pipe != NULL)
	{
		if(isServer) DisconnectNamedPipe(pipe);
		CloseHandle(pipe);
	}
}

bool PipeHandel::Connect()
{
	if (isServer)
	{
		std::cout << "Waiting for connection to pipe..." << std::endl;

		bool result = ConnectNamedPipe(pipe, NULL);
		DWORD error = GetLastError();

		if (!result && error != ERROR_PIPE_CONNECTED) {
			std::cout << "Failed to make connection on named pipe. " << GetLastError() << GetLastErrorAsString() << std::endl;
			if (error != ERROR_PIPE_LISTENING)
				DisconnectNamedPipe(pipe);
			state = PipeState::ePipeInitComple;
		}
		else
		{
			state = PipeState::ePipeRxTx;
			return true;
		}
	}
	else // client 
	{
		std::cout << "Connecting to pipe..." << std::endl;
		// Open the named pipe
		pipe = CreateFile(
			PipeName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		state = PipeState::ePipeInitComple;
		//ERROR_PIPE_NOT_CONNECTED
		if (pipe == INVALID_HANDLE_VALUE) {
			std::cout << "Failed to connect to pipe. " 
				<< GetLastError() << GetLastErrorAsString() << std::endl;
		}
		else
		{
			// Config pipe to nowait pipe 
			DWORD dwMode = PIPE_NOWAIT | PIPE_TYPE_BYTE;
			if (SetNamedPipeHandleState(pipe, &dwMode, NULL, NULL) == 0)
			{
				std::cout << "Failed to create NoWait Pipe"
					<< GetLastError() << GetLastErrorAsString() << std::endl;
			}
			else
			{
				state = PipeState::ePipeRxTx;
				return true; 
			}
		}
	}

	return false;
}

bool PipeHandel::SendData(char* data)
{
	DWORD numBytesWritten = 0;
	BOOL result = WriteFile(
		pipe, // handle to our outbound pipe
		data, // data to send
		strlen(data), // length of data to send (bytes)
		&numBytesWritten, // will store actual amount of data sent
		NULL // not using overlapped IO
	);


	if (!result)
	{
		std::cout << "Failed to send data." << GetLastError() << GetLastErrorAsString() << std::endl;
		if (GetLastError() == ERROR_BROKEN_PIPE ||
			GetLastError() == ERROR_NO_DATA ||
			GetLastError() == ERROR_PIPE_NOT_CONNECTED)
			state = PipeState::ePipeInitComple;
			return false;
	}
	state = PipeState::ePipeRxTx;
	std::cout << "Send:" << data;
	return true;
}

bool PipeHandel::ReadData(char** data)
{
	// The read operation will block until there is data to read
	static char buffer[1024];	//original
	if (data) *data = buffer;

	DWORD numBytesRead = 0;
	BOOL result = ReadFile(
		pipe,
		buffer, // the data from the pipe will be put here
		sizeof(buffer) - 1, // number of bytes allocated
		&numBytesRead, // this will store number of bytes actually read
		NULL // not using overlapped IO
	);

	if (!result)
	{
		DWORD errro = GetLastError();
		if (errro != ERROR_NO_DATA)
		{
			std::cout << "Failed to read data from the pipe. " << errro << GetLastErrorAsString() << std::endl;

			if (errro == ERROR_BROKEN_PIPE ||
				errro == ERROR_INVALID_HANDLE)
			{
				state = PipeState::ePipeInitComple;
				return false;
			}
		}
	}
	else
	{
		buffer[numBytesRead] = '\0'; // null terminate the string
		std::cout << buffer;
		state = PipeState::ePipeRxTx;
	}
	return true;
}

PipeHandel::PipeState PipeHandel::getState()
{
	return state;
}

