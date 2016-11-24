#include "tntsearch.h"



Tntsearch::Tntsearch()
{
	codes = new ASCIICodes();
}


Tntsearch::~Tntsearch()
{
}

void Tntsearch::runTNTSearch(Tntsearchdesc* t)
{
	tntdesc = t;
	
	std::string tempstring1 = "http://forum.tntvillage.scambioetico.org/index.php?act=Login&CODE=01";
	std::string tempstring2 = "http://forum.tntvillage.scambioetico.org/index.php?act=allreleases";
	std::string tempstring3 = "referer=http%3A%2F%2Fforum.tntvillage.scambioetico.org%2Findex.php%3F&UserName=" + tntdesc->getTntUsername() + "&PassWord=" + tntdesc->getTntPassword() + "&CookieDate=1";
	std::string tempstring4 = "sb=0&sd=0&cat=0&releaser=&stn=20&releaser=&filter=" + tntdesc->getTntStringsearch() + "&set=Imposta+filtro";


	tntvillageLoginUrl = tempstring1.c_str();
	tntvillageSearchUrl = tempstring2.c_str();
	loginQUERY = tempstring3.c_str();
	searchQUERY = tempstring4.c_str();
	
	//questa funzione si occupa del login e dello scaricamento della pagina dei risultati, che viene poi salvata in htmlPage
	grabWebContent();

	cleanPage();

	//questa funzione si occupa di estrarre i dati utili dalla pagina htmlPage
	risP = extractTNTResults();
	risultati = &risP;
	//questa funzione si occupa di salvare i risultati su file
	if(risultati!=NULL)
		saveTNTResults();

}

//questa funzione si occupa del login e dello scaricamento della pagina dei risultati, che viene poi salvata in htmlPage
void Tntsearch::grabWebContent()
{
	//oggetti necessari per usufruire della libreria libcurl, utilizzata per effettuare le richieste HTTP POST al server
	//di TNTVillage
	CURL *curl;
	//contiene eventuali codici d'errore
	CURLcode res;

	//In questa prima fase devo eseguire il login dell'utente, all'indirizzo tntvillageLoginUrl
	/*L'opzione CURLOPT_COOKIEFILE serve per mantenere i cookie in seguito alla ricezione del SessionID da parte del
	  server di TNTVIllage, il SessionID mi serve perché, per poter effettuare ricerche sul server, devo risultare loggato
	*/
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (curl)
	{
		char nline[256];

		curl_easy_setopt(curl, CURLOPT_URL, tntvillageLoginUrl);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, loginQUERY);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(loginQUERY));
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); // start cookie engine
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
			//return 1;
		}
	}

	//procedo ad effettuare la ricerca, al termine di questa operazione la stringa htmlPage conterrà l'intera pagina
	//dei risultati, da cui posso procedere con l'estrazione dei dati utili, tramite la funzione extractTNTResults()

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, tntvillageSearchUrl);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, searchQUERY);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(searchQUERY));
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlPage);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		curl_easy_cleanup(curl);
	}
}

