#include "libiconv_wrapper.h"

#include <exception>
#include <iostream>


#include <stdio.h>   // perror


LibiconvWrapper::LibiconvWrapper()
    : initialized_(false)
{
}

LibiconvWrapper::LibiconvWrapper(const std::string tocode, const std::string fromcode)
{
    conversion_descriptor_ = iconv_open(tocode.c_str(), fromcode.c_str());
    if (conversion_descriptor_ == (iconv_t)(-1))
    {
        perror("iconv_open");
        throw std::exception();
    }
    initialized_ = true;
}

LibiconvWrapper::~LibiconvWrapper()
{
    if (iconv_close(conversion_descriptor_) == -1)
    {

        throw std::exception();
    }
}

void LibiconvWrapper::ResetConversionDescriptor()
{
    size_t res;
    res = iconv(conversion_descriptor_, NULL, NULL, NULL, NULL);
    if (res == -1)
    {
        perror("iconv reset");
        throw std::exception();
    }

}

void LibiconvWrapper::ConvertBuffer(char* inbuf, size_t inbuf_size, char* outbuf, size_t outbuf_size)
{
    size_t res;
    char* inbuf1[] = { inbuf };
    char* outbuf1[] = { outbuf };

    std::cout << "inbuf: " << inbuf << std::endl;
    res = iconv(conversion_descriptor_, inbuf1, &inbuf_size, outbuf1, &outbuf_size);
    if (res == -1)
    {
        perror("iconv");
        throw std::exception();
    }
    std::cout << "bytes converted: " << res << std::endl;
    std::cout << "outbuf: " << outbuf1 << " inleft: " << inbuf_size \
              << " outleft: " << outbuf_size << std::endl;

    ResetConversionDescriptor();
}


int main()
{
    LibiconvWrapper converter("UTF-8", "CP1251");
    // converter.ConvertCP(NULL, NULL, NULL, NULL);
    return 0;
}
