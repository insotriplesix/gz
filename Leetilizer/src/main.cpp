#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <string>

/* Leet alphabet. */
const std::map<char, std::string> alphabet =
{
	{ 'a', "4" },{ 'b', "8" },{ 'c', "(" },
  	{ 'd', "[)" },{ 'e', "3" },{ 'f', "|=" },
	{ 'g', "6" },{ 'h', "|-|" },{ 'i', "!" },
  	{ 'j', "]" },{ 'k', "|<" },{ 'l', "1" },
	{ 'm', "|Y|" },{ 'n', "/\\/" },{ 'o', "0" },
  	{ 'p', "|>" },{ 'q', "0," },{ 'r', "|2" },
	{ 's', "5" },{ 't', "7" },{ 'u', "[_]" },
  	{ 'v', "\\/" },{ 'w', "\\v/" },{ 'x', "}{" },
	{ 'y', "`/" },{ 'z', "2" }
};

/* Takes a string of text and leetilize every symbol from A(a) to Z(z). */
std::string leetilize(std::string buf)
{
	std::string lbuf = "";
	auto nil = alphabet.end();

	/* Use length() - 1 cuz of EOF. */
	for (size_t i = 0; i < buf.length() - 1; ++i) {
		auto item = alphabet.find(buf[i]);
		if (item != nil)
			lbuf += item->second;
		else
			lbuf += buf[i];
	}

	return lbuf;
}

int main(int argc, char **argv)
{
	std::string filename;

	std::cout << "*** Leetilizer v1.0 ***" << std::endl;
	std::cout << "Input name of a file to make it leet: ";
	std::cin >> filename;

	std::ifstream fin;
	std::ofstream fout;

	std::string buf = "";
	std::string lbuf = "";

	/* Open your file. */
	fin.open(filename, std::ios::in);

	if (fin.is_open()) {
		/* Get all text from it. */
		while (!fin.eof())
			buf += fin.get();

		/* Transform the string to lowercase characters. */
		std::transform(buf.begin(), buf.end(), buf.begin(), ::tolower);

		/* Leetilized it. */
		std::cout << "Leetilizing . . ." << std::endl;
		lbuf = leetilize(buf);

		/* Open the output file. */
		fout.open("leetilized_" + filename, std::ios::out);

		if (fout.is_open()) {
			/* Drop the leetilized text to that file. */
			fout << lbuf;

			/* ??? PROFIT */
			std::cout << "Complete." << std::endl;
		}
		else {
			std::cout << "Error opening file '" << "leetilized_" + filename << "'" << std::endl;
		}	
	}
	else {
		std::cout << "Error opening file '" << filename << "'" << std::endl;
	}

	fin.close();
	fout.close();

	return 0;
}
