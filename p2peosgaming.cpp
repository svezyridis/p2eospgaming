#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/crypto.h>
#include "score4.cpp"

using namespace eosio;
using std::string;
using std::vector;
typedef vector<vector<string>> string2dvector;
typedef string2dvector::iterator string2dvector_it;

class tablegame : public contract
{
  public:
        using contract::contract;
        tablegame(name self) : contract(self,self,){}

        //clear table rows
        [[eosio::action]] void clear(uint64_t id){
            require_auth(name("tablegames"));
            games existing_games(_self,0);
            auto iterator=existing_games.find(id);
            eosio_assert(iterator!=existing_games.end(),"Object does not exist");
            if(iterator!=existing_games.end())
                existing_games.erase(iterator);
        }

        // create new game action
        [[eosio::action]] void create(const capi_name &host, const string game_name)
    {
        require_auth(host);
        games existing_games(_self,0);
        auto host_games = existing_games.get_index<name("host")>();
        auto iterator = host_games.find(host);
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
            sha1( (char*)&game_name, game_name.length(), &calc_hash );
            printi(calc_hash.hash[0]);
            game.game_name = game_name;
            game.game_state = initialize_state(game_name);
        });
        send_summary(host, "game successfully created");
    }

    // reciept
    [[eosio::action]] void notify(capi_name user, std::string msg) {
        require_auth(get_self());
        require_recipient(user);
    }

    // join game action
    void join(uint64_t id, const capi_name &guest){

    }
    // make move action
    // @param by the player who wants to make the move
    void move(uint64_t id, const capi_name &by, string move_params);
    //state initialization
    string2dvector initialize_state(string game_name){
        score4game* instance =new score4game();
        return instance->init_state();
    }

  private:
    /**
     * @Game structure
     */
    struct [[eosio::table]] game
    {
        uint64_t id; //auto increment
        name host;
        name guest= name("none");
        name player_to_play =name("none");
        name winner = name("none");
        string2dvector game_state;
        string game_name;
        uint64_t primary_key() const { return id; }
        uint64_t secondary_key() const { return host; }
        EOSLIB_SERIALIZE(game, (id)(host)(guest)(player_to_play)(winner)(game_state)(game_name))

    };
    // table definition
    typedef eosio::multi_index<name("game"), game, 
    indexed_by<name("host"), const_mem_fun<game, uint64_t, &game::secondary_key>>
    > games;

    void send_summary(capi_name user, std::string message)
    {
        action(
            permission_level(get_self(), name("active")),
            get_self(),
            name("notify"),
            std::make_tuple(user, name{user}.to_string() + message))
            .send();
    }
};

EOSIO_DISPATCH(tablegame, (create)(notify)(clear))