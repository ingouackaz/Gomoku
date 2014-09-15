#define _XOPEN_SOURCE
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "bnet_server.hpp"

#define BNET_DEST_SRV 3411
#define BNET_DEST_CLT 42
#define BNET_MSG_MOD 0777
#define BNET_PIPE_OK 'y'
#define BNET_CMD_HELLO 'h'
#define BNET_CMD_ADD 'a'
#define BNET_CMD_REM 'r'
#define BNET_CMD_END 'e'
#define BNET_CMD_PLAY 'p'
#define BNET_TOK_DELIM " \n"

char const *bnet_s__end_state[4] ={
  "CAPTURE",
  "FIVEALIGN",
  "RULEERR",
  "TIMEOUT",
};

typedef struct bnet_msg{
  long mtype;
  char mtext[BNET_MAX_MSG_SZ + 1];
} bnet_msg_t;

typedef struct bnet_ai{
  int socket;
  int msg;
  bool rule_d3;
  bool rule_b5;
  int rule_timeout_ms;
  struct timeval rule_timeout;
  struct timeval welcome_timeout;
  int child;
  int pipe;
  bnet_msg_t buf;
} bnet_ai_t;

static void bnet_kill_socket(int s){
  shutdown(s, SHUT_RDWR);
  close(s);
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

static bool bnet_msg_cmd(bnet_ai_t *ai, char cmd, struct timeval *tvp){
  struct timeval tv;
  fd_set rfds;
  char got;
  
  tv = *tvp;
  FD_ZERO(&rfds);
  FD_SET(ai->pipe, &rfds);
  ai->buf.mtype = BNET_DEST_CLT;
  ai->buf.mtext[0] = cmd;
  if (msgsnd(ai->msg, &ai->buf, sizeof(bnet_msg_t), IPC_NOWAIT) < 0
      || select(ai->pipe + 1, &rfds, NULL, NULL, &tv) <= 0
      || FD_ISSET(ai->pipe, &rfds) == 0
      || read(ai->pipe, &got, 1) < 1
      || got != BNET_PIPE_OK)
    return false;
  return true;
}

static int bnet_child_main(bnet_ai_t *ai){
  char ok;
  bnet_msg_t buf;
  int size, maxsize;

  ok = BNET_PIPE_OK;
  while (3411){
    if (msgrcv(ai->msg, &ai->buf, sizeof(bnet_msg_t), BNET_DEST_CLT, 0) <= 0)
      break;
    switch ((int)ai->buf.mtext[0]){
      
    case BNET_CMD_HELLO:
      if ((size = recv(ai->socket, buf.mtext, strlen(BNET_HELLO), MSG_WAITALL)) < 1)
	return (1);
      buf.mtext[size] = '\0';
      if (strcmp(buf.mtext, BNET_HELLO))
	return (1);
      sprintf(buf.mtext, BNET_RULES, (ai->rule_d3?'1':'0'), (ai->rule_b5?'1':'0'), ai->rule_timeout_ms);
      if (!bnet_send_all(ai->socket, buf.mtext))
	return (1);
      break;
      
    case BNET_CMD_ADD:
    case BNET_CMD_REM:
    case BNET_CMD_END:
      if (!bnet_send_all(ai->socket, ai->buf.mtext + 1))
	return (1);
      break;
      
    case BNET_CMD_PLAY:
      if (!bnet_send_all(ai->socket, BNET_YOURTURN))
	return (1);
      if ((size = recv(ai->socket, buf.mtext, strlen(BNET_PLAY2), MSG_WAITALL)) < 1)
	return (1);
      buf.mtext[size] = '\0';
      if (strcmp(buf.mtext, BNET_PLAY2))
	return (1);
      maxsize = BNET_MAX_MSG_SZ - size - 1;
      size = 0;
      memset(ai->buf.mtext, '\0', BNET_MAX_MSG_SZ + 1);
      while (recv(ai->socket, ai->buf.mtext + size, 1, MSG_WAITALL) > 0
	     && ai->buf.mtext[size++] != '\n' && size < maxsize);
      ai->buf.mtype = BNET_DEST_SRV;
      if (size < 2 || size >= maxsize || ai->buf.mtext[size - 1] != '\n'
	  || msgsnd(ai->msg, &ai->buf, sizeof(bnet_msg_t), IPC_NOWAIT) < 0)
	return (1);
      break;
      
    }
    write(ai->pipe, &ok, 1);
  }
  return (0);
}

bool bnet_say_hello(void *ai_hdl){
  bnet_ai_t *ai;
  
  ai = (bnet_ai_t *)ai_hdl;
  return (bnet_msg_cmd(ai, BNET_CMD_HELLO, &ai->welcome_timeout));
}

bool bnet_tell_add(void *ai_hdl, std::pair<int, int> const& pos){
  bnet_ai_t *ai;
  
  ai = (bnet_ai_t *)ai_hdl;
  sprintf(ai->buf.mtext + 1, BNET_ADD, pos.first, pos.second);
  return (bnet_msg_cmd(ai, BNET_CMD_ADD, &ai->welcome_timeout));
}

bool bnet_tell_remove(void *ai_hdl, std::pair<int, int> const& pos1, std::pair<int, int> const& pos2){
  bnet_ai_t *ai;
  
  ai = (bnet_ai_t *)ai_hdl;
  sprintf(ai->buf.mtext + 1, BNET_REM, pos1.first, pos1.second, pos2.first, pos2.second);
  return (bnet_msg_cmd(ai, BNET_CMD_REM, &ai->welcome_timeout));
}

bool bnet_let_play(void *ai_hdl, std::pair<int, int>& pos){
  bnet_ai_t *ai;
  char *tok;

  ai = (bnet_ai_t *)ai_hdl;
  if (!bnet_msg_cmd(ai, BNET_CMD_PLAY, &ai->rule_timeout))
    return false;
  if (msgrcv(ai->msg, &ai->buf, sizeof(bnet_msg_t), BNET_DEST_SRV, IPC_NOWAIT) < 1
      || (tok = strtok(ai->buf.mtext, BNET_TOK_DELIM)) == NULL)
    return false;
  pos.first = atoi(tok);
  if ((tok = strtok(NULL, BNET_TOK_DELIM)) == NULL)
    return false;
  pos.second = atoi(tok);
  return true;
}

void bnet_tell_end(void *ai_hdl, bool wins, bnet_end_reason reason){
  bnet_ai_t *ai;
  
  ai = (bnet_ai_t *)ai_hdl;
  sprintf(ai->buf.mtext + 1, BNET_END, wins ? BNET_WIN : BNET_LOSE, bnet_s__end_state[reason]);
  bnet_msg_cmd(ai, BNET_CMD_END, &ai->welcome_timeout);
}


void  *bnet_get_ai(short port, bool d3, bool b5, int timeout_ms){
  int yes;
  struct sockaddr_in sin;
  bnet_ai_t *ret;
  int m_sock;
  struct timeval tv;
  fd_set rfds;
  int pipefd[2];

  yes = 1;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = INADDR_ANY;
  if ((m_sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1)
    goto bt_e_a;
  if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1
      || bind(m_sock, (struct sockaddr *)&sin, sizeof(sin)) == -1
      || listen(m_sock, 1) == -1)
    goto bt_e_a_socket;
  if (NULL == (ret = (bnet_ai_t *)malloc(sizeof(*ret))))
    goto bt_e_a_listen;
  if ((ret->msg = msgget(IPC_PRIVATE, BNET_MSG_MOD)) == -1)
    goto bt_e_a_malloc;
  ret->socket = 0;
  ret->rule_d3 = d3;
  ret->rule_b5 = b5;
  ret->rule_timeout_ms = timeout_ms;
  ret->rule_timeout.tv_sec = timeout_ms / 1000;
  ret->rule_timeout.tv_usec = (timeout_ms % 1000) * 1000;
  ret->welcome_timeout.tv_sec = BNET_WELCOME_TIMEOUT;
  ret->welcome_timeout.tv_usec = 0;
  FD_ZERO(&rfds);
  FD_SET(m_sock, &rfds);
  tv = ret->welcome_timeout;
  if (select(m_sock + 1, &rfds, NULL, NULL, &tv) <= 0
      || FD_ISSET(m_sock, &rfds) == 0
      || (ret->socket = accept(m_sock, NULL, NULL)) == -1)
    goto bt_e_a_msg;
  if (pipe(pipefd) < 0)
    goto bt_e_a_accept;
  if ((ret->child = fork()) == -1)
    goto bt_e_a_pipe;
  ret->pipe = pipefd[ret->child ? 0 : 1];
  close(pipefd[ret->child ? 1 : 0]);
  bnet_kill_socket(m_sock);
  if (!ret->child)
    exit(bnet_child_main(ret));
  return (ret);

  //error
 bt_e_a_pipe:
  close(pipefd[0]);
  close(pipefd[1]);
 bt_e_a_accept:
  bnet_kill_socket(ret->socket);
 bt_e_a_msg:
  msgctl(ret->msg, IPC_RMID, NULL);
 bt_e_a_malloc:
  free(ret);
 bt_e_a_listen:
  shutdown(m_sock, SHUT_RDWR);
 bt_e_a_socket:
  close(m_sock);
 bt_e_a:
  return (0);
}

void bnet_close_ai(void *ai_hdl){
  bnet_ai_t *ai;

  ai = (bnet_ai_t *)ai_hdl;
  close(ai->pipe);
  bnet_kill_socket(ai->socket);
  msgctl(ai->msg, IPC_RMID, NULL);
  if (kill(ai->child, SIGKILL) == 0)
    waitpid(ai->child, NULL, 0);
  free(ai);
}
