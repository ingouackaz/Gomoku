#define _XOPEN_SOURCE
#define _BSD_SOURCE
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "bnet_ai_main.hpp"

#define BNET_IPC_MOD 0777
#define BNET_POS_VALID(p) (((p)[0]) >= 0 && ((p)[0]) <= 18 && ((p)[1]) >= 0 && ((p)[1]) <= 18)
#define BNET_POS_RESET(p) do{ (p)[0] = -1; (p)[1] = -1;}while(0);

union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO
			      (Linux-specific) */
};

//global
Map bnet_g__map;

//static
static int bnet_s__child = -1;
static int bnet_s__ipc_sem;
static int bnet_s__ipc_shm;
static void *bnet_s__ipc_shmaddr;
static bool bnet_s__rule_d3;
static bool bnet_s__rule_b5;
static int bnet_s__rule_timeout_ms;
static bool bnet_s__got_first_add = false;
static int const bnet_s__null_pos[2] = {-1,-1};
static struct sembuf bnet_s__ipc_sembuf = {bnet_s__ipc_sembuf.sem_num = 1, bnet_s__ipc_sembuf.sem_op = 0, bnet_s__ipc_sembuf.sem_flg = 0};
static union semun bnet_s__ipc_semun = {1};
static int bnet_s__pre_timeout_us;

static inline int bnet_get_timeout_failsafe(int timeout){
  if (timeout < 5)
    return 2;
  if (timeout < 10)
    return 3;
  if (timeout < 100)
    return 5;
  if (timeout < 200)
    return 10;
  return (timeout / 20);
  //return (frac > 20 ? 20 : frac);
}

static void bnet_destructor(){
  if (bnet_s__child > 0)
    semctl(bnet_s__ipc_sem, 0, IPC_RMID);
  shmctl(bnet_s__ipc_shm, IPC_RMID, NULL);
  shmdt(bnet_s__ipc_shmaddr);
}

static bool bnet_send_all(int socket, char const *buf){
  int done, total, ret;

  total = strlen(buf);
  done = 0;
  while (done < total)
    if ((ret = send(socket + done, buf, total - done, MSG_NOSIGNAL)) < 0)
      return false;
    else
      done += ret;
  return true;
}

static bool bnet_get_query(int socket, std::string& res){
  char c;
  int size;

  res.clear();
  size = 0;
  while (size < BNET_MAX_MSG_SZ){
    if (recv(socket, &c, 1, MSG_WAITALL) < 1)
      return false;
    if (c == '\n')
      break;
    res += c;
  }
  if (c != '\n')
    return false;
  return true;
}

inline static void bnet_reset_play(){
  memcpy(bnet_s__ipc_shmaddr, bnet_s__null_pos, sizeof(int) + sizeof(int));
}

inline static bool bnet_semaphore_op(int op){
  bnet_s__ipc_sembuf.sem_op = op;
  return (semop(bnet_s__ipc_sem, &bnet_s__ipc_sembuf, 1) == 0);
}

void bnet_commit_play(int pos[2]){
  bnet_semaphore_op(-1);
  memcpy(bnet_s__ipc_shmaddr, pos, sizeof(int) + sizeof(int));
  bnet_semaphore_op(1);
}

void bnet_end(bool error){
  bnet_destructor();
  std::cout << "Connection closed" << std::endl;
  if (error)
    exit(EXIT_FAILURE);
}

void bnet_suicide(char const *msg){
  bnet_destructor();
  std::cerr << "Error: " << msg << std::endl;
  exit(EXIT_FAILURE);
}

