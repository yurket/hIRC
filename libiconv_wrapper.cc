#include "libiconv_wrapper.h"

#include <cstdio>   // perror

#include <exception>
#include <iostream>


LibiconvWrapper::LibiconvWrapper()
    : initialized_(false)
{
}

LibiconvWrapper::LibiconvWrapper(const std::string& fromcode, const std::string& tocode)
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
        perror("iconv_close");
        throw std::exception();
    }
}

void LibiconvWrapper::ResetConversionDescriptor()
{
    size_t res;
    res = iconv(conversion_descriptor_, NULL, NULL, NULL, NULL);
    if (res == (size_t)-1)
    {
        perror("iconv reset");
        throw std::exception();
    }

}

void LibiconvWrapper::ConvertBuffer(char* inbuf, const size_t inbuf_size,
                                    char* outbuf, const size_t outbuf_size)
{
    size_t res, inbuf_size_, outbuf_size_;
    char* inbuf1[] = { inbuf };
    char* outbuf1[] = { outbuf };

    inbuf_size_ = inbuf_size;
    outbuf_size_ = outbuf_size;

    res = iconv(conversion_descriptor_, inbuf1, &inbuf_size_, outbuf1, &outbuf_size_);
    if (res == (size_t)-1)
    {
        perror("iconv");
        std::cerr << "inbuf_size: " << inbuf_size_ << ", outbuf_size: " << outbuf_size_ << std::endl;
        throw std::exception();
    }

    // debug
    if (0)
    {
        std::cout << "bytes converted: " << res << std::endl;
        std::cout << "outbuf: " << outbuf1 << " inleft: " << inbuf_size \
                  << " outleft: " << outbuf_size << std::endl;
    }
    ResetConversionDescriptor();
}


// TODO: add simple tests for some encoding coverstations?

// int main()
// {
//     LibiconvWrapper converter("UTF-8", "CP1251");
//     // converter.ConvertCP(NULL, NULL, NULL, NULL);
//     return 0;
// }
