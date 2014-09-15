//inclus dans bnet_server.hpp et bnet_ai_main.hpp


#ifndef __H_BNET_HEADER
# define __H_BNET_HEADER

# define BNET_ERROR -1
# define BNET_WELCOME_TIMEOUT 5
# define BNET_MAX_MSG_SZ 100

# define BNET_HELLO "CONNECT CLIENT\n"
# define BNET_RULES2 "RULES"
# define BNET_RULES BNET_RULES2" %c %c %i\n"
# define BNET_PLAY2 "PLAY"
# define BNET_PLAY BNET_PLAY2" %i %i\n"
# define BNET_REM2 "REM"
# define BNET_REM BNET_REM2" %i %i %i %i\n"
# define BNET_ADD2 "ADD"
# define BNET_ADD BNET_ADD2" %i %i\n"
# define BNET_END "%s %s\n"
# define BNET_WIN "WIN"
# define BNET_LOSE "LOSE"
# define BNET_YOURTURN2 "YOURTURN"
# define BNET_YOURTURN BNET_YOURTURN2"\n"


#endif
