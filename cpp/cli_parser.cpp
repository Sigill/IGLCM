#include "cli_parser.h"

#include <iostream>
#include <sstream>
#include <vector>

CliParser::CliParser()
{}

CliParser::ParseResult CliParser::parse_argv(int argc, char ** argv)
{
	po::options_description desc("Command line parameters");
	desc.add_options()
		("help,h",
		 "Produce help message.")
		("input-image,i",
		 po::value< std::string >(&(this->m_InputImage))->required(),
		 "Input image.")
		("graylevels,g",
		 po::value< UnsignedChar >(&(this->m_NumberOfGraylevels))->default_value(16),
		 "Number of graylevels used.")
		("window,w",
		 po::value< StrictlyPositiveInteger >(&(this->m_WindowRadius))->default_value(5),
		 "Radius of the window.")
		("offset,o",
		 po::value< Offset >(&this->m_Offset)->required(),
		 "Offset (required)")
		;

	po::variables_map vm;

	try {
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);

		// Handling --help before notify() in order to allow ->required()
		// http://stackoverflow.com/questions/5395503/required-and-optional-arguments-using-boost-library-program-options#answer-5517755
		if (vm.count("help")) {
			std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
			std::cout << desc;
			return EXIT;
		}

		po::notify(vm);
	} catch(po::error &err) {
		throw CliException(err.what());
	}

	return CONTINUE;
}

const std::string CliParser::input_image() const {
	return this->m_InputImage;
}

const unsigned char CliParser::number_of_graylevels() const {
	return this->m_NumberOfGraylevels;
}

const unsigned int CliParser::window_radius() const {
	return this->m_WindowRadius;
}

Offset CliParser::offset() const {
	return this->m_Offset;
}
