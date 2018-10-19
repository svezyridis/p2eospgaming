#pragma once
#include <eosiolib/eosio.hpp>

using namespace eosio;

class tablegame : public contract{
    public:
    tablegame(account_name self):contract(self){}
    
    /**
     * @Game structure
     */
    struct game{
        game() {
            init_game();
        }
        account_name host;
        account_name guest;
        account_name player_to_play;
        account_name winner = N(none);
        std::string game_state;
        std::string

        //game initialization
        void init_game(){

        }
    }
}