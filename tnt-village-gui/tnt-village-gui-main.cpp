#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include "resource.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//necessario per la finestra di dialogo dove impostare la directory
#include "Shlobj.h"
#include <vector>

#include <commctrl.h>
#pragma comment (lib,"comctl32.lib")

#include "include\curl\curl.h"
#include "picturepreview.h"

PicturePreview* picture;

//GLOBALI
int numElements = 0;
std::wstring executablePath;
HWND hwndGoto;

void loadConfiguration()
{
	
	std::string line;
	//Risultato risultati[100];
	std::ifstream fileConfigurazione("C:\\Users\\Re Kotc\\development\\c++\\progetti\\curl-test\\tnt-village-GUI\\configurazione.txt");
	if (fileConfigurazione.is_open())
	{
		
		//la prima riga contiene il percorso del client torrent
		getline(fileConfigurazione, line);
		//questa conversione funziona solo se line contiene SOLO caratteri ASCII
		executablePath.assign(line.begin(), line.end());
		
		//prelevo una riga
	/*
		while (getline(fileRisultati, line))
		{
			//estraggo i dati
			//std::cout << line << '\n';
		}
		*/
	fileConfigurazione.close();
	}
}

void saveConfiguration()
{
	//cancello il file dei risultati esistente, se esiste
	remove("C:\\Users\\Re Kotc\\development\\c++\\progetti\\curl-test\\tnt-village-GUI\\configurazione.txt");
	//apro un nuovo file risultati
	std::wofstream fileConfigurazione;
	fileConfigurazione.open("C:\\Users\\Re Kotc\\development\\c++\\progetti\\curl-test\\tnt-village-GUI\\configurazione.txt", std::ios::out | std::ios::trunc);
	if (fileConfigurazione.is_open())
	{	
		//salvo alla prima riga il numero di risultati trovati, il valore viene poi utilizzato da tnt-village-gui
		fileConfigurazione << executablePath.c_str() << "\n";
		fileConfigurazione.close();
	}
}


std::string replace(std::string str, std::string search,std::string replace)
{
	for (size_t pos = 0; ; pos += replace.length()) {
		// Locate the substring to replace
		pos = str.find(search, pos);
		if (pos == std::string::npos) break;
		// Replace by erasing and inserting
		str.erase(pos, search.length());
		str.insert(pos, replace);
	}

	return str;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}


LPWSTR chooseDirectory()
{
	BROWSEINFO   bi;
	ZeroMemory(&bi, sizeof(bi));
	TCHAR*   szDisplayName = new TCHAR[MAX_PATH];
	//szDisplayName[0] = '';

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	//bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = _T("Please select a folder for storing received files :");
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.ulFlags = BIF_BROWSEINCLUDEFILES;
	bi.lParam = NULL;
	bi.iImage = 0;

	LPITEMIDLIST   pidl = SHBrowseForFolder(&bi);
	TCHAR*   szPathName = new TCHAR[MAX_PATH];
	SHGetPathFromIDList(pidl, szPathName);
	return szPathName;
}

bool callProcess(wchar_t* pszFileName, wchar_t* pszParameters, bool aSync)
{
	/*wchar_t* wString = new wchar_t[4096];
	wchar_t* wString2 = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, pszFileName, -1, wString, 4096);
	MultiByteToWideChar(CP_ACP, 0, pszParameters, -1, wString2, 4096);*/

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

	if(aSync==false)ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

	//ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = pszFileName;
	ShExecInfo.lpParameters = pszParameters;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	return true;

}


