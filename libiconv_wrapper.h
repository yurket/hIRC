#include <iconv.h>

#include <string>


class LibiconvWrapper
{
public:
    explicit LibiconvWrapper();
    LibiconvWrapper(const std::string tocode, const std::string fromcode);
    ~LibiconvWrapper();
    
    size_t ConvertCP(iconv_t cd,
              const char** inbuf, size_t * inbytesleft,
              char** outbuf, size_t * outbytesleft);

private:
    iconv_t *conversion_descriptor_;
    bool initialized_;
};

