#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <boost/program_options.hpp>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <string>

#include "boost_program_options_types.h"

namespace po = boost::program_options;

class CliException : public std::runtime_error
{
public:
	CliException ( const std::string &err ) : std::runtime_error (err) {}
};

class CliParser
{
public:
	enum ParseResult {
		CONTINUE = 0,
		EXIT,
	};

	CliParser();

	/**
	 * Parses argv.
	 *
	 * \return A ParseResult if the arguments are successfully parsed.
	 *
	 * \throw A CliException if the arguments cannot be parsed.
	 */
	ParseResult parse_argv(int argc, char ** argv);

	const std::string input_image() const;
	const unsigned char number_of_graylevels() const;
	const unsigned int window_radius() const;
	Offset offset() const;


private:
	std::string m_InputImage;
	UnsignedChar m_NumberOfGraylevels;
	StrictlyPositiveInteger m_WindowRadius;
	Offset m_Offset;
};

#endif /* CLI_PARSER_H */