std::wstring getMagnet(std::wstring url)
{

	std::wstring par = TEXT("-m ") + url;
	std::wstring path = TEXT("C:\\Users\\Re Kotc\\development\\c++\\progetti\\curl-test\\Debug\\tnt-village-CLI.exe");
	wchar_t* wc = const_cast<wchar_t*>(path.c_str());
	wchar_t* wc2 = const_cast<wchar_t*>(par.c_str());

	callProcess(wc, wc2, false);

	//C:\Users\Re Kotc\development\c++\progetti\curl - test\tnt - village - gui
	std::string line;
	std::wstring magnetUrl;
	//Risultato risultati[100];
	std::ifstream fileRisultati("C:\\Users\\Re Kotc\\development\\c++\\progetti\\curl-test\\tnt-village-CLI\\magnet.txt");
	if (fileRisultati.is_open())
	{
		getline(fileRisultati, line);
		//numElements = std::stoi(line);
		magnetUrl.assign(line.begin(), line.end());
		fileRisultati.close();
	}

	return magnetUrl;
}




typedef struct
{
	int id;
	std::wstring idTNT;
	std::wstring label;
	std::wstring desc;
	std::wstring url;	
	std::wstring magnetUrl;
	std::wstring leech;
	std::wstring seed;
	std::wstring size;

} Risultato;

Risultato *risultati;

void loadResults(HWND hDlg) {

	HWND hwndProgressBar = GetDlgItem(hDlg, IDC_PROGRESSBAR);
	SendMessage(hwndProgressBar, PBM_SETPOS, 90, 0);
	//C:\Users\Re Kotc\development\c++\progetti\curl - test\tnt - village - gui
	std::string line;
	//Risultato risultati[100];
	std::ifstream fileRisultati("C:\\Users\\Re Kotc\\development\\c++\\progetti\\curl-test\\tnt-village-CLI\\risultati.txt");
	if (fileRisultati.is_open())
	{
		//la prima riga del file dei risultati contiene sempre il numero di risultati trovati
		getline(fileRisultati, line);
		numElements = std::stoi(line);
		//inizializzo l'array di risultati
		risultati = new Risultato[numElements];

		//prelevo una riga
		int i = 0;
		int j = 0;
		while (getline(fileRisultati, line))
		{
			
			switch (i)
			{
			case 0:
				risultati[j].id = stoi(line);
				break;
			case 1:
				risultati[j].idTNT.assign(line.begin(), line.end());
				break;
			case 2:
				risultati[j].label.assign(line.begin(), line.end());
				break;
			case 3:
				risultati[j].desc.assign(line.begin(), line.end());
				break;
			case 4:
				risultati[j].url.assign(line.begin(), line.end());
				break;
			case 5:
				risultati[j].leech.assign(line.begin(), line.end());
				break;
			case 6:
				risultati[j].seed.assign(line.begin(), line.end());
				break;
			case 7:
				risultati[j].size.assign(line.begin(), line.end());
				break;
			}
			//se ho inserito i 4 valori del j-esimo oggetto, passo all'oggetto j+1
			if (i == 7)j++;

			i = (i + 1) % 8;

			//estraggo i dati
			//std::cout << line << '\n';
		}
		fileRisultati.close();
	}
	else MessageBox(hDlg, L"Non trovo il file dei risultati!\nPop-Up", L"Error", MB_ICONEXCLAMATION);

	//vado a popolare la listview
	LVCOLUMN lvc;
	LVITEM LvItem;

	HWND hwndListView = GetDlgItem(hDlg, IDC_LISTVIEW);	
	memset(&LvItem, 0, sizeof(LvItem)); // Zero struct's Members
	WCHAR szText[256];     // Temporary buffer.
	std::wstring tempFields[5];
	//  Setting properties Of members:
	LvItem.mask = LVIF_TEXT;   // Text Style
	LvItem.cchTextMax = 256; // Max size of test	
	//LvItem.iSubItem = 0;       // Put in first coluom	
	//LvItem.pszText = szText; // Text to display (can be from a char variable) (Items)	

	//scorro tutti i risultati
	for (int i = 0; i < numElements; i++) {
		
		LvItem.iItem = i;          // choose item  
		SendMessage(hwndListView, LVM_INSERTITEM, 0, (LPARAM)&LvItem); // Send info to the Listview
		
		tempFields[0] = (risultati[i].label).c_str();
		tempFields[1] = (risultati[i].desc).c_str();

		tempFields[2] = (risultati[i].leech).c_str();

		tempFields[3] = (risultati[i].seed).c_str();

		tempFields[4] = (risultati[i].size).c_str();

		for (int j = 0; j < 5; j++) // Add SubItems in a loop
		{
			LvItem.iSubItem = j;
			//std::string temp = tempFields[j];
			//MultiByteToWideChar(CP_UTF8, 0, tempFields[j].c_str(), -1, szText, 256);
			LvItem.pszText = &tempFields[j][0];
			SendMessage(hwndListView, LVM_SETITEM, 0, (LPARAM)&LvItem); // Enter text to SubItems
		}
		LvItem.iSubItem = 0;
		//memset(&LvItem, 0, sizeof(LvItem)); // Zero struct's Members
	}

}

