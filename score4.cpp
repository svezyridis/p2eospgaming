#include <eosiolib/eosio.hpp>
#include "game_interface.hpp"
class score4game : tablegame{
    public:
    int get_rows(){return rows;}
    int get_columns(){return columns;}       
    private:
    int rows=6;
    int columns=7;

};