#include <libgen.h>

#include "Gomoku.hpp"
#include "ResourceMgr.hpp"

int		main(int ac, char** av)
{
    Gomoku app;
    
    (void)ac;
    ResourceMgr::set_binary_directory(std::string(dirname(av[0])));
    app.run();

  return 0;
}
