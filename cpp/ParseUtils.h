#ifndef PARSEUTILS_H
#define PARSEUTILS_H

class ParseUtils
{
public:
	static bool ParseUChar(unsigned char &i, char const *s, const int base = 0);
	static bool ParseInt(int &i, char const *s, const int base = 0);
	static bool ParseUInt(unsigned int &i, char const *s, const int base = 0);
	static bool ParseFloat(float &i, char const *s);
	static bool ParseDouble(double &i, char const *s);
};

#endif /* PARSEUTILS_H */
