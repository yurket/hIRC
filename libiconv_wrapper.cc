#include "libiconv_wrapper.h"

#include "logger.h"

#include <cstring>
#include <exception>
#include <iostream>

#include <errno.h>

namespace
{

class LibiconvWrapperException : public std::exception
{
public:
    explicit LibiconvWrapperException(const std::string& msg) :
        msg_(msg)
    {
    }

    const char* what() const noexcept
    {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

} // namespace


LibiconvWrapper::LibiconvWrapper() :
    logger_(Logger::Get("general")),
    fromcode_(),
    tocode_()
{
}

LibiconvWrapper::LibiconvWrapper(const std::string& fromcode, const std::string& tocode) :
    logger_(Logger::Get("general")),
    fromcode_(fromcode),
    tocode_(tocode)
{
    conversion_descriptor_ = iconv_open(tocode.c_str(), fromcode.c_str());
    if (conversion_descriptor_ == (iconv_t)(-1))
    {
        const std::string error_message = "iconv_open error: " + std::string(strerror(errno));
        logger_.Log(error_message);
        throw LibiconvWrapperException(error_message);
    }
}

LibiconvWrapper::~LibiconvWrapper()
{
    if (iconv_close(conversion_descriptor_) == -1)
    {
        const std::string error_message = "iconv_close error: " + std::string(strerror(errno));
        logger_.Log(error_message);
        throw LibiconvWrapperException(error_message);
    }
}

void LibiconvWrapper::ResetConversionDescriptor()
{
    size_t res;
    res = iconv(conversion_descriptor_, NULL, NULL, NULL, NULL);
    if (res == (size_t)-1)
    {
        const std::string error_message = "iconv error: " + std::string(strerror(errno));
        logger_.Log(error_message);
        throw LibiconvWrapperException(error_message);
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
        perror("iconv error");
        std::cerr << "inbuf_size: " << inbuf_size_ << ", outbuf_size: " << outbuf_size_ << std::endl;

        // TODO: ugly string formatting
        throw LibiconvWrapperException("iconv error: input encoding: " + fromcode_ \
                                       + ", output encoding: " + tocode_);
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
