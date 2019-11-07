#define WIN32_LEAN_AND_MEAN 

#define NOATOM
#define NOCLIPBOARD
#define NOCOMM
#define NOCTLMGR
#define NOCOLOR
#define NODEFERWINDOWPOS
#define NODESKTOP
#define NODRAWTEXT
#define NOEXTAPI
#define NOGDICAPMASKS
#define NOHELP
#define NOICONS
#define NOTIME
#define NOIMM
#define NOKANJI
#define NOKERNEL
#define NOKEYSTATES
#define NOMCX
#define NOMEMMGR
#define NOMENUS
#define NOMETAFILE
#define NOMSG
#define NONCMESSAGES
#define NOPROFILER
#define NORASTEROPS
#define NORESOURCE
#define NOSCROLL
//#define NOSERVICE		/* Windows NT Services */
#define NOSHOWWINDOW
#define NOSOUND
#define NOSYSCOMMANDS
#define NOSYSMETRICS
#define NOSYSPARAMS
#define NOTEXTMETRIC
#define NOVIRTUALKEYCODES
#define NOWH
#define NOWINDOWSTATION
#define NOWINMESSAGES
#define NOWINOFFSETS
#define NOWINSTYLES
#define OEMRESOURCE
#pragma warning(disable : 4996)

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <WinSock2.h>

#if !defined(_Wp64)
#define DWORD_PTR DWORD
#define LONG_PTR LONG
#define INT_PTR INT
#endif

typedef struct _RECORD {

	int AccountNumber;

	int Transaction;

} RECORD;


typedef struct _Account {

	int AccountNumber;
	int balance;
	CRITICAL_SECTION acc;

	HANDLE a;
} ACCOUNT;



RECORD r1;
DWORD BIn;
HANDLE FileHandle;

int *Transactions;
ACCOUNT *Accounts;


int NumOfAccounts;

LARGE_INTEGER FileSize;


void doTransaction(char* filename) {
	//printf("dddsdf");
	DWORD BIn;

	LARGE_INTEGER File;

	RECORD curTransaction;

	HANDLE f = /* This handle is inheritable */
		CreateFile(filename,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	GetFileSizeEx(f, &File);


	for (int i = 0; i < File.QuadPart / 8; i++) {

		ReadFile(f, &curTransaction, 8, &BIn, NULL);

		EnterCriticalSection(&Accounts[curTransaction.AccountNumber].acc);
		Accounts[curTransaction.AccountNumber].balance += curTransaction.Transaction;
		LeaveCriticalSection(&Accounts[curTransaction.AccountNumber].acc);

	}


}


int _tmain(int argc, LPTSTR argv[])
{
	HANDLE *hThreads;
	HANDLE STDInput = GetStdHandle(STD_INPUT_HANDLE); //get the handle to standard input
	GetFileSizeEx(STDInput, &FileSize);
	NumOfAccounts = FileSize.QuadPart / 8;

	Accounts = malloc(sizeof(ACCOUNT) * NumOfAccounts);
	hThreads = malloc(sizeof(HANDLE) * (argc -1));
	
	for (int i = 0; i < NumOfAccounts; i++) {

		ReadFile(STDInput, &Accounts[i], 8, &BIn, NULL);
		InitializeCriticalSection(&Accounts[i].acc);

	}

	for (int x = 0; x < argc-1; x++)
	{
		//strcpy(searchPatternArray[x - 1].searchPattern, argv[x]);
		hThreads[x] = (HANDLE)_beginthreadex(NULL, 0, doTransaction, argv[x+1], NULL, NULL);

	}

	for (int x = 0; x < argc-1; x++)
	{
		WaitForSingleObject(hThreads[x], INFINITE);
	}

	HANDLE STDOutput = GetStdHandle(STD_OUTPUT_HANDLE); //get the handle to standard input

	for (int i = 0; i < NumOfAccounts; i++) {

		//HANDLE f = /* This handle is inheritable */
		//	CreateFile(filename,
		//		GENERIC_READ | GENERIC_WRITE,
		//		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		//		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		WriteFile(STDOutput,&Accounts[i], 8, &BIn, NULL);


	}

	return 0;
}


