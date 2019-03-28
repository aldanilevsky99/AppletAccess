////////////////////////////////////////////////////////////////////////////////
// APPLETACCESS - For simplifications to run Java applets.
// Date:		- 19.03.2019
// Author:	    - www.github.ru/aldanilevsky99 
// IDE:			- Microsoft Visual Studio 6 
////////////////////////////////////////////////////////////////////////////////
// TODO: 1.Shell Extension
//		 2.Check if record is present in config 
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <commdlg.h> 
#include <ShellAPI.h>

#include <fstream>
using namespace std;
#define SEC_PATH "\\AppData\\LocalLow\\Sun\\Java\\Deployment\\security\\"
#define SEC_NAME  "exception.sites"
#define INVALID_HANDLE -1


////////////////////////////////////////////////////////////////////////////////
// GetPathToSecConfig() - Define path to Java security config
// IN: path - path to file
// OUT:path - path to file converted to URI
////////////////////////////////////////////////////////////////////////////////
void GetPathToSecConfig(char *SecPath){
	char WinDir[MAX_PATH];
	char UserName[MAX_PATH];

	// Define path to Windows disk
	GetWindowsDirectory(WinDir, MAX_PATH);

	// Use only disk, e.g. "c:\"
	WinDir[3]='\0';
		
	// Define current user name to find path to Java Security config
	memset(UserName, 0, MAX_PATH);
	ULONG ul = MAX_PATH;
	GetUserName(UserName, &ul);

	// Form path to config
	//
	// C:\Users\%profile%\AppData\LocalLow\Sun\Java\Deployment\security\exception.sites
	//
	strcat( WinDir,"Users\\" );
	strcat( WinDir,UserName );
	strcat( WinDir, SEC_PATH );
	strcat( WinDir, SEC_NAME );
	strcpy( SecPath, WinDir );
}



////////////////////////////////////////////////////////////////////////////////
// ConvertPathToURI() - convert Windows local path to URI which use in Java Security Exception 
// IN: path - path to file
// OUT:path - path to file converted to URI
//*****************************************************************************
// Windows   Local path: C:\Coding\Java\Books\WorkshopApplets\Chap03\Selection\SelectSort.html
// C++ path: C:\\Coding\\Java\\Books\\WorkshopApplets\\Chap03\\Selection\\SelectSort.html
// URI path: FILE:////C:/Coding/Java/Books/WorkshopApplets/Chap03/Selection/SelectSort.html
//*****************************************************************************
void ConvertPathToURI(char *URIpath){
	char FILE[] = "FILE:///";

	// Change symbol '\' to '/'
	for (UINT i=0; i<strlen(URIpath); i++) {
		if (URIpath[i]=='\\') {		
			URIpath[i]='/';
		}	
	}

	// Form path
	char lpb2[MAX_PATH];
	strcpy( lpb2, FILE );
	strcat( lpb2, URIpath );
	strcpy( URIpath, lpb2 );		

}


////////////////////////////////////////////////////////////////////////////////
// FileExists() - Check for existing file
// IN: lpName - name file 
// OUT:TRUE   - if file exists TRUE
//		 
////////////////////////////////////////////////////////////////////////////////
bool FileExists( char* lpName ){
	return GetFileAttributes( lpName ) != INVALID_HANDLE;
}


////////////////////////////////////////////////////////////////////////////////
// AddExceptionRecordToConfig() - write to file
// IN: ConfPath - path to config
//	   NeedText - what to write
////////////////////////////////////////////////////////////////////////////////
void AddExceptionRecordToConfig(char *ConfPath, char *NeedText){

	// if file not exists 
	if (!FileExists(ConfPath)){
		MessageBox(0,"File not found!",0,0);
		return;
	}

	//
	// Open, go to the end, write, write line break, close
	ofstream ofstr; 
	ofstr.open(ConfPath, ios::in);				  
	ofstr.seekp(0, ios::end);
	ofstr.write(NeedText,strlen(NeedText));	
	ofstr << endl;								 
	ofstr.close();	
}


////////////////////////////////////////////////////////////////////////////////
//
// Interface part
//
////////////////////////////////////////////////////////////////////////////////
void OpenFileAndDoAccess(HWND hWnd){

	OPENFILENAME ofn;       
    char szFile[260];                    

    ZeroMemory(&ofn, sizeof( ofn ));
    ofn.lStructSize = sizeof( ofn );
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof( szFile );
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Select file
	if ( GetOpenFileName( &ofn ) == TRUE ) { 
		
		// Define path to config 
		char LocalSecPath[MAX_PATH];
		GetPathToSecConfig( LocalSecPath );

		// Convert path to URI
		ConvertPathToURI( ofn.lpstrFile );

		// Add to Java Security config
		AddExceptionRecordToConfig( LocalSecPath, szFile );

		SetDlgItemText(hWnd, IDC_EDIT1, szFile);
		
		// Run Java applet if need
		if (MessageBox( 0,"Successfully added to Java Security config!\nAttention, check for repeat is not performed!"
			"\n\nDo you want to run Java Applet?","Success",MB_ICONQUESTION | MB_OKCANCEL ) == IDOK ) {
			// Run Java applet
			ShellExecute (NULL, "open", "iexplore.exe", szFile, NULL,SW_SHOWNORMAL);
			
		}
	}
}


BOOL CALLBACK DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
	switch (uMsg) 
	{
		case WM_COMMAND: 
		{

			if ( wParam == IDC_SELECT )
			{
				// Select file, and add Java Security access
				OpenFileAndDoAccess( hwndDlg );

				return TRUE;
			}

			if ( wParam == ID_ABOUT ) {
				MessageBox(0,"Tool for help with add old Java applets to Java Security Exception List\n\nCoded by https://github.com/aldanilevsky99",MB_OK,MB_ICONINFORMATION);
				return TRUE;
			}


			else return FALSE;
		}


		case WM_CLOSE:
			EndDialog(hwndDlg, NULL); 
				return TRUE;

			break;

		default: return FALSE; 
	} 
}



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DialogBoxParam( hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc, 0);
	return 0;
}



