#include <string>

class ASCIICodes
{
public:
	ASCIICodes();
	~ASCIICodes();

	int getSize();

	std::string getCode(int i);
	std::string getSymbol(int i);

	

private:

	typedef struct {

		int dec;
		std::string code;
		std::string symbol;

	} ASCIICode;

	ASCIICode* codes[3];
	int size = 3;
};

