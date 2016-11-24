#ifndef TNTSEARCHDESC_H
#define TNTSEARCHDESC_H

#include <string>


class Tntsearchdesc                 // begin declaration of the class
{
public:                    // begin public section
	Tntsearchdesc();     // constructor
				   //Risultato(int, std::string, std::string, std::string, std::string);
	~Tntsearchdesc();                  // destructor


								 //int GetAge() const;            // accessor function
								 //void SetAge(int age);    // accessor function
								 //void Meow();
	void setTntParameters(std::string, std::string, std::string, std::string);

	void setTntUsername(std::string);
	void setTntPassword(std::string);
	void setTntStringsearch(std::string);
	void setTntTypesearch(std::string);

	std::string getTntUsername();
	std::string getTntPassword();
	std::string getTntStringsearch();
	std::string getTntTypesearch();
								 //void setRisultato(int, std::string, std::string, std::string, std::string);
	//void runTNTSearch(std::string, Risultato*);

private:                   // begin private section

						   //std::string search;
	std::string username;
	std::string password;
	std::string stringsearch;
	std::string typesearch;

};
#endif