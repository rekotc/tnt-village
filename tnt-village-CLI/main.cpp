//include di sistema
#include <iostream>

//include locali
#include "tntsearch.h"
#include "tntsearchdesc.h"

int main(int argc, char* argv[])
{
	//queste variabili conterranno i parametri di login e di ricerca
	
	//inizializzo l'oggetto che utilizzerò per effettuare l'intera ricerca
	Tntsearch* tntsearch = new Tntsearch();
	//inizializzo l'oggetto che conterrà i parametri di login e ricerca
	Tntsearchdesc* tntdesc = new Tntsearchdesc();
	
	//se voglio scaricare un magnet direttamente
	///*
	if ((std::string(argv[1]) == "-m") )
	{

		std::cout << "scarico il magnet" << std::endl;
		tntsearch->getMagnet(argv[2]);

		return 1;
	}
	//altrimenti faccio una ricerca
	else
	{
		//faccio il parsing degli argomenti per riempire l'oggetto tntparam
		for (int i = 1; i < argc; ++i) {
			if ((std::string(argv[i]) == "-u") || (std::string(argv[i]) == "-username"))
			{
				if (i + 1 < argc)
				{ // Make sure we aren't at the end of argv!
					tntdesc->setTntUsername(argv[++i]); // Increment 'i' so we don't get the argument as the next argv[i].
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
						tntdesc->setTntPassword(argv[++i]); // Increment 'i' so we don't get the argument as the next argv[i].
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
							tntdesc->setTntStringsearch(argv[++i]); // Increment 'i' so we don't get the argument as the next argv[i].
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
								tntdesc->setTntTypesearch(argv[++i]); // Increment 'i' so we don't get the argument as the next argv[i].
							}
							else
							{ // Uh-oh, there was no argument to the destination option.
								std::cerr << "-type option requires one argument." << std::endl;
								return 1;
							}
						}

		}
	}
	
	//*/

	//TEMP
	/*
	tntdesc->setTntUsername("rekotc");
	tntdesc->setTntPassword("superuserdo87");
	tntdesc->setTntStringsearch("ocean's eleven");
	tntdesc->setTntTypesearch("tutti");
	*/

	//avvio la ricerca sulla base dei parametri inseriti
	tntsearch->runTNTSearch(tntdesc);
	

	return 0;
}