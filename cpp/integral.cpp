#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "log4cxx/logger.h"
#include "log4cxx/consoleappender.h"
#include "log4cxx/patternlayout.h"
#include "log4cxx/basicconfigurator.h"

#include "cli_parser.h"
#include "GLCM.h"

namespace po = boost::program_options;

void copy(unsigned short *out, unsigned short *in, const unsigned int len)
{
	for(int i = 0; i < len; ++i)
		out[i] = in[i];
}

void ppm(unsigned short *out, unsigned short *in1, unsigned short *in2, unsigned short *in3, const unsigned int len)
{
	for(int i = 0; i < len; ++i)
		out[i] = in1[i] + in2[i] - in3[i];
}

void pm(unsigned short *out, unsigned short *in1, unsigned short *in2, const unsigned int len)
{
	for(int i = 0; i < len; ++i)
		out[i] = in1[i] - in2[i];
}

void pmmp(unsigned short *out, unsigned short *in1, unsigned short *in2, unsigned short *in3, unsigned short *in4, const unsigned int len)
{
	for(int i = 0; i < len; ++i) {
		out[i] = in1[i] + in4[i] - in2[i] - in3[i];
		//std::cout << in1[i] << "-" << in2[i] << "-" << in3[i] << "+" << in4[i] << std::endl;
	}
}

void print_matrix(const unsigned short* in, const unsigned int size)
{
	int i, j;
	for(i = 0; i < size; ++i) {
		for(j = 0; j < size; ++j) {
			std::cout << in[j * size + i]  << " ";
		}
		std::cout << std::endl;
	}
}

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

	std::cout << img.cols << "x" << img.rows << std::endl;

	cv::Rect image_region(cv::Point(0, 0), img.size());

	const unsigned int matrix_size = cli_parser.number_of_graylevels() * cli_parser.number_of_graylevels();

	unsigned int **matrix_offsets = new unsigned int*[cli_parser.number_of_graylevels()];
	matrix_offsets[0] = new unsigned int[matrix_size];

	for(int i = 1; i < cli_parser.number_of_graylevels(); ++i)
		matrix_offsets[i] = matrix_offsets[0] + i * cli_parser.number_of_graylevels();

	for(int i = 0; i < matrix_size; ++i) matrix_offsets[0][i] = i;

	unsigned short* data = new unsigned short[img.cols * img.rows * matrix_size];

	unsigned short ***integral_image = new unsigned short **[img.cols];
	integral_image[0] = new unsigned short *[img.cols * img.rows];
	for(int i = 1; i < img.cols; ++i) {
		integral_image[i] = integral_image[0] + i * img.rows;
	}

	int k = 0;
	for(int j = 0; j < img.rows; j++) {
		for(int i = 0; i < img.cols; i++) {
			integral_image[i][j] = &data[k];
			k += matrix_size;
		}
	}

	Offset offset = cli_parser.offset();
	cv::Point o1(-std::max(offset[0], 0), -std::max(offset[1], 0));
	cv::Point o2(std::min(offset[0], 0), std::min(offset[1], 0));

	std::fill(data, data + matrix_size, 0);

	cv::Point p, p1, p2;

	p.y = 0;
	for(int i = 1; i < img.cols; i++) {
		p.x = i;
		p1 = p + o1;
		p2 = p + o2;

		copy(integral_image[i][0], integral_image[i-1][0], matrix_size);

		if(image_region.contains(p1) && image_region.contains(p2))
			++integral_image[i][0][matrix_offsets[img.at<uchar>(p1)][img.at<uchar>(p2)]];

		/*
		std::cout << p << std::endl;
		print_matrix(integral_image[i][0], cli_parser.number_of_graylevels());
		std::cout << std::endl;
		*/
	}

	p.x = 0;
	for(int j = 1; j < img.rows; j++) {
		p.y = j;
		p1 = p + o1;
		p2 = p + o2;

		copy(integral_image[0][j], integral_image[0][j-1], matrix_size);

		if(image_region.contains(p1) && image_region.contains(p2))
			++integral_image[0][j][matrix_offsets[img.at<uchar>(p1)][img.at<uchar>(p2)]];

		/*
		std::cout << p << std::endl;
		print_matrix(integral_image[0][j], cli_parser.number_of_graylevels());
		std::cout << std::endl;
		*/
	}

	for(int j = 1; j < img.rows; j++) {
		for(int i = 1; i < img.cols; i++) {
			p.x = i;
			p.y = j;

			p1 = p + o1;
			p2 = p + o2;

			ppm(integral_image[i][j], integral_image[i][j-1], integral_image[i-1][j], integral_image[i-1][j-1], matrix_size);
			if(image_region.contains(p1) && image_region.contains(p2))
				++integral_image[i][j][matrix_offsets[img.at<uchar>(p1)][img.at<uchar>(p2)]];

			/*
			std::cout << p << " (" << p1 << "; " << p2 << ") = " << (int)img.at<uchar>(p1) << ", " << (int)img.at<uchar>(p2) << std::endl;
			print_matrix(integral_image[i][j], cli_parser.number_of_graylevels());
			std::cout << std::endl;
			*/

			//if(i == 3 && j == 3)
			//	print_matrix(integral_image[i][j], cli_parser.number_of_graylevels());
		}
	}

	unsigned short *coocmat = new unsigned short[matrix_size];

	for(int j = 0; j < img.rows; j++) {
		for(int i = 0; i < img.cols; i++) {
			cv::Point p1, p2;
			p1.x = i - cli_parser.window_radius() - 1;
			//if(p1.x < 0) p1.x = 0;

			p1.y = j - cli_parser.window_radius() - 1;
			//if(p1.y < 0) p1.y = 0;

			p2.x = i + cli_parser.window_radius();
			if(p2.x >= img.cols) p2.x = img.cols - 1;

			p2.y = j + cli_parser.window_radius();
			if(p2.y >= img.rows) p2.y = img.rows - 1;

			cv::Point pp1(p1);
			p1.x += offset[0] < 0 ? -offset[0] : offset[0];
			p1.y += offset[1] < 0 ? -offset[1] : offset[1];

			if(pp1.x < 0 && pp1.y < 0) {
				copy(coocmat, integral_image[p2.x][p2.y], matrix_size);
			} else if(pp1.x < 0) {
				pm(coocmat, integral_image[p2.x][p2.y], integral_image[p2.x][p1.y], matrix_size);
			} else if(pp1.y < 0) {
				pm(coocmat, integral_image[p2.x][p2.y], integral_image[p1.x][p2.y], matrix_size);
			} else {
				pmmp(coocmat, integral_image[p2.x][p2.y],
					 integral_image[p2.x][p1.y],
					 integral_image[p1.x][p2.y],
					 integral_image[p1.x][p1.y],
					 matrix_size);
			}

			if(i == 31 && j == 31)
				print_matrix(coocmat, cli_parser.number_of_graylevels());
		}
	}

	delete[] coocmat;
	delete[] integral_image[0];
	delete[] integral_image;
	delete[] data;

	return 0;
}
