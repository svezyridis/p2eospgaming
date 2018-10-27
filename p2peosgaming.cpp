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
    send_summary(name("tablegames"), " successfully erased table row, time: " + std::to_string(now()));
}

// create new game action

void tablegame::create(const name &host, const string game_name)
{
    require_auth(host);
    games existing_games(_self, _self.value);
    auto host_games = existing_games.get_index<"byhost"_n>();
    auto iterator = host_games.find(host.value);
    //check if host has other games in play
    if (iterator != host_games.end()) //player has other games active
    {
        for (auto loop_iterator = iterator; loop_iterator != host_games.end() && loop_iterator->host == host; loop_iterator++)
            eosio_assert((loop_iterator->game_name != game_name), "You can't create 2 games of the same type");
    }

    existing_games.emplace(_self, [&](auto &game) {
        game.id = existing_games.available_primary_key();
        game.host = host;
        game.game_name = game_name;
    });
    send_summary(host, "game successfully created, time: " + std::to_string(now()));
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
        game.player_to_play = random_number() == 0 ? guest : game.host;
        game.game_state = initialize_state(game.game_name);
    });
    string summary = "you have successfully joined game, next move by:" + iterator->player_to_play.to_string();
    send_summary(guest, summary.c_str());
    summary = guest.to_string() + " has joined your gamen, next move by:" + iterator->player_to_play.to_string();
    send_summary(iterator->host, summary.c_str());
}

// make move action
// @param by the player who wants to make the move
void tablegame::move(uint64_t id, const name &by, string move_params)
{
    require_auth(by);
    games existing_games(_self, _self.value);
    auto iterator = existing_games.find(id);
    eosio_assert(iterator != existing_games.end(), "Game with ID specified could not be found");
    eosio_assert(iterator->winner=="none"_n, "This game has ended");
    eosio_assert(iterator->player_to_play == by, "It is not your turn to play");
    
    string error_message;
    eosio_assert(is_valid_movement(iterator->game_state, move_params, name{by}.to_string(), error_message, iterator->game_name), error_message.c_str());

    existing_games.modify(iterator, _self, [&](auto &game) {
        game.game_state = updated_state(iterator->game_state, move_params, name{by}.to_string(), iterator->game_name);
        if (is_winning_condition(game.game_state, move_params, name{by}.to_string(), game.game_name))
        {
            game.winner = name(by);
            send_summary(by, " You won the game!");
            send_summary(by == game.host ? game.guest : game.host, " You lost the game!");
            //TODO bet handling
        }
        else
        {
            if (game.player_to_play == game.host)
                game.player_to_play = game.guest;
            else
                game.player_to_play = game.host;
            send_summary(by, " move executed");
            send_summary(iterator->player_to_play, " your turn to play");
        }
    });
}
//state initialization
string2dvector tablegame::initialize_state(string game_name)
{
    score4game *instance = new score4game();
    return instance->init_state();
}

//state update
string2dvector tablegame::updated_state(string2dvector game_state, string move_params, string player, string game_name)
{
    score4game *instance = new score4game();
    return instance->updated_state(game_state, move_params, player);
}
// move validity check
bool tablegame::is_valid_movement(const string2dvector &game_state, const string &move_params, string player, string &error_message, string game_name)
{
    score4game *instance = new score4game();
    return instance->is_valid_move(game_state, move_params, player, error_message);
}
// winning condition check
bool tablegame::is_winning_condition(const string2dvector &game_state, const string &move_params, string player, string game_name)
{
    score4game *instance = new score4game();
    return instance->winning_condition(game_state, move_params, player);
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
int tablegame::random_number()
{
    capi_checksum160 calc_hash;
    string time = std::to_string(now());
    sha1((char *)&time, time.length(), &calc_hash);
    return calc_hash.hash[0] % 2;
}
