#include "ParseUtils.h"

#include <cerrno>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

// Inspired by: http://stackoverflow.com/questions/194465/how-to-parse-a-string-to-an-int-in-c

bool ParseUtils::ParseUChar(unsigned char &value, char const *s, const int base)
{
	unsigned char v;

	try {
		v = boost::numeric_cast<unsigned char>(boost::lexical_cast<int>(s));
	} catch( boost::bad_lexical_cast &ex ) {
		return false;
	} catch(boost::numeric::bad_numeric_cast &ex) {
		return false;
	}

	value = v;
	return true;
}

bool ParseUtils::ParseInt(int &value, char const *s, const int base)
{
	int v;

	try {
		v = boost::lexical_cast< int >(s);
	} catch( boost::bad_lexical_cast &ex ) {
		return false;
	}

	value = v;
	return true;
}

bool ParseUtils::ParseUInt(unsigned int &value, char const *s, const int base)
{
	unsigned int v;

	try {
		v = boost::lexical_cast< unsigned int >(s);
	} catch( boost::bad_lexical_cast &ex ) {
		return false;
	}

	value = v;
	return true;
}

bool ParseUtils::ParseFloat(float &value, char const *s)
{
	float v;
	char *old_locale;

	try {
		v = boost::lexical_cast< float >(s);
	} catch( boost::bad_lexical_cast &ex ) {
		return false;
	}

	value = v;
	return true;
}

bool ParseUtils::ParseDouble(double &value, char const *s)
{
	double v;

	try {
		v = boost::lexical_cast< double >(s);
	} catch( boost::bad_lexical_cast &ex ) {
		return false;
	}

	value = v;
	return true;
}
