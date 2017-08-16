#ifndef LIBICONV_WRAPPER_H
#define LIBICONV_WRAPPER_H

#include <string>

#include <iconv.h>

class Logger;

class LibiconvWrapper
{
public:
    explicit LibiconvWrapper();
    LibiconvWrapper(const std::string& fromcode, const std::string& tocode);
    ~LibiconvWrapper();

    LibiconvWrapper(const LibiconvWrapper &) = delete;
    LibiconvWrapper& operator= (const LibiconvWrapper &) = delete;

    void ConvertBuffer(char* inbuf, const size_t inbuf_size,
                         char* outbuf, const size_t outbuf_size);

private:
    void ResetConversionDescriptor();

    Logger& logger_;
    iconv_t conversion_descriptor_;
    std::string fromcode_;
    std::string tocode_;
};

#endif // LIBICONV_WRAPPER_H