//questa funzione si occupa di estrarre i dati utili dalla pagina htmlPage
Risultato* Tntsearch::extractTNTResults()
{
	static Risultato*   risP;
	//Risultato**  ris = &risP;
	
	std::size_t torrentTableTagStartPos, torrentTableTagEndPos;

	torrentTableTagStartPos = htmlPage.find(torrentTableTagStart);
	std::string tabella = htmlPage.substr(torrentTableTagStartPos + 20);
	torrentTableTagEndPos = tabella.find(torrentTableTagEnd);
	tabella = tabella.substr(0, torrentTableTagEndPos);	

	if (tabella.length() != 0)
	{
		//nella pagina HTML dei risultati il numero di stringhe "PeerLists" presenti è pari al numero dei risultati trovati
		//conto allora quanti "PeerLists" compaiono nella pagina HTML e salvo questa informazione.
		numOfResults = countSubstring(tabella, "PeerLists");

		risP = new Risultato[numOfResults];
		
		std::size_t idTNTStartPos;
		std::size_t hrefTagPos;
		std::size_t beginSplitTagPos;
		std::size_t endSplitTagPos;
		
		for (int i = 0; i < numOfResults; i++) {

			//inizializzo l'oggetto che andrà a contenere i dati dell'i-esimo risultato trovato
			//risultati[i] = new Risultato();
			risP->setId(i);
			//cerco il tag href
			hrefTagPos = tabella.find(hrefTag);
			//cerco l'url del torrent
			tabella = tabella.substr(hrefTagPos + 6);
			beginSplitTagPos = tabella.find(beginSplitTag);
			//splitto l'url che ho ricavato grazie ai find, e lo salvo nei risultati
			risP->setUrl(tabella.substr(0, beginSplitTagPos));
			
			//cerco l'IDTNT a partire dall'url appena ricavato
			idTNTStartPos = risP->getUrl().find("=");
			//lo splitto, poi lo converto in integer per poterlo salvare nei risultati
			risP->setIdTNT(	(risP->getUrl()).substr(idTNTStartPos + 1)	);

			//ora cerco il magnet url
			//risultati[i].magnetUrl = getMagnetUrl(risultati[i].url,risultati[i].idTNT);

			//torno a lavorare sulla tabella, la splitto in corrispondenza di beginSplit
			
			//SPLIT DEL TITOLO
			tabella = tabella.substr(beginSplitTagPos + 2);
			//vado a prendere l'indice del tag di chiusura key2
			endSplitTagPos = tabella.find(endSplitTag);
			//salvo la stringa ottenuta
			risP->setLabel(tabella.substr(0, endSplitTagPos));
			//aggiorno anche la tabella, spostandomi oltre il risultato appena salvato
			tabella = tabella.substr(endSplitTagPos + 2);

			risP->setDesc("");
			
			//SPLIT DELLA DESCRIZIONE
			beginSplitTagPos = tabella.find(beginSplitDescTag);
			tabella = tabella.substr(beginSplitTagPos + 2);
			//vado a prendere l'indice del tag di chiusura key2
			endSplitTagPos = tabella.find(endSplitDescTag);
			std::string temp = tabella.substr(0, endSplitTagPos);
			//salvo la stringa ottenuta
			risP->setDesc(temp);
			//aggiorno anche la tabella, spostandomi oltre il risultato appena salvato
			tabella = tabella.substr(endSplitTagPos + 2);
			

			//ci sono 4 ulteriori '> prima del dato utile successivo, mi sposto avanti subito
			for (int j = 0; j < 4; j++) {

				beginSplitTagPos = tabella.find(beginSplitTag);
				tabella = tabella.substr(beginSplitTagPos + 2);
			}

			//SPLIT DEL LEECH
			//vado a prendere l'indice del tag di chiusura key2
			endSplitTagPos = tabella.find(endSplitLeechTag);
			std::string leech = tabella.substr(0, endSplitTagPos);
			//salvo la stringa ottenuta
			risP->setLeech(leech);
			//aggiorno anche la tabella, spostandomi oltre il risultato appena salvato
			tabella = tabella.substr(endSplitTagPos+7);

			//ci sono 1 ulteriori '> prima del dato utile successivo, mi sposto avanti subito
			for (int j = 0; j < 1; j++) {

				beginSplitTagPos = tabella.find(beginSplitTag);
				tabella = tabella.substr(beginSplitTagPos + 2);
			}

			// SPLIT DEL SEED
			beginSplitTagPos = tabella.find(beginSplitTag);
			tabella = tabella.substr(beginSplitTagPos + 2);
			//vado a prendere l'indice del tag di chiusura key2
			endSplitTagPos = tabella.find(endSplitLeechTag);
			std::string seed = tabella.substr(0, endSplitTagPos);
			//salvo la stringa ottenuta
			risP->setSeed(seed);
			//aggiorno anche la tabella, spostandomi oltre il risultato appena salvato
			tabella = tabella.substr(endSplitTagPos + 7);

			//ci sono 3 ulteriori '> prima del dato utile successivo, mi sposto avanti subito
			for (int j = 0; j < 3; j++) {

				beginSplitTagPos = tabella.find(beginSplitTag);
				tabella = tabella.substr(beginSplitTagPos + 2);
			}

			// SPLIT DEL SIZE
			beginSplitTagPos = tabella.find(beginSplitTag);
			tabella = tabella.substr(beginSplitTagPos + 2);
			//vado a prendere l'indice del tag di chiusura key2
			endSplitTagPos = tabella.find(endSplitLeechTag);
			std::string size = tabella.substr(0, endSplitTagPos);
			//salvo la stringa ottenuta
			risP->setSize(size);
			//aggiorno anche la tabella, spostandomi oltre il risultato appena salvato
			tabella = tabella.substr(endSplitTagPos + 7);

			//ci sono 2 ulteriori '> prima del risultato successivo, mi sposto avanti subito
			for (int j = 0; j < 2; j++) {

				beginSplitTagPos = tabella.find(beginSplitTag);
				tabella = tabella.substr(beginSplitTagPos + 2);
			}

			//incremento il puntatore
			risP = risP + 1;

		}

		//risP = NULL;
		risP = risP - (numOfResults);

	}
	else
	{//se non ho trovato risultati inizializzo comunque a NULL
		risP = NULL;
	}

	 
	return risP;
}
	