//svuota la lista dei risultati prima di una nuova ricerca
void cleanList(HWND hDlg) {

	HWND hwndList = GetDlgItem(hDlg, IDC_LIST1);
	HWND hwndListView = GetDlgItem(hDlg, IDC_LISTVIEW);
	SendMessageA(hwndList, LB_RESETCONTENT, NULL, NULL);
	SendMessageA(hwndListView, LVM_DELETEALLITEMS, NULL, NULL);
	numElements = 0;
}

void runSearch(HWND hDlg) {
	int bufSize = 1024;
	LPTSTR szText = new TCHAR[bufSize];
	GetDlgItemText(hDlg, IDC_EDIT2, szText, bufSize);
	std::wstring search = szText;
	/*
	wchar_t* prova;
	prova = szText;
	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, prova, -1, NULL, 0, NULL, NULL);

	::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &buf[0], size, 0, NULL);
	*/
	//tipo di file
	GetDlgItemText(hDlg, IDC_COMBO2, szText, bufSize);
	//test = szText;
	//delete[] szText;
	//std::string type = test;


	//replace(search, "'", "\'");

	//MessageBox(hDlg, L"Inizio la ricerca!\nPop-Up", L"Error", MB_ICONEXCLAMATION);
	bool terminato = false;
	std::wstring parametri = TEXT("-u rekotc -p superuserdo87 -s \"") + search + TEXT("\" -t film");

	if (numElements != 0)cleanList(hDlg);

	HWND hwndProgressBar = GetDlgItem(hDlg, IDC_PROGRESSBAR);
	SendMessage(hwndProgressBar, PBM_SETPOS, 10, 0);

	//std::wstring par = TEXT("-m ") + url;
	std::wstring path = TEXT("C:\\Users\\Re Kotc\\development\\c++\\progetti\\curl-test\\Debug\\tnt-village-CLI.exe");
	wchar_t* wc = const_cast<wchar_t*>(path.c_str());
	wchar_t* wc2 = const_cast<wchar_t*>(parametri.c_str());

	callProcess(wc, wc2, false);
	terminato = callProcess(wc, wc2, false);

	SendMessage(hwndProgressBar, PBM_SETPOS, 80, 0);
	loadResults(hDlg);
	SendMessage(hwndProgressBar, PBM_SETPOS, 100, 0);
}

INT_PTR CALLBACK ListBoxExampleProc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam);

int CALLBACK _tWinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPTSTR lpCmdLine,
	_In_  int nCmdShow)
{
	//inizializzo l'oggetto con cui gestisco l'immagine di anteprima
	picture = new PicturePreview();
	
	
	::DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, ListBoxExampleProc);
	
	return 0;
}

typedef struct
{
	TCHAR name[25];
	int id;
} Type;

Type types[] =
{
	{TEXT("Tutti"),0},
	{TEXT("Film"),1},
	{TEXT("Software"),2}

};

