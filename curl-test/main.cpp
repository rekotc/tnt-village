#include <stdio.h>
#include <sstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <fstream>

#include "include\curl\curl.h"
using namespace std;

typedef struct {

	int dec;
	std::string code;
	std::string symbol;

} ASCIICode;

ASCIICode codici[]
{

	{33,"&#33;","!"},
	{34,"&#34;","\"" },
	{39,"&#39;","\'" },

};

int countSubstring(const std::string& str, const std::string& sub)
{
	if (sub.length() == 0) return 0;
	int count = 0;
	for (size_t offset = str.find(sub); offset != std::string::npos;
		offset = str.find(sub, offset + sub.length()))
	{
		++count;
	}
	return count;
}

std::string replace(std::string str, std::string search, std::string replace)
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

//clean up function

void cleanUp() {


}


static void
print_cookies(CURL *curl)
{
	CURLcode res;
	struct curl_slist *cookies;
	struct curl_slist *nc;
	int i;

	printf("Cookies, curl knows:\n");
	res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
	if (res != CURLE_OK) {
		fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
			curl_easy_strerror(res));
		exit(1);
	}
	nc = cookies, i = 1;
	while (nc) {
		printf("[%d]: %s\n", i, nc->data);
		nc = nc->next;
		i++;
	}
	if (i == 1) {
		printf("(none)\n");
	}
	curl_slist_free_all(cookies);
}


void view_debug(const char* pszFileName, const char* pszParameters)
{
	//ShellExecuteA(GetDesktopWindow(), "open", pszFileName, NULL, NULL, SW_SHOW);
	int nRet = (int)ShellExecuteA(0, "open", pszFileName, pszParameters, 0, SW_SHOWNORMAL);
	if (nRet <= 32) {
		DWORD dw = GetLastError();
		char szMsg[250];
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM,
			0, dw, 0,
			(LPWSTR)szMsg, sizeof(szMsg),
			NULL
		);
		MessageBox(
			NULL,
			(LPCWSTR)szMsg,
			(LPCWSTR)L"Account Details",
			MB_ICONWARNING | MB_DEFBUTTON2
		);
	
	}


}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

//recupera la magnet string per un dato url
string getMagnetUrl(string u, string par) {
	
	CURL *curlMagnet;
	CURLcode res;
	string content;
	string magnetUrl;
	//static const char urlPage = u.c_str();

	curlMagnet = curl_easy_init();
	if (curlMagnet) {
		curl_easy_setopt(curlMagnet, CURLOPT_URL, u.c_str());
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curlMagnet, CURLOPT_FOLLOWLOCATION, 1L);
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, par);
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));
		curl_easy_setopt(curlMagnet, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curlMagnet, CURLOPT_WRITEDATA, &content);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curlMagnet);
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curlMagnet);
	}

	//ora cerco la stringa magnet dentro la pagina
	//cerco l'url del torrente
	int magnetStartPos	= content.find("magnet:?");
	content = content.substr(magnetStartPos);
	int magnetEndPos = content.find("'");
	magnetUrl = content.substr(0, magnetEndPos);

	return magnetUrl;

}

typedef struct
{
	int id;
	string idTNT;
	string label;
	string url;
	string magnetUrl;

} Risultato;


