#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdexcept>

#include "log4cxx/logger.h"
#include "log4cxx/consoleappender.h"
#include "log4cxx/patternlayout.h"
#include "log4cxx/basicconfigurator.h"

#include "cli_parser.h"
#include "GLCM.h"

namespace po = boost::program_options;

int main( int argc, char** argv )
{
	log4cxx::BasicConfigurator::configure(
			log4cxx::AppenderPtr(new log4cxx::ConsoleAppender(
					log4cxx::LayoutPtr(new log4cxx::PatternLayout("\%-5p - [%c] - \%m\%n")),
					log4cxx::ConsoleAppender::getSystemErr()
					)
				)
			);

	log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("main"));
	CliParser cli_parser;
	try {
		if(cli_parser.parse_argv(argc, argv) != CliParser::CONTINUE)
			exit(0);
	} catch (CliException &err) {
		LOG4CXX_FATAL(logger, err.what());
		return -1;
	}

	cv::Mat img;

	// http://stackoverflow.com/a/4505166/1263626
	img = cv::imread(cli_parser.input_image(), CV_LOAD_IMAGE_GRAYSCALE);

	for(int i = 0; i < img.rows; i++) {
		for(int j = 0; j < img.cols; j++) {
			img.at<uchar>(i, j) = img.at<uchar>(i, j) * cli_parser.number_of_graylevels() / 256;
		}
	}

	/*
	std::vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	try {
		cv::imwrite("out.png", img, compression_params);
	}
	catch (std::runtime_error& ex) {
		std::cerr << "Exception converting image to PNG format:" << ex.what() << std::endl;
		return 1;
	}
	*/

	std::cout << img.cols << "x" << img.rows << std::endl;

	cv::Rect image_region(cv::Point(0, 0), img.size());
	GLCM<unsigned short> glcm(cli_parser.number_of_graylevels());

	Offset offset = cli_parser.offset();

	for(int j = 0; j < img.rows; j++) {
		for(int i = 0; i < img.cols; i++) {
			cv::Point p1, p2;
			p1.x = i - cli_parser.window_radius();
			if(p1.x < 0) p1.x = 0;

			p2.x = i + cli_parser.window_radius();
			if(p2.x >= img.cols) p2.x = img.cols - 1;

			p1.y = j - cli_parser.window_radius();
			if(p1.y < 0) p1.y = 0;

			p2.y = j + cli_parser.window_radius();
			if(p2.y >= img.rows) p2.y = img.rows - 1;

			if(offset[0] > 0)
				p2.x -= offset[0];
			else
				p1.x -= offset[0];

			if(offset[1] > 0)
				p2.y -= offset[1];
			else
				p1.y -= offset[1];

			//std::cout << p1.x << "x" << p1.y << " to " << p2.x << "x" << p2.y << std::endl;
			//continue;

			glcm.reset();

			for(int y = p1.y; y <= p2.y; ++y) {
				for(int x = p1.x; x <= p2.x; ++x) {
					//std::cout << x << "x" << y << ": " << (int)img.at<uchar>(y, x) << " --- " << (x + offset[0]) << "x" << (y + offset[1]) << ": " << (int)img.at<uchar>(y + offset[1], x + offset[0]) << std::endl;
					glcm.inc(img.at<uchar>(y, x), img.at<uchar>(y + offset[1], x + offset[0]));
				}
			}

			if(i == 31 && j == 31)
				glcm.print();
		}
	}

	return 0;
}
