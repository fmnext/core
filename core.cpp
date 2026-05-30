#include "core.h"

LIB_API int GetCoreVersion()
{
	return 200;
}

LIB_API const char* GetCoreBuild()
{
	char* timeString = new char[15];
	//timeString.resize(15);

	std::tm tmstamp = {};
	std::istringstream ss(__TIMESTAMP__);
	ss.imbue(std::locale("en_US.utf-8"));
	ss >> std::get_time(&tmstamp, "%a %b %d %H:%M:%S %Y");

	//std::cout << std::put_time(&tmstamp, "%Y%m%d%H%M%S") << "\n";

	//std::strftime(std::data(timeString), std::size(timeString), "%Y%m%d%H%M%S", &tmstamp);
	std::strftime(timeString, 15, "%Y%m%d%H%M%S", &tmstamp);

	return timeString;
}
