/* network cote serveur
*/

#ifndef __H_BNET_SERVER
# define __H_BNET_SERVER

# include <utility>
# include "bnet_header.hpp"

//parametres de bnet_tell_end (voir le PDF sur le protocole)
typedef enum{
  BNET_END_CAPTURE=0,
  BNET_END_FIVEALIGN,
  BNET_END_RULEERR,
  BNET_END_TIMEOUT,
} bnet_end_reason;

//informatif seulement
typedef void* bnet_ai_hdl_t;


/*
  faut pas oublier de close chaque IA ouverte avec cette fonction
  param: le port, double 3, break 5, timeout en millisecondes
  valeur de retour d'erreur: 0 [zero] (considerer qu'aucune IA ne s'est co dans le temps imparti)
*/
bnet_ai_hdl_t bnet_get_ai(short port, bool d3, bool b5, int timeout_ms);

/*
  param: l'IA
*/
void bnet_close_ai(bnet_ai_hdl_t ai);


/*===============
| si la valeur de retour de l'une de ces fonctions vaut false, cela veut dire que le client correspondant ne repond plus (BNET_TIMEOUT) ou qu'il est deconnecte. il faut toujours appeler bnet_close_ai pour liberer les ressources.
================*/

//toujours dire bonjour apres la connection d'une IA
bool bnet_say_hello(bnet_ai_hdl_t ai);

//pour les parametres pos de ces 2 fonctions, il est possible de passer une vraie variable ou une valeur temporaire au choix
bool bnet_tell_add(bnet_ai_hdl_t ai, std::pair<int, int> const& pos);
bool bnet_tell_remove(bnet_ai_hdl_t ai, std::pair<int, int> const& pos1, std::pair<int, int> const& pos2);

//ici il faut passer une vraie variable, elle sera remplie avec le resultat (toujours verifier les valeurs!)
bool bnet_let_play(bnet_ai_hdl_t ai, std::pair<int, int>& pos);

//voir l'enum pour le parametre; osef de la valeur de retour
void bnet_tell_end(bnet_ai_hdl_t ai, bool wins, bnet_end_reason reason);

#endif
