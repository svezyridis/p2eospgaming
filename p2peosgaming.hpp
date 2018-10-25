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
    ACTION clear(uint64_t id);

    ACTION create(const name &host, const string game_name);

    ACTION notify(name user, std::string msg);

    ACTION join(uint64_t id, const name &guest);

    ACTION move(uint64_t id, const name &by, string move_params);

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
    typedef eosio::multi_index<"games"_n, game, 
    indexed_by<"byhost"_n, const_mem_fun<game, uint64_t, &game::get_host>>
    > games;

    void send_summary(name user, string message);

    bool is_valid_move(string2dvector game_state,string move_params);

    string2dvector initialize_state(string game_name);
    
    int random_number();


};

EOSIO_DISPATCH(tablegame, (create)(notify)(clear)(join)(move) )