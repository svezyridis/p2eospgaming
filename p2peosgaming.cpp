#include "p2peosgaming.hpp"
using namespace eosio;


[[eosio::action]]
void tablegame::create(const account_name& host, const gametype game_name){
    require_auth(host);
    games existing_games(_self, _self);
    auto host_games=existing_games.get_index<N(host)>();
    auto matching_games=host_games.lower_bound();    
}
