#ifndef TNTSEARCH_H
#define TNTSEARCH_H

//#include <iostream>
//#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include "include\curl\curl.h"

#include "risultato.h"
#include "tntsearchdesc.h"
#include "asciicodes.h"

#define NUM_RESULTS 500

class Tntsearch                  // begin declaration of the class
{
public:                    // begin public section

	
	Tntsearch();     // constructor
	//Risultato(int, std::string, std::string, std::string, std::string);
	~Tntsearch();                  // destructor

								   //int GetAge() const;            // accessor function
								   //void SetAge(int age);    // accessor function
								   //void Meow();

	//void setRisultato(int, std::string, std::string, std::string, std::string);
	void runTNTSearch(Tntsearchdesc*);
	void getMagnet(std::string);
	//static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

private:                   // begin private section
	
	ASCIICodes* codes;

	Risultato** risultati;
	Risultato* risP;
	Tntsearchdesc* tntdesc;
	//limite risultati NUM_RESULTS è hardcoded, non è una bella soluzione
	

	/*DIRECTORY*/
	const char *resultsDir = "C:\\Users\\Re Kotc\\development\\c++\\progetti\\curl-test\\tnt-village-CLI\\risultati.txt";
	const char *magnetDir = "C:\\Users\\Re Kotc\\development\\c++\\progetti\\curl-test\\tnt-village-CLI\\magnet.txt";

	//dovrebbero essere statici?
	const char *loginQUERY;
	const char *searchQUERY;	
	const char * tntvillageLoginUrl;
	const char * tntvillageSearchUrl;

	/*Qui di seguito sono definite le stringhe che utilizzo per splittare i risultati dalle pagine di TNT Village*/
	std::string torrentTableTagStart = "<!--TORRENT TABLE-->";
	std::string torrentTableTagEnd = "<!--END TORRENT TABLE-->";
	//definisco il tag href
	std::string hrefTag = "href='";
	//definisco i tag di apertura e chiusura che utilizzo per individuare i titoli nella tabella
	std::string beginSplitTag = "'>";
	std::string endSplitTag = "</a>";
	//definisco i tag di apertura e chiusura che utilizzo per individuare le descrizioni nella tabella
	std::string beginSplitDescTag = "'>";
	std::string endSplitDescTag = "</b>";
	//definisco i tag di chiusura che utilizzo per individuare il numero leech
	std::string endSplitLeechTag = "</span>";

	//questa variabile contiene il numero di risultati
	int numOfResults;

	
	std::string htmlPage;

	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

	void grabWebContent();
	void cleanPage();
	std::string replace(std::string, std::string, std::string);
	std::string getMagnetUrl(std::string);
	Risultato* extractTNTResults();
	void saveTNTResults();
	int countSubstring(const std::string&, const std::string&);

};
#endif