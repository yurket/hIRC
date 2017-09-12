#include "utils.h"

#include <sstream>

namespace Util
{

std::string ToString(const long int li)
{
    std::stringstream ss;
    ss << li;
    return ss.str();
}

}