size_t Tntsearch::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

/*funzioni di utilità*/

int Tntsearch::countSubstring(const std::string& str, const std::string& sub)
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

//questa funzione si occupa di salvare i risultati su file
void Tntsearch::saveTNTResults() {

	//cancello il file dei risultati esistente, se esiste
	remove(resultsDir);
	//apro un nuovo file risultati
	std::ofstream fileRisultati;
	fileRisultati.open(resultsDir, std::ios::out | std::ios::trunc);

	if (fileRisultati.is_open())
	{
		//salvo alla prima riga il numero di risultati trovati, il valore viene poi utilizzato da tnt-village-gui
		fileRisultati << numOfResults << "\n";

		for (int i = 0; i < numOfResults; i++)
		{
			fileRisultati << (*risultati)->getId() << "\n";
			fileRisultati << (*risultati)->getIdTNT() << "\n";
			fileRisultati << (*risultati)->getLabel() << "\n";
			fileRisultati << (*risultati)->getDesc() << "\n";
			fileRisultati << (*risultati)->getUrl() << "\n";
			fileRisultati << (*risultati)->getLeech() << "\n";
			fileRisultati << (*risultati)->getSeed() << "\n";
			fileRisultati << (*risultati)->getSize() << "\n";
			
			risP = risP + 1;

		}

		fileRisultati.close();
	}
	else
	{
		//	fprintf(stderr, "vaffanculo: %s\n", curl_easy_strerror(res));
	}

}

void Tntsearch::cleanPage()
{
		for (int i = 0; i < codes->getSize(); i++)
		{
			htmlPage = replace(htmlPage, codes->getCode(i), codes->getSymbol(i));

		}
}

std::string Tntsearch::replace(std::string str, std::string search, std::string replace)
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

//recupera la magnet string per un dato url
std::string Tntsearch::getMagnetUrl(std::string url) {

	CURL *curlMagnet;
	CURLcode res;
	std::string content;
	std::string magnetUrl;
	//static const char urlPage = u.c_str();

	curlMagnet = curl_easy_init();
	if (curlMagnet) {
		curl_easy_setopt(curlMagnet, CURLOPT_URL, url.c_str());
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
	int magnetStartPos = content.find("magnet:?");
	content = content.substr(magnetStartPos);
	int magnetEndPos = content.find("'");
	magnetUrl = content.substr(0, magnetEndPos);

	return magnetUrl;

}

void Tntsearch::getMagnet(std::string url)
{
	
	std::string magnetUrl = getMagnetUrl(url);
	std::cout << magnetUrl << std::endl;
	//cancello il file dei risultati esistente, se esiste
	remove(resultsDir);
	//apro un nuovo file risultati
	std::ofstream fileRisultati;
	fileRisultati.open(magnetDir, std::ios::out | std::ios::trunc);

	if (fileRisultati.is_open())
	{
		//salvo alla prima riga il numero di risultati trovati, il valore viene poi utilizzato da tnt-village-gui
		fileRisultati << magnetUrl << "\n";

		fileRisultati.close();
	}
	
}