#include "logger.h"

int main()
{
    const std::string filename = "test.log";
    Logger logger(filename);
    logger.Log("test string\n");

    return 0;
}
