#include "boost_program_options_types.h"
#include "ParseUtils.h"

namespace po = boost::program_options;

void validate(boost::any& v, const std::vector<std::string>& values, PositiveInteger*, int)
{
	po::validators::check_first_occurrence(v);
	const std::string& s = po::validators::get_single_string(values);

	unsigned int value;
	if( ParseUtils::ParseUInt(value, s.data(), 10) && value >= 0 )
	{
		v = boost::any(PositiveInteger(value));
	} else {
		throw po::invalid_option_value(s);
	}
}

void validate(boost::any& v, const std::vector<std::string>& values, StrictlyPositiveInteger*, int)
{
	po::validators::check_first_occurrence(v);
	const std::string& s = po::validators::get_single_string(values);

	unsigned int value;
	if( ParseUtils::ParseUInt(value, s.data(), 10) && value > 0 )
	{
		v = boost::any(StrictlyPositiveInteger(value));
	} else {
		throw po::invalid_option_value(s);
	}
}

void validate(boost::any& v, const std::vector<std::string>& values, Float*, int)
{
	po::validators::check_first_occurrence(v);
	const std::string& s = po::validators::get_single_string(values);

	float value;
	if( ParseUtils::ParseFloat(value, s.data()) )
	{
		v = boost::any(Float(value));
	} else {
		throw po::invalid_option_value(s);
	}
}

void validate(boost::any& v, const std::vector<std::string>& values, Double*, int)
{
	po::validators::check_first_occurrence(v);
	const std::string& s = po::validators::get_single_string(values);

	double value;
	if( ParseUtils::ParseDouble(value, s.data()) )
	{
		v = boost::any(Double(value));
	} else {
		throw po::invalid_option_value(s);
	}
}

void validate(boost::any& v, const std::vector<std::string>& values, Percentage*, int)
{
	po::validators::check_first_occurrence(v);
	const std::string& s = po::validators::get_single_string(values);

	float value;
	if( ParseUtils::ParseFloat(value, s.data()) && (value >= 0.0f) && (value <= 1.0f) )
	{
		v = boost::any(Percentage(value));
	} else {
		throw po::invalid_option_value(s);
	}
}

void validate(boost::any& v, const std::vector<std::string>& values, UnsignedChar*, int)
{
	po::validators::check_first_occurrence(v);
	const std::string& s = po::validators::get_single_string(values);

	unsigned char value;
	if( ParseUtils::ParseUChar(value, s.data(), 10))
	{
		v = boost::any(UnsignedChar(value));
	} else {
		throw po::invalid_option_value(s);
	}
}

Offset::Offset() : m_offset(2)
{
	m_offset[0] = 0; m_offset[1] = 0;;
}

Offset::Offset(const int o1, const int o2) : m_offset(2)
{
	m_offset[0] = o1; m_offset[1] = o2;
}

std::vector< int > Offset::getOffset() { return m_offset; }

const int Offset::operator[](const unsigned int i) const { return m_offset[i]; }

void validate(boost::any& v, const std::vector<std::string>& values, Offset* target_type, int)
{
	static boost::regex r("(-?\\d+),(-?\\d+)");

	using namespace boost::program_options;

	// Make sure no previous assignment to 'v' was made.
	validators::check_first_occurrence(v);
	// Extract the first string from 'values'. If there is more than
	// one string, it's an error, and exception will be thrown.
	const std::string& s = validators::get_single_string(values);

	// Do regex match and convert the interesting part to int.
	boost::smatch match;
	if (boost::regex_match(s, match, r)) {
		v = boost::any(Offset(boost::lexical_cast<unsigned int>(match[1]), boost::lexical_cast<unsigned int>(match[2])));
	} else {
		throw invalid_option_value(s);
	}
}
