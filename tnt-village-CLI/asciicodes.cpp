#include "asciicodes.h"



ASCIICodes::ASCIICodes()
{
	codes[0] = new ASCIICode({ 33,"&#33;","!" });
	codes[1] = new ASCIICode({ 34, "&#34;", "\"" });
	codes[2] = new ASCIICode({ 39, "&#39;", "\'" });

}


ASCIICodes::~ASCIICodes()
{
}

int ASCIICodes::getSize()
{
	return size;
}

std::string ASCIICodes::getCode(int i)
{
	return codes[i]->code;
}

std::string ASCIICodes::getSymbol(int i)
{
	return codes[i]->symbol;
}