int main(int argc, char* argv[])
{

	std::string username = "";
	std::string password = "";
	std::string search = "";
	std::string type = "";

	/*
	
	std::cout << argv[0] << std::endl;

	// Check the number of parameters
	//servono 4 parametri al programma, più il parametro 0 che è il nome del programma stesso (default)
	if (argc < 5) {
		// Tell the user how to run the program
		std::cerr << "Utilizzo: " << argv[0] << " -u <USERNAME> -p <PASSWORD> -s <RICERCA> -t <TIPOFILE>" << std::endl;
		std::cerr << "Esempio: " << argv[0] << " -u pippo84 -p 1984pippo -s \"matrix evolution\" -t film" << std::endl;
		std::cerr << "I valori validi per il TIPOFILE sono: tutti,film" << std::endl;
		// "Usage messages" are a conventional way of telling the user
		// how to run a program if they enter the command incorrectly.
		
		return 1;
	}
	
	//faccio il parsing degli argomenti
	for (int i = 1; i < argc; ++i) {
		if (	(std::string(argv[i]) == "-u") || (std::string(argv[i]) == "-username")	)
		{
			if (i + 1 < argc)
			{ // Make sure we aren't at the end of argv!
				username = argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
			}
			else
			{ // Uh-oh, there was no argument to the destination option.
				std::cerr << "-username option requires one argument." << std::endl;
				return 1;
			}
		}
		else
			if ((std::string(argv[i]) == "-p") || (std::string(argv[i]) == "-password"))
			{
				if (i + 1 < argc)
				{ // Make sure we aren't at the end of argv!
					password = argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
				}
				else
				{ // Uh-oh, there was no argument to the destination option.
					std::cerr << "-password option requires one argument." << std::endl;
					return 1;
				}
			}
			else
				if ((std::string(argv[i]) == "-s") || (std::string(argv[i]) == "-search"))
				{
					if (i + 1 < argc)
					{ // Make sure we aren't at the end of argv!
						search = argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
					}
					else
					{ // Uh-oh, there was no argument to the destination option.
						std::cerr << "-search option requires one argument." << std::endl;
						return 1;
					}
				}
				else
					if ((std::string(argv[i]) == "-t") || (std::string(argv[i]) == "-type"))
					{
						if (i + 1 < argc)
						{ // Make sure we aren't at the end of argv!
							type = argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
						}
						else
						{ // Uh-oh, there was no argument to the destination option.
							std::cerr << "-type option requires one argument." << std::endl;
							return 1;
						}
					}

	}
	
	*/
	
	
	username = "rekotc";
	password = "superuserdo87";
	//search = "Il Pasto Nudo";
	type = "Tutti";
	search = "prova";
	

	//Stampa un riepilogo della ricerca inserita
	std::cout <<"Inizio la ricerca dell'utente: " << username << " per il termine " << search << " di tipo " << type << std::endl;

	//CURL CODE
	
	CURL *curl,*curl2;
	CURLcode res;
	std::string htmlPage;



	string post = "referer=http%3A%2F%2Fforum.tntvillage.scambioetico.org%2Findex.php%3F&UserName=" + username + "&PassWord="+password+"&CookieDate=1";
	static const char *postthis = post.c_str();

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (curl) {
		char nline[256];
		
		curl_easy_setopt(curl, CURLOPT_URL, "http://forum.tntvillage.scambioetico.org/index.php?act=Login&CODE=01");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); // start cookie engine
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
			return 1;
		}

		print_cookies(curl);

		//printf("Erasing curl's knowledge of cookies!\n");
		//curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
		//print_cookies(curl);

		//ora provo a fare una ricerca sul forum
		post = "sb=0&sd=0&cat=0&releaser=&stn=20&releaser=&filter=" + search + "&set=Imposta+filtro";
		postthis = post.c_str();

		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, "http://forum.tntvillage.scambioetico.org/index.php?act=allreleases");
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlPage);

			res = curl_easy_perform(curl);

			if (res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

			curl_easy_cleanup(curl);
		}

		//htmlPage cleanUp()
		for (int i=0; i<ARRAYSIZE(codici);i++)
		{
			htmlPage = replace(htmlPage, codici[i].code, codici[i].symbol);

		}
		//int data2len;

		//const char* prova = htmlPage.c_str();
		//std::string test = "prova&#39;prova";

		//elimino la formattazione URL-encoded di content
		//test = curl_easy_unescape(curl, test.c_str(), 0, &data2len);
		
		Risultato risultati[50];

		//std::cout << content << std::endl;

		string torrentTableTagStart = "<!--TORRENT TABLE-->";
		string torrentTableTagEnd = "<!--END TORRENT TABLE-->";
		std::size_t torrentTableTagStartPos, torrentTableTagEndPos;

		torrentTableTagStartPos = htmlPage.find(torrentTableTagStart);
		string tabella = htmlPage.substr(torrentTableTagStartPos+20);
		torrentTableTagEndPos = tabella.find(torrentTableTagEnd);
		tabella = tabella.substr(0, torrentTableTagEndPos);
		int numOfResults;

		if (tabella.length()!=0)
		{
			numOfResults = countSubstring(tabella,"PeerLists");

			//definisco il tag href
			string hrefTag = "href='";
			//definisco i tag di apertura e chiusura che utilizzo per individuare i risultati dalla tabella
			string beginSplitTag = "'>";
			string endSplitTag = "</a>";
					
			std::size_t idTNTStartPos;
			std::size_t hrefTagPos;
			std::size_t beginSplitTagPos;
			std::size_t endSplitTagPos;

			for (int i = 0; i < numOfResults; i++) {


				risultati[i].id = i;
				//cerco il tag href
				hrefTagPos = tabella.find(hrefTag);
				//cerco l'url del torrent
				tabella = tabella.substr(hrefTagPos + 6);
				beginSplitTagPos = tabella.find(beginSplitTag);
				//splitto l'url che ho ricavato grazie ai find, e lo salvo nei risultati
				risultati[i].url = tabella.substr(0, beginSplitTagPos);

				//cerco l'IDTNT a partire dall'url appena ricavato
				idTNTStartPos = (risultati[i].url).find("=");
				//lo splitto, poi lo converto in integer per poterlo salvare nei risultati
				risultati[i].idTNT = (risultati[i].url).substr(idTNTStartPos + 1);

				//ora cerco il magnet url
				//risultati[i].magnetUrl = getMagnetUrl(risultati[i].url,risultati[i].idTNT);

				//torno a lavorare sulla tabella, la splitto in corrispondenza di beginSplit
				tabella = tabella.substr(beginSplitTagPos + 2);
				//vado a prendere l'indice del tag di chiusura key2
				endSplitTagPos = tabella.find(endSplitTag);
				//salvo la stringa ottenuta
				risultati[i].label = tabella.substr(0, endSplitTagPos);



				//aggiorno anche la tabella, spostandomi oltre il risultato appena salvato
				tabella = tabella.substr(endSplitTagPos + 2);

				//ci sono 13 ulteriori '> prima del risultato successivo, mi sposto avanti subito
				for (int j = 0; j < 13; j++) {

					beginSplitTagPos = tabella.find(beginSplitTag);
					tabella = tabella.substr(beginSplitTagPos + 2);
				}

			}			
			
			std::cout << "RISULTATI:\n";
			for (int i = 0; i < numOfResults; i++)
			{
				std::cout << " - " << risultati[i].label << " ---- " << risultati[i].url<< " ---- "<< risultati[i].idTNT << " ---- " << risultati[i].magnetUrl <<'\n';
			}

			//cancello il file dei risultati esistente, se esiste
			remove("C:\\Users\\Re Kotc\\development\\c++\\progetti\\curl-test\\tnt-village-gui\\risultati.txt");
			//salvataggio risultati
			ofstream fileRisultati;
			fileRisultati.open("C:\\Users\\Re Kotc\\development\\c++\\progetti\\curl-test\\tnt-village-gui\\risultati.txt", ios::out | ios::trunc);

			//salvo in cima il numero di risultati trovati
			fileRisultati << numOfResults << "\n";

			for (int i = 0; i < numOfResults; i++)
			{
				fileRisultati << risultati[i].id << "\n";
				fileRisultati << risultati[i].idTNT << "\n";
				fileRisultati << risultati[i].label << "\n";
				fileRisultati << risultati[i].url << "\n";
				//il magnet viene caricato separatamente
				fileRisultati << "" << "\n";
				
			}
			
			fileRisultati.close();


		}
		else
		{
			std::cout << "non ho trovato un cazzo\n";
		}

		
	}
	else {
		fprintf(stderr, "Curl init failed!\n");
		return 1;
	}

	curl_global_cleanup();

	//view_debug("C:\\Program Files\\Windows NT\\Accessories\\wordpad.exe");
	//view_debug("C:\\Users\\Re Kotc\\development\\c++\\progetti\\curl-test\\Debug\\curl.exe", "http://www.google.it -o output.txt");
	
	return 0;
}