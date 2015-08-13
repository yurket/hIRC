#ifndef LIBICONV_WRAPPER_H
#define LIBICONV_WRAPPER_H

#include <string>

#include <iconv.h>


class LibiconvWrapper
{
public:
    explicit LibiconvWrapper();
    LibiconvWrapper(const std::string tocode, const std::string fromcode);
    ~LibiconvWrapper();


    LibiconvWrapper(const LibiconvWrapper &) = delete;
    LibiconvWrapper & operator = (const LibiconvWrapper &) = delete;


    void ConvertBuffer(char* inbuf, size_t inbuf_size,
                       char* outbuf, size_t outbuf_size);

private:
    void ResetConversionDescriptor();

private:
    iconv_t conversion_descriptor_;
    bool initialized_;
};

#endif // LIBICONV_WRAPPER_H