void initMainInterface(HWND hDlg) {

	//HWND activeWindow = GetActiveWindow();
	//controllo se l'activeWindow contiene IDC_IMAGE, in quel caso significa che sto lavorando con la finestra
	//principale, allora elimino anche l'oggetto picture.
	HWND imgHandler = GetDlgItem(hDlg, IDC_IMAGE);
	if (imgHandler != NULL)
	{
		HWND hwndProgressBar = GetDlgItem(hDlg, IDC_PROGRESSBAR);
		SendMessage(hwndProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
		HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
		SetMenu(hDlg, hMenu);

		//acquisisco l'handler sul menu a tendina
		HWND hWndComboBox = GetDlgItem(hDlg, IDC_COMBO2);
		//devo caricare le voci del menu a tendina
		for (int i = 0; i < ARRAYSIZE(types); i++)
		{
			SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)types[i].name);
		}
		//preseleziono "Tutti" dal menù a tendina
		SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);


		HWND hwndList = GetDlgItem(hDlg, IDC_LIST1);



		//TEST COLONNE
		WCHAR szText[256];     // Temporary buffer.
		const char* labelColonne[5] = { "Release","Descrizione","Leech","Seed","Dimensione" };
		//wchar_t* wString = new wchar_t[4096];

		LVCOLUMN lvc;
		int iCol;
		HWND hwndListView = GetDlgItem(hDlg, IDC_LISTVIEW);

		// Initialize the LVCOLUMN structure.
		// The mask specifies that the format, width, text,
		// and subitem members of the structure are valid.
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

		// Add the columns.
		for (iCol = 0; iCol < 5; iCol++)
		{
			MultiByteToWideChar(CP_ACP, 0, labelColonne[iCol], -1, szText, 256);
			lvc.iSubItem = iCol;
			lvc.pszText = szText;
			// Width of column in pixels.
			if (iCol == 0)lvc.cx = 300;
			else if (iCol == 1)lvc.cx = 300;
			else if (iCol == 2 || iCol == 3)lvc.cx = 50;
			else if (iCol == 4)lvc.cx = 70;
			else lvc.cx = 100;

			if (iCol < 2)
				lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
			else
				lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

										// Load the names of the column headings from the string resources.
										/*LoadString(g_hInst,
										0 + iCol,
										szText,
										sizeof(szText) / sizeof(szText[0]));
										*/
										// Insert the columns into the list view.
										/*if ( == -1)
										return FALSE;*/
			ListView_InsertColumn(hwndListView, iCol, &lvc);

		}

		picture->initPicture(hDlg, IDC_IMAGE);
		SetFocus(hwndList);
	}
}

