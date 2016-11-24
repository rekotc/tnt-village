#ifndef RISULTATO_H
#define RISULTATO_H

#include <string>

class Risultato                   // begin declaration of the class
{
public:                    // begin public section
	Risultato();     // constructor
	Risultato(int, std::string, std::string, std::string, std::string);
	~Risultato();                  // destructor

	//int GetAge() const;            // accessor function
	//void SetAge(int age);    // accessor function
	//void Meow();

	void setRisultato(int, std::string, std::string, std::string, std::string);

	void setId(int);
	void setIdTNT(std::string);
	void setLabel(std::string);
	void setDesc(std::string);
	void setUrl(std::string);
	void setMagnetUrl(std::string);
	void setLeech(std::string);
	void setSeed(std::string);
	void setSize(std::string);

	int getId();
	std::string getIdTNT();
	std::string getLabel();
	std::string getDesc();
	std::string getUrl();
	std::string getMagnetUrl();
	std::string getLeech();
	std::string getSeed();
	std::string getSize();

private:                   // begin private section
	int id;
	std::string idTNT;
	std::string label;
	std::string desc;
	std::string url;
	std::string magnetUrl;
	std::string leech;
	std::string seed;
	std::string size;

};
#endif