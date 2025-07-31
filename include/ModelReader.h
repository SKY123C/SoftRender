#include "string"
#ifdef _WIN32
    #ifdef MODELREADER_EXPORTS
        #define MODELREADER_API __declspec(dllexport)
    #else
        #define MODELREADER_API __declspec(dllimport)
    #endif
#else
    #define MODELREADER_API_API
#endif


class MODELREADER_API ModelReaderManager 
{

public:
    void readModel(const std::string& filename);
};