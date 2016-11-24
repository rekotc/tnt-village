#include "Tntsearchdesc.h"



Tntsearchdesc::Tntsearchdesc()
{
	username = "";
	password = "";
	stringsearch = "";
	typesearch = "";
}


Tntsearchdesc::~Tntsearchdesc()
{
}

void Tntsearchdesc::setTntParameters(std::string u, std::string p, std::string s, std::string t)
{

	username = u;
	password = p;
	stringsearch = s;
	typesearch = t;
}

void Tntsearchdesc::setTntUsername(std::string u )
{
	username = u;
}
void Tntsearchdesc::setTntPassword(std::string p)
{
	password = p;
}
void Tntsearchdesc::setTntStringsearch(std::string s)
{
	stringsearch = s;
}
void Tntsearchdesc::setTntTypesearch(std::string t)
{
	typesearch = t;
}

std::string Tntsearchdesc::getTntUsername()
{
	return username;
}
std::string Tntsearchdesc::getTntPassword()
{
	return password;
}
std::string Tntsearchdesc::getTntStringsearch()
{
	return stringsearch;
}
std::string Tntsearchdesc::getTntTypesearch()
{
	return typesearch;
}