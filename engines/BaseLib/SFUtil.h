#pragma once
//#include <tchar.h>

namespace SFUtil
{
	/*typedef enum eExtractPathInfoType
	{

		PATH_DRIVE = 0,
		PATH_DIR,
		PATH_FILENAME,
		PATH_EXTENSION,
	};

	DWORD GetPESize(LPCTSTR filename); //PE(Portable Executable) ������ ����� ��´�.
	INT	GetProgramName( LPWSTR buf, INT len ); //���������� �̸��� ��´�.
	TCHAR* ExtractPathInfo(TCHAR* szPath, eExtractPathInfoType InfoType); //���� ��ηκ��� �ʿ� ������ �����Ѵ�.
	TCHAR* BuildPath(TCHAR* szDrive, TCHAR* szDirectory, TCHAR* szFileName, TCHAR* szExtension); //�ش� ���ϰ� ���õ� ������ ��θ� �����Ѵ�.
	void ShowFileProperties(HWND hwnd, LPTSTR lpszFile); //������ ������ ��´�.
	TCHAR* CreateGuid(int& iGuidSize); //GUID ����

#ifdef _DEBUG
	char* CustomFormatA(LPCSTR msg, ...);
#else
	wchar_t* CustomFormatW(LPCWSTR msg, ...);
#endif

	DWORD CountSetBits(ULONG_PTR bitMask); // Helper function to count set bits in the processor mask.
	BOOL ConvertBytesToMB(DWORD dwBytes, DWORD& dwMB, DWORD& dwKB);  //����Ʈ���� �ް�����Ʈ, ų�ι���Ʈ�� ��ȯ
	BOOL Is64BitOS(); //���� �ü���� 64��Ʈ �Ǵ� 32��Ʈ �ü�� �󿡼� ����ǰ� �ִ����� Ȯ�ν��� �ش�.
	void GetScreenResolution(RECT& DesktopRect); //��ũ�� �ػ� ���
	void CheckOSVersion(DWORD& dwVersion, DWORD dwMajorVersion, DWORD dwMinorVersion, DWORD& dwBuild); //OS Version ���
	float GetWindowAssetPoint(); //���� ��ǻ�� ���� ���� ����.. ��Ÿ �̻󿡼��� ����� ����
*/
	void SetCurDirToModuleDir();

};