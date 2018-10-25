#include <eosiolib/eosio.hpp>
#include "game_interface.hpp"
using namespace eosio;
class score4game : game_base{
    public:
    score4game(){
        set_rows(6);
        set_columns(7);
    }


    bool is_valid_movement(string2dvector board, string move_params, string player, string& error_message)
    {
        int column=std::stoi(move_params);
        --column;
        if(column<=0||column>7){
            error_message="Out of range, board is(6*7)";
            return false;
        }
        vector<string> column_to_search= board.at(column);
        for (auto itr=column_to_search.begin(); itr!=column_to_search.end(); itr++){
            if(*itr==" ")
            return true;
        }
        error_message="The column provided is full";
        return false;

        }
    string2dvector updated_state (string2dvector board, string move_params, string player){
        print("updating state");
        int column=std::stoi(move_params);
        --column;
        for (auto itr=board.at(column).begin(); itr!=board.at(column).end(); itr++){
            if(*itr==" "){
                *itr=player;
                break;
            }
        }
        return board;

        }

    string2dvector init_state(){
        string2dvector state=string2dvector(columns,vector<string>(rows," "));
        return state;
        }   
};
