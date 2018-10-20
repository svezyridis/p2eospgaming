#pragma once
#include <eosiolib/eosio.hpp>

using namespace eosio;
enum gametype {chess,chekcers,connect4};
using std::string;
using std::vector;
typedef vector<vector<string> > string2dvector;
typedef string2dvector::iterator string2dvector_it;

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
        string2dvector game_state;
        gametype game_name;
        string move_params;
        //game initialization
        void init_game(){
            switch(game_name){
                case connect4 :
                    game_state=string2dvector(5,vector<string>(5,""));
            }

        }
    };
};