int main(int ac, char **av){
  int sock;
  struct addrinfo hints;
  struct addrinfo *reslist, *res;
  int error;
  int got1, got2, got3;
  int pos[2];
  int pos2[2];
  std::string query, cmd, reason;
  std::istringstream iss;
  char buf[BNET_MAX_MSG_SZ + 1];
  
  if (ac < 3)
    {
      std::cerr << "Usage: " << av[0] << " address port" << std::endl;
      exit(1);
    }
  std::cout << "Gomoku IA\n  guina_b ingoua_r irjud_n jowett_j phan_d" << std::endl;
  if ((bnet_s__ipc_shm = shmget(IPC_PRIVATE, sizeof(int) + sizeof(int), BNET_IPC_MOD)) == -1
      || (bnet_s__ipc_shmaddr = shmat(bnet_s__ipc_shm, NULL, 0)) == (void *)-1)
    bnet_suicide("failed to initialize IPCs");
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_NUMERICSERV;
  if ((error = getaddrinfo(av[1], av[2], &hints, &reslist)) != 0)
    bnet_suicide((std::string("resolving host: ") + gai_strerror(error)).c_str());
  else if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    bnet_suicide("Could not create socket");
  for (res = reslist; res != NULL; res = res->ai_next)
    if (connect(sock, res->ai_addr, res->ai_addrlen) == 0)
      break;
  if (res == NULL)
    bnet_suicide("Could not connect to host");
  freeaddrinfo(reslist);
  std::cout << "Connection successful" << std::endl
	    << "Address: " << av[1] << std::endl
	    << "Port: " << av[2] << std::endl;
  if (!bnet_send_all(sock, BNET_HELLO)
      || !bnet_get_query(sock, query))
    bnet_end(true);
  iss.str(query);
  cmd.clear();
  iss >> cmd;
  if (cmd != BNET_RULES2)
    bnet_suicide("Expected RULES command");
  got1 = got2 = got3 = -1;
  iss >> got1;
  iss >> got2;
  iss >> got3;
  if (got1 < 0 || got1 > 1 || got2 < 0 || got2 > 1 || got3 < 0)
    bnet_suicide("Server gave wrong rule params");
  Map::set_double_three(bnet_s__rule_d3 = got1 ? true : false);
  Map::set_breaking_five(bnet_s__rule_b5 = got2 ? true : false);
  bnet_s__rule_timeout_ms = got3;
  std::cout << "Double 3: " << (bnet_s__rule_d3 ? "yes" : "no") << std::endl
	    << "Breaking 5: " << (bnet_s__rule_b5 ? "yes" : "no") << std::endl
	    << "Play timeout: " << bnet_s__rule_timeout_ms << "ms" << std::endl << std::endl;
  bnet_s__pre_timeout_us = (bnet_s__rule_timeout_ms - bnet_get_timeout_failsafe(bnet_s__rule_timeout_ms)) * 1000;
  while (3411){
    std::cout << "Waiting for next command: " << std::flush;
    if (!bnet_get_query(sock, query))
      bnet_end(true);
    iss.str(query);
    iss.clear();
    cmd.clear();
    iss >> cmd;
    if (cmd == BNET_REM2){
      std::cout << "Remove: " << std::flush;
      BNET_POS_RESET(pos);
      BNET_POS_RESET(pos2);
      iss >> pos[0];
      iss >> pos[1];
      iss >> pos2[0];
      iss >> pos2[1];
      if (!BNET_POS_VALID(pos) || !BNET_POS_VALID(pos2))
	bnet_suicide("Invalid positions");
      std::cout << "(" << pos[0] << ", " << pos[1] << ") and (" << pos2[0] << ", " << pos2[1] << ")" << std::endl;
      //bnet_g__map.take(pos[0], pos[1]);
      //bnet_g__map.take(pos2[0], pos2[1]);
    }
    else if (cmd == BNET_ADD2){
      if (!bnet_s__got_first_add)
	{
	  bnet_s__got_first_add = true;
	  std::cout << "(your color: white) " << std::flush;
	}
      std::cout << "Opponent played: " << std::flush;
      BNET_POS_RESET(pos);
      iss >> pos[0];
      iss >> pos[1];
      if (!BNET_POS_VALID(pos))
	bnet_suicide("Invalid position");
      std::cout << "(" << pos[0] << ", " << pos[1] << ")" << std::endl;
      bnet_g__map.play(0, pos[0], pos[1]);
    }
    else if (cmd == BNET_WIN || cmd == BNET_LOSE){
      std::cout << "Game ended: " << std::flush;
      reason.clear();
      iss >> reason;
      std::cout << (cmd == BNET_WIN ? "Victory" : "Defeat") << std::endl;
      if (reason.empty())
	bnet_suicide("Expected reason");
      std::cout << "Reason: " << reason << std::endl
		<< "bye bye" << std::endl;
      break;
    }
    else if (cmd == BNET_YOURTURN2){
      if (!bnet_s__got_first_add)
	{
	  bnet_s__got_first_add = true;
	  std::cout << "(your color: black) First stone: (9, 9)" << std::endl;
	  sprintf(buf, BNET_PLAY, 9, 9);
	  if (!bnet_send_all(sock, buf))
	    bnet_end(true);
	  bnet_g__map.play(1, 9, 9);
	  continue;
	}
      std::cout << "Play Request: " << std::flush;
      bnet_reset_play();
      if ((bnet_s__ipc_sem = semget(IPC_PRIVATE, 1, BNET_IPC_MOD)) == -1)
	bnet_suicide("Couldn't get semaphore for new child");
      bnet_s__child = 3411; //en cas de suicide avant le fork
      if (semctl(bnet_s__ipc_sem, 0, SETVAL, bnet_s__ipc_semun) == -1)
	bnet_suicide("Couldn't set semaphore value for new child");
      if ((bnet_s__child = fork()) == -1)
	bnet_suicide("fork fail");
      if (bnet_s__child == 0)
	{
	  IA::play();
	  pause();
	  while (3411);
	  exit(EXIT_FAILURE);
	}
      usleep(bnet_s__pre_timeout_us);
      bnet_semaphore_op(-1);
      if (!memcmp(bnet_s__ipc_shmaddr, bnet_s__null_pos, sizeof(int) + sizeof(int)))
	memset(bnet_s__ipc_shmaddr, 0, sizeof(int) + sizeof(int));
      sprintf(buf, BNET_PLAY, ((int *)bnet_s__ipc_shmaddr)[0], ((int *)bnet_s__ipc_shmaddr)[1]);
      if (!bnet_send_all(sock, buf))
	bnet_end(true);
      std::cout << "(" << ((int *)bnet_s__ipc_shmaddr)[0] << ", " << ((int *)bnet_s__ipc_shmaddr)[1] << ")" << std::endl;
      kill(bnet_s__child, SIGKILL);
      if (waitpid(bnet_s__child, NULL, 0) != bnet_s__child)
	bnet_suicide("Couldn't kill child properly");
      bnet_s__child = -1;
      semctl(bnet_s__ipc_sem, 0, IPC_RMID);
      if (BNET_POS_VALID((int *)bnet_s__ipc_shmaddr))
	bnet_g__map.play(1, ((int *)bnet_s__ipc_shmaddr)[0], ((int *)bnet_s__ipc_shmaddr)[1]);
    }
    else
      bnet_suicide((std::string("Unknown command: ") + cmd).c_str());
  }
  shutdown(sock, SHUT_RDWR);
  close(sock);
  bnet_destructor();
  bnet_end(false);
  return (EXIT_SUCCESS);
}
