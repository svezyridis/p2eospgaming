#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include "score4.cpp"

using namespace eosio;
using std::string;
using std::vector;
typedef vector<vector<string>> string2dvector;
typedef string2dvector::iterator string2dvector_it;

class tablegame : public contract
{
  public:
    tablegame(account_name self) : contract(self){}
        [[eosio::action]] void clear(uint64_t id){
            require_auth(N(tablegames));
            games existing_games(_self, _self);
            auto iterator=existing_games.find(id);
            if(iterator!=existing_games.end())
                existing_games.erase(iterator);
        }

        // create new game action
        [[eosio::action]] void create(const account_name &host, const string game_name)
    {
        require_auth(host);
        games existing_games(_self, _self);
        auto host_games = existing_games.get_index<N(host)>();
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
            game.game_name = game_name;
            game.game_state = initialize_state(game_name);
        });
        send_summary(host, "game successfully created");
    }

    // reciept
    [[eosio::action]] void notify(account_name user, std::string msg) {
        require_auth(get_self());
        require_recipient(user);
    }

    // join game action
    void join(uint64_t id, const account_name &guest);
    // make move action
    // @param by the player who wants to make the move
    void move(uint64_t id, const account_name &by, string move_params);
    //state initialization
    vector<string> initialize_state(string game_name){
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
        account_name host;
        account_name guest= N(none);
        account_name player_to_play =N(none);
        account_name winner = N(none);
        vector<string> game_state;
        string game_name;
        uint64_t primary_key() const { return id; }
        uint64_t secondary_key() const { return host; }
        EOSLIB_SERIALIZE(game, (id)(host)(guest)(player_to_play)(winner)(game_state)(game_name))

    };
    // table definition
    typedef eosio::multi_index<N(game), game, 
    indexed_by<N(host), const_mem_fun<game, uint64_t, &game::secondary_key>>
    > games;

    void send_summary(account_name user, std::string message)
    {
        action(
            permission_level(get_self(), N(active)),
            get_self(),
            N(notify),
            std::make_tuple(user, name{user}.to_string() + message))
            .send();
    }
};

EOSIO_ABI(tablegame, (create)(notify)(clear))