INT_PTR CALLBACK ListBoxExampleProc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		picture->paintPicture(hDlg);
		return 0;
	case WM_CLOSE:
	{
		EnableWindow(hDlg, TRUE);

		HWND activeWindow = GetActiveWindow();
		//controllo se l'activeWindow contiene IDC_IMAGE, in quel caso significa che sto chiudendo la finestra
		//principale, allora elimino anche l'oggetto picture.
		HWND imgHandler = GetDlgItem(activeWindow, IDC_IMAGE);
		if(imgHandler!=NULL)picture->deletePicture();
		
		//EnableWindow(hDlg, TRUE);
			//SetFocus(hDlg);
		//ShowWindow(hDlg, SW_SHOW);
	
		

		EndDialog(activeWindow, 0); // Use this if you create the dialog via DialogBox function	
		//PostQuitMessage(0); // Use this if you create the dialog via CreateDialog		

		return TRUE;
	}

	//inizializzazione della finestra di dialogo
	case WM_INITDIALOG:
	{
		loadConfiguration();
		initMainInterface(hDlg);		
		return TRUE;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
	
		case ID_OPZIONI_ESCI40009:
		{
			exit(0);
		}

		case ID_OPZIONI_PREFERENZE: 
		{
			//cliccando sulla voce Preferenze visualizzo la finestra di dialogo relativa alle opzioni
			//::DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOGOPTION), NULL, NULL);
			hwndGoto = CreateDialog(GetModuleHandle(NULL),
				MAKEINTRESOURCE(IDD_DIALOGOPTION),
				hDlg,
				(DLGPROC)ListBoxExampleProc);

			EnableWindow(hDlg, FALSE);

			HWND hwndEditPath = GetDlgItem(hwndGoto, IDC_EDIT3);
			//SendMessage(hwndEditPath, WM_SETTEXT, NULL, (LPARAM)TEXT("Insert text here..."));
			
			SendMessage(hwndEditPath, WM_SETTEXT, NULL, (LPARAM)executablePath.c_str());

			ShowWindow(hwndGoto, SW_SHOW);
			//SetFocus(hwndGoto);

			/*
			switch (HIWORD(wParam))
			{
				case DOUBLE_CLICK:
				{
					MessageBox(hDlg, L"doppio click\nPop-Up", L"Error", MB_ICONEXCLAMATION);

				}
			}*/
			return true;
		}
		//gestione del pulsante "Cerca"
		case IDC_BUTTON1:
		{			
			runSearch(hDlg);
			return TRUE;
		}	

		case IDC_CHOOSE:
		{
			//runSearch(hDlg);
			//
			LPCWSTR str = chooseDirectory();
			executablePath = str;

			//executablePath = (std::wstring*)
			HWND hwndEditPath = GetDlgItem(hwndGoto, IDC_EDIT3);
			//SendMessage(hwndEditPath, WM_SETTEXT, NULL, (LPARAM)TEXT("Insert text here..."));

			SendMessage(hwndEditPath, WM_SETTEXT, NULL, (LPARAM)executablePath.c_str());
		

			MessageBox(hDlg, str, TEXT("Error"), MB_ICONEXCLAMATION);
			
			return TRUE;
		}

		case ID_SETTINGSOK :
		{
			//MessageBox(hwndGoto, TEXT("prova"), TEXT("Error"), MB_ICONEXCLAMATION);
			saveConfiguration();
			HWND activeWindow = GetActiveWindow();
			EndDialog(activeWindow, 0);
			return TRUE;
		}

		case ID_SETTINGSCANCEL:
		{
			HWND activeWindow = GetActiveWindow();
			EndDialog(activeWindow, 0);
			return TRUE;
		}

		return TRUE;
		}

	case WM_NOTIFY:
	{
		switch (LOWORD(wParam))
		{
		case IDC_LISTVIEW:
		{
			
			if (((LPNMHDR)lParam)->code == NM_CLICK)
			{
				HWND hwndListView = GetDlgItem(hDlg, IDC_LISTVIEW);

				int iPos = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
				if (iPos != -1) {
					// iPos is the index of a selected item
					// do whatever you want with it

					//std::wstring temp = L"ciao";

					//std::wstring temp = std::to_wstring(iPos);

					//std::wstring::c_str()
					//MessageBox(hDlg, temp.c_str(), L"Error", MB_ICONEXCLAMATION);
					// Get the next selected item
					//iPos = ListView_GetNextItem(hwndListView, iPos, LVNI_SELECTED);

					//aggiorno interfaccia
					wchar_t buffer[1024];

					std::wstring label = risultati[iPos].label;
					std::wstring desc  = risultati[iPos].desc;
					std::wstring caption = label + TEXT("\n")+desc;
					SetDlgItemText(hDlg, IDC_DETTAGLI, &caption[0]);
				
				}
			}
			else
				if (((LPNMHDR)lParam)->code == NM_DBLCLK)
				{

					HWND hwndListView = GetDlgItem(hDlg, IDC_LISTVIEW);
					int iPos = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);

					//devo trovare il magnet
					std::wstring magnetLink = getMagnet(risultati[iPos].url);

					//printf("wchar_t string: %ls \n", str);

					wchar_t* wc = const_cast<wchar_t*>(executablePath.c_str());
					wchar_t* wc2 = const_cast<wchar_t*>(magnetLink.c_str());

					//MessageBox(hDlg, L"Error creating window\nPop-Up", L"Error", MB_ICONEXCLAMATION);
					callProcess(wc, wc2, true);
				}			
		}
		}		
	}//END WM_NOTIFY

	}
	return FALSE;
}


