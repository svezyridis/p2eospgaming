#include "score4.cpp"
#include "p2peosgaming.hpp"

using namespace eosio;
using std::string;
using std::vector;

//clear table rows

void tablegame::clear(uint64_t id)
{
    require_auth(name("tablegames"));
    games existing_games(_self, _self.value);
    auto iterator = existing_games.find(id);
    eosio_assert(iterator != existing_games.end(), "Object does not exist");
    if (iterator != existing_games.end())
        existing_games.erase(iterator);
    send_summary(name("tablegames")," successfully erased table row, time: "+std::to_string(now()));
}

// create new game action

void tablegame::create(const name &host, const string game_name)
{
    require_auth(host);
    games existing_games(_self, _self.value);
    auto host_games = existing_games.get_index<"byhost"_n>();
    auto iterator = host_games.find(host.value);
    //check if host has other games in play
    if (iterator != host_games.end())
    { //player has other games active
        for (auto loop_iterator = iterator; loop_iterator != host_games.end() && loop_iterator->host == host; loop_iterator++)
            eosio_assert((loop_iterator->game_name != game_name), "You can't create 2 games of the same type");
    }

    existing_games.emplace(_self, [&](auto &game) {
        game.id = existing_games.available_primary_key();
        game.host = host;
        capi_checksum160 calc_hash;
        sha1((char *)&game_name, game_name.length(), &calc_hash);
        printi(calc_hash.hash[0]);
        game.game_name = game_name;
        game.game_state = initialize_state(game_name);
    });
    send_summary(host, "game successfully created, time: "+std::to_string(now()));
}

// reciept
void tablegame::notify(name user, std::string msg)
{
    require_auth(get_self());
    require_recipient(user);
}

// join game action
void tablegame::join(uint64_t id, const name &guest)
{
    require_auth(guest);
    games existing_games(_self, _self.value);
    auto iterator = existing_games.find(id);
    eosio_assert(iterator != existing_games.end(), "Game with ID specified could not be found");
    eosio_assert(iterator->guest == "none"_n, "Game has allready started");
    eosio_assert(iterator->host != guest, "You cannot play against yourself");
    existing_games.modify(iterator, _self, [&](auto &game) {
        game.guest = guest;
        game.player_to_play= random_number()==0 ? guest : game.host; 
    });
    string summary="you have successfully joined game, next move by:"+iterator->player_to_play.to_string();
    send_summary(guest, summary.c_str());
    summary=guest.to_string() + " has joined your gamen, next move by:"+iterator->player_to_play.to_string();
    send_summary(iterator->host, summary.c_str());


}
// make move action
// @param by the player who wants to make the move
void tablegame::move(uint64_t id, const name &by, string move_params){
    require_auth(by);
    games existing_games(_self, _self.value);
    auto iterator = existing_games.find(id);
    eosio_assert(iterator != existing_games.end(), "Game with ID specified could not be found");
    eosio_assert(iterator->player_to_play==by,"It is not your turn to play");

    score4game *instance = new score4game();
    string error_message;
    print("checking move validity");
    eosio_assert(instance->is_valid_movement(iterator->game_state,move_params,name{by}.to_string(), error_message),error_message.c_str());
    print("validity confirmed");
    existing_games.modify(iterator, _self, [&](auto &game) {
        print("updateing state");
        game.game_state = instance->updated_state(iterator->game_state, move_params, name{by}.to_string());
        if(game.player_to_play==game.host)
            game.player_to_play=game.guest;
        else
            game.player_to_play=game.host;
    });
    send_summary(by," move executed");

}
//state initialization
string2dvector tablegame::initialize_state(string game_name)
{
    score4game *instance = new score4game();
    return instance->init_state();
}

void tablegame::send_summary(name user, std::string message)
{
    action(
        permission_level(get_self(), name("active")),
        get_self(),
        name("notify"),
        std::make_tuple(user, name{user}.to_string() + " " + message))
        .send();
}

//pseudo rundom number generator using the block time
int tablegame::random_number(){
    capi_checksum160 calc_hash;
    string time=std::to_string(now());
    sha1( (char*)&time, time.length(), &calc_hash );
    return calc_hash.hash[0]%2;
}
    
