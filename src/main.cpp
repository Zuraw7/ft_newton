#include "../include/common/Logger.hpp"

using namespace newton::common;


int main() {

	Logger &logger = Logger::getInstance();

	logger.printf(LogLevel::WARN ,"Hello World\n");

	return 0;
}
