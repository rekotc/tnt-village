#include "risultato.h"



Risultato::Risultato()
{
	id = NULL;
	idTNT = "";
	label = "";
	desc = "";
	url = "";
	magnetUrl = "";
	leech = "";
	seed = "";
	size = "";

}

Risultato::Risultato(int i, std::string t, std::string l, std::string u, std::string m)
{
	id = NULL;
	idTNT = "";
	label = "";
	url = "";
	magnetUrl = "";

}

Risultato::~Risultato()
{
}

void Risultato::setRisultato(int i, std::string t, std::string l, std::string u, std::string m) {

	id = i;
	idTNT = t;
	label = l;
	url = u;
	magnetUrl = m;
	
}

void Risultato::setId(int i)
{
	id = i;
}
void Risultato::setIdTNT(std::string t)
{
	idTNT = t;
}
void Risultato::setLabel(std::string l)
{
	label = l;
	
}

void Risultato::setDesc(std::string d)
{
	desc = d;
}
void Risultato::setUrl(std::string u)
{
	url = u;
	
}
void Risultato::setMagnetUrl(std::string m)
{
	magnetUrl = m;
}

int Risultato::getId()
{
	return id;
}
std::string Risultato::getIdTNT()
{
	return idTNT;
}
std::string Risultato::getLabel()
{
	return label;
}

std::string Risultato::getDesc()
{
	return desc;
}

std::string Risultato::getUrl()
{
	return url;
}
std::string Risultato::getMagnetUrl()
{
	return magnetUrl;
}

void Risultato::setLeech(std::string l) {

	leech = l;
}

void Risultato::setSeed(std::string s) {
	
	seed = s;
}

void Risultato::setSize(std::string s) {

	size = s;
}

std::string Risultato::getLeech() {

	return leech;
}

std::string Risultato::getSeed() {

	return seed;
}

std::string Risultato::getSize() {

	return size;
}