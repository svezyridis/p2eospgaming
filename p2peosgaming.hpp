#pragma once
#include <eosiolib/eosio.hpp>
#include "score4.cpp"

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
    struct [[eosio::table]] game{
        game() {
            init_game();
        }
        uint64_t id; //auto increment
        account_name host;
        account_name guest;
        account_name player_to_play;
        account_name winner = N(none);
        string2dvector game_state;
        gametype game_name;
        uint64_t primary_key() const { return id; }
        uint64_t secondary_key() const {return host;}
        EOSLIB_SERIALIZE( game, (host)(guest)(player_to_play)(winner)(game_state)(game_name)(move_params))
        
        //game initialization
        void init_game(){
            switch(game_name){
                case connect4 :
                    score4game *game_instance = new score4game();
                    game_state=game_instance->init_state();
            }

        }
    };
    // table definition
    typedef eosio::multi_index< N(games), game, indexed_by<N(host), const_mem_fun<game, uint64_t, &game::secondary_key>>> games;
    // create new game action
    void create(const account_name& host, const gametype game_name);
    // join game action
    void join(uint64_t id, const account_name& guest);
    // make move action
    // @param by the player who wants to make the move
    void move(uint64_t id, const account_name& by, string move_params);

};
