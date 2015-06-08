#include "libiconv_wrapper.h"

#include <exception>

LibiconvWrapper::LibiconvWrapper()
    : conversion_descriptor_(nullptr)
    , initialized_(false)
{
}

LibiconvWrapper::LibiconvWrapper(const std::string tocode, const std::string fromcode)
{
    *conversion_descriptor_ = iconv_open(tocode.c_str(), fromcode.c_str());
    if (conversion_descriptor_ == (iconv_t)(-1))
    {
        throw std::exception();
    }
    initialized_ = true;
}



int main()
{
    LibiconvWrapper converter();
    return 0;
}
