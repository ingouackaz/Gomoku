
#ifndef HPP_RESOURCEMGR
#define HPP_RESOURCEMGR

#include <string>

class ResourceMgr
{
    public:
    static inline void set_binary_directory(std::string const& dir)
    {
        m_path = dir + "/../resources/";
    }

    static inline std::string fetch_resource(std::string const& file)
    {
        return m_path + file;
    }

    private:
    static std::string m_path;
    
    ResourceMgr();
    ResourceMgr(ResourceMgr const&);
    ~ResourceMgr();
};

#endif

