#include "StdAfx.h"
#include "SFUtil.h"
#include <objbase.h>
#include <Shellapi.h>
#include <tchar.h>
#include  <Winsatcominterfacei.h>
#include  <Lmshare.h>
#include  <Lm.h>
#include <direct.h>
#include <tchar.h>



#pragma warning(disable : 4996)

namespace SFUtil
{
	typedef enum eExtractPathInfoType
	{

		PATH_DRIVE = 0,
		PATH_DIR,
		PATH_FILENAME,
		PATH_EXTENSION,
	};


	DWORD GetPESize(LPCTSTR filename)
	{
		DWORD peSize=0UL;
		HANDLE hFile=CreateFile(filename,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile!=INVALID_HANDLE_VALUE)
		{
			IMAGE_DOS_HEADER dosHeader;
			DWORD readBytes=0UL;

			if ((ReadFile(hFile,&dosHeader,sizeof(dosHeader),&readBytes,NULL))&&(readBytes==sizeof(dosHeader)))
			{
				if (SetFilePointer(hFile,dosHeader.e_lfanew,NULL,FILE_BEGIN)!=0xFFFFFFFFUL)
				{
					DWORD signature;

					if ((ReadFile(hFile,&signature,sizeof(signature),&readBytes,NULL))&&(readBytes==sizeof(signature)))
					{
						IMAGE_FILE_HEADER fileHeader;

						if ((ReadFile(hFile,&fileHeader,sizeof(fileHeader),&readBytes,NULL))&&(readBytes==sizeof(fileHeader)))
						{
							WORD magic;

							if ((ReadFile(hFile,&magic,sizeof(magic),&readBytes,NULL))&&(readBytes==sizeof(magic)))
							{
								DWORD optionalHeaderSize=0UL;
								union
								{
									IMAGE_OPTIONAL_HEADER32 optionalHeader32;
									IMAGE_OPTIONAL_HEADER64 optionalHeader64;
								} optionalHeader;

								if (magic==IMAGE_NT_OPTIONAL_HDR32_MAGIC)
								{
									optionalHeaderSize=sizeof(optionalHeader.optionalHeader32)-sizeof(magic);
								}
								else if (magic==IMAGE_NT_OPTIONAL_HDR64_MAGIC)
								{
									optionalHeaderSize=sizeof(optionalHeader.optionalHeader64)-sizeof(magic);
								}

								if (optionalHeaderSize!=0UL)
								{
									if ((ReadFile(hFile,&optionalHeader,optionalHeaderSize,&readBytes,NULL))&&(readBytes==optionalHeaderSize))
									{
										int sectionNumber;

										for (sectionNumber=0;sectionNumber<fileHeader.NumberOfSections;sectionNumber++)
										{
											IMAGE_SECTION_HEADER sectionHeader;

											if ((ReadFile(hFile,&sectionHeader,sizeof(sectionHeader),&readBytes,NULL))&&(readBytes==sizeof(sectionHeader)))
											{
												if ((sectionHeader.SizeOfRawData!=0)&&(sectionHeader.PointerToRawData+sectionHeader.SizeOfRawData>peSize))
													peSize=sectionHeader.PointerToRawData+sectionHeader.SizeOfRawData;
											}
										}
									}
								}
							}
						}
					}
				}
			}

			CloseHandle(hFile);
		}

		return peSize;
	}

