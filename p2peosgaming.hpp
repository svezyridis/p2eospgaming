#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/crypto.h>

using namespace eosio;
using std::string;
using std::vector;
typedef vector<vector<string>> string2dvector;
typedef string2dvector::iterator string2dvector_it;

class tablegame : public contract
{
  public:
    using contract::contract;
    [[eosio::action]] void clear(uint64_t id);

    [[eosio::action]] void create(const name &host, const string game_name);

    [[eosio::action]] void notify(name user, std::string msg);

    [[eosio::action]] void join(uint64_t id, const name &guest);

    [[eosio::action]] void move(uint64_t id, const capi_name &by, string move_params);

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
        uint64_t get_host() const { return host.value; }
        EOSLIB_SERIALIZE(game, (id)(host)(guest)(player_to_play)(winner)(game_state)(game_name))

    };
    // table definition
    typedef eosio::multi_index<"game"_n, game, 
    indexed_by<"byhost"_n, const_mem_fun<game, uint64_t, &game::get_host>>
    > games;

    void send_summary(name user, string message);

    string2dvector initialize_state(string game_name);


};

EOSIO_DISPATCH(tablegame, (create)(notify)(clear)(join) )