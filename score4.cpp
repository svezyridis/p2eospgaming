#include <eosiolib/eosio.hpp>
#include "game_interface.hpp"
class score4game : game_base{
    public:
    score4game(){
        set_rows(6);
        set_columns(7);
    }
    bool is_valid_movement(string2dvector game_state, string move_params){return true;}
    void update_state (vector<vector<string> >& game_state, string move_params){eosio::print("Savvas");}
    string2dvector init_state(){return string2dvector(rows,vector<string>(columns,""));}   
};