	INT	GetProgramName( LPWSTR buf, INT len )
	{
		WCHAR szDrive[MAX_PATH], szDir[MAX_PATH], szFilename[MAX_PATH], szExt[MAX_PATH];

		// Figure out what the report file will be named, and store it away
		GetModuleFileNameW(0, buf, len);

		LPWSTR pszDot = buf;

		// Look for the '.' before the "EXE" extension. Replace '.' to '\0'
		if((pszDot = wcsrchr( pszDot, L'.'))) 
		{
			*pszDot = 0;
		}

		_wsplitpath_s(buf, szDrive, szDir, szFilename, szExt);
		wcsncpy(buf, szFilename, len);

		return (INT)wcslen( buf );
	}

///////////////////////////////
//��Ƽ������ ���������� ����
///////////////////////////////
	TCHAR* ExtractPathInfo(TCHAR* szPath, eExtractPathInfoType InfoType)
	{
		static TCHAR szDrive[_MAX_DRIVE];
		static TCHAR szDirectory[_MAX_DIR];
		static TCHAR szFileName[_MAX_FNAME];
		static TCHAR szExtension[_MAX_EXT];

		_tsplitpath_s(szPath, szDrive, szDirectory, szFileName, szExtension );

		switch(InfoType)
		{
		case PATH_DRIVE:
			return szDrive;
		case PATH_DIR:
			return szDirectory;
		case PATH_FILENAME:
			return szFileName;
		case PATH_EXTENSION:
			return szExtension;
		}

		return NULL;
	}

	TCHAR* BuildPath(TCHAR* szDrive, TCHAR* szDirectory, TCHAR* szFileName, TCHAR* szExtension)
	{
		static TCHAR szPathBuffer[_MAX_PATH];
		_tmakepath_s( szPathBuffer, szDrive, szDirectory, szFileName, szExtension);
		return szPathBuffer;
	}

	void ShowFileProperties(HWND hwnd, LPTSTR lpszFile)
	{
		SHELLEXECUTEINFO ShExecInfo = {0};

		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_INVOKEIDLIST ;
		ShExecInfo.hwnd = hwnd;
		ShExecInfo.lpVerb = _T("properties");
		ShExecInfo.lpFile = lpszFile;
		ShExecInfo.lpParameters = _T(""); 
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_SHOW;
		ShExecInfo.hInstApp = NULL; 
		ShellExecuteEx(&ShExecInfo);
	}

	TCHAR* CreateGuid(int& iGuidSize) //�ϴ� 40���� ����
	{
		iGuidSize = 40;
		static TCHAR TestGUIDString[40] = {0,};  
		_GUID TestGUID;

		// CoCreateGuid �����ϱ�
		CoCreateGuid(&TestGUID); 

		// ������ GUID�� 829C1584-C57B-4dac-BCE7-6F33455F747A �� ���� �������� ��ȯ.
		wsprintf(TestGUIDString, _T("%.8X-%.4X-%.4X-%.2X%.2X-%.2X%.2X%.2X%.2X%.2X%.2X"), 
			TestGUID.Data1,    TestGUID.Data2,    TestGUID.Data3,  TestGUID.Data4[0],
			TestGUID.Data4[1], TestGUID.Data4[2], TestGUID.Data4[3], TestGUID.Data4[4],
			TestGUID.Data4[5], TestGUID.Data4[6], TestGUID.Data4[7]
		);

		return TestGUIDString;
	}

#ifdef _DEBUG
	char* CustomFormatA(LPCSTR msg, ...)
	{
		va_list va_ptr;
		static char message[256];

		va_start(va_ptr, msg);
		::vsprintf_s(message, msg, va_ptr); 
		va_end(va_ptr);

		return message;
	}


#else
	wchar_t* CustomFormatW(LPCWSTR msg, ...)
	{
		va_list va_ptr;
		static wchar_t message[256];

		va_start(va_ptr, msg);
		::vswprintf_s(message, msg, va_ptr); 
		va_end(va_ptr);

		return message;
	}
#endif

	DWORD CountSetBits(ULONG_PTR bitMask)
	{
		DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
		DWORD bitSetCount = 0;
		ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;    
		DWORD i;

		for (i = 0; i <= LSHIFT; ++i)
		{
			bitSetCount += ((bitMask & bitTest)?1:0);
			bitTest/=2;
		}

		return bitSetCount;
	}

	BOOL ConvertBytesToMB(DWORD dwBytes, DWORD& dwMB, DWORD& dwKB)
	{
		dwMB = 0;
		dwKB = 0;

		if(dwBytes == 0)
			return TRUE;

		if(dwBytes >= 1048576)
			dwMB = dwBytes / 1048576;

		DWORD dwRemain = dwBytes % 1048576;

		if(dwRemain > 0)
			dwKB = dwRemain / 1024;

		return TRUE;
	}

	BOOL Is64BitOS()
	{
		BOOL bIs64BitOS = FALSE;

		// We check if the OS is 64 Bit
		typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 

		LPFN_ISWOW64PROCESS
			fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
			GetModuleHandle(_T("kernel32")),"IsWow64Process");

		if (NULL != fnIsWow64Process)
		{
			if (!fnIsWow64Process(GetCurrentProcess(),&bIs64BitOS))
			{
				//error
			}
		}
		return bIs64BitOS;
	}

	void GetScreenResolution(RECT& DesktopRect)
	{     
		// Gets the Desktop window
		HWND hDesktop=::GetDesktopWindow();
		// Gets the Desktop window rect or screen resolution in pixels
		::GetWindowRect(hDesktop, &DesktopRect);
	}

	void CheckOSVersion(DWORD& dwVersion, DWORD dwMajorVersion, DWORD dwMinorVersion, DWORD& dwBuild)
	{    
		dwVersion = ::GetVersion();

		// Gets the Major version
		dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

		// Gets the Minor version
		dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

		// Get the build number. 
		if (dwVersion < 0x80000000)            
			dwBuild = (DWORD)(HIWORD(dwVersion));
		else    // Windows Me/98/95
			dwBuild =  0;
	}


	float GetWindowAssetPoint()
	{
		HRESULT hr = S_OK;
		IQueryRecentWinSATAssessment* pAssessment;
		IProvideWinSATResultsInfo* pResults = NULL;
		float baseScore = 0.0;
		WINSAT_ASSESSMENT_STATE state;

		CoInitialize(NULL);
		hr = CoCreateInstance(__uuidof(CQueryWinSAT),
			NULL,CLSCTX_INPROC_SERVER,
			__uuidof(IQueryRecentWinSATAssessment),
			(void**)&pAssessment);

		if (SUCCEEDED(hr))
		{
			hr = pAssessment->get_Info(&pResults);
			if (SUCCEEDED(hr))
			{
				hr = pResults->get_AssessmentState(&state);
				if (SUCCEEDED(hr))
				{
					switch(state)
					{
					case WINSAT_ASSESSMENT_STATE_UNKNOWN:
						wprintf(L"The state of the assessment is unknown.\n");
						break;

					case WINSAT_ASSESSMENT_STATE_VALID:
					case WINSAT_ASSESSMENT_STATE_INCOHERENT_WITH_HARDWARE:
						hr = pResults->get_SystemRating(&baseScore);
						if (SUCCEEDED(hr))
						{
							wprintf(L"The base score for the computer is %.1f\n", baseScore);
							if (WINSAT_ASSESSMENT_STATE_INCOHERENT_WITH_HARDWARE == state)
							{
								wprintf(L"Note that the hardware configuration of the computer has changed\n"
									L"since the assessment was run; the score reflects the prior configuration.");
							}
						}
						else
						{  // Handle error  }
							break;
						}
					case WINSAT_ASSESSMENT_STATE_NOT_AVAILABLE:
						wprintf(L"A formal assessment has not run on the computer.\n");
						break;
					case WINSAT_ASSESSMENT_STATE_INVALID:
						wprintf(L"The assessment data is not valid.\n");
						break;
					default:
						wprintf(L"The assessment state value is not valid.\n");

					}
				}
				else
				{
					// Handle error
				}
				pResults->Release();
				pResults = NULL;
			}
			else
			{
				// Handle error
			}
			pAssessment->Release();
			pAssessment = NULL;
		}
		else
		{
			// Handle error
		}

		if (SUCCEEDED(hr))
		return baseScore;

		return 0.0f;
	}

	void SetCurDirToModuleDir()
	{
		TCHAR szCurrentDirectory[MAX_PATH] = {0,};
		TCHAR* pEnd = NULL;
		GetModuleFileName(NULL, szCurrentDirectory, MAX_PATH);
		pEnd = _tcsrchr(szCurrentDirectory, _T('\\'));
		

		if(NULL == pEnd)
		{
			return;
		}

		*pEnd = _T('\0');
		SetCurrentDirectory(szCurrentDirectory);
	}

	
}