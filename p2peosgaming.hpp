#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/crypto.h>
#include <eosiolib/asset.hpp>

using namespace eosio;
using std::string;
using std::vector;
typedef vector<vector<string>> string2dvector;
typedef string2dvector::iterator string2dvector_it;

class tablegame : public contract
{
  public:
    using contract::contract;
    tablegame(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds) {}

    ACTION clear(uint64_t id);

    ACTION create(const name &host, const string game_name);

    ACTION notify(name user, std::string msg);

    ACTION join(uint64_t id, const name &guest);

    ACTION move(uint64_t id, const name &by, string move_params);

    ACTION transfer();

  private:
    /**
     * @Game structure
     */
    struct [[eosio::table]] game
    {
        uint64_t id; //auto increment
        uint64_t stake = 0;
        name host;
        name guest = name("none");
        name player_to_play = name("none");
        name winner = name("none");
        string2dvector game_state;
        string game_name;
        uint64_t primary_key() const { return id; }
        uint64_t get_host() const { return host.value; }
        EOSLIB_SERIALIZE(game, (id)(stake)(host)(guest)(player_to_play)(winner)(game_state)(game_name))
    };
    struct st_transfer
    {
        name from;
        name to;
        asset quantity;
        string memo;
    };

    // table definition
    typedef eosio::multi_index<"games"_n, game,
                               indexed_by<"byhost"_n, const_mem_fun<game, uint64_t, &game::get_host>>>
        games;

    void send_summary(name user, string message);

    void create_game(name host, string game_name);

    void join_game(name guest, uint64_t game_id);

    bool is_valid_movement(const string2dvector &game_state, const string &move_params, string player, string &error_message, string game_name);

    string2dvector initialize_state(string game_name);

    int random_number();

    string2dvector updated_state(string2dvector game_state, string move_params, string player, string game_name);

    bool is_winning_condition(const string2dvector &game_state, const string &move_params, string player, string game_name);
};

extern "C"
{
    void apply(uint64_t receiver, uint64_t code, uint64_t action)
    {
        tablegame _tablegame(name(receiver));
        if (code == receiver && action == name("create").value)
        {
            execute_action(name(receiver), name(code), &tablegame::create);
        }
        else if (code == receiver && action == name("notify").value)
        {
            execute_action(name(receiver), name(code), &tablegame::notify);
        }
        else if (code == receiver && action == name("clear").value)
        {
            execute_action(name(receiver), name(code), &tablegame::clear);
        }
        else if (code == receiver && action == name("join").value)
        {
            execute_action(name(receiver), name(code), &tablegame::join);
        }
        else if (code == receiver && action == name("move").value)
        {
            execute_action(name(receiver), name(code), &tablegame::move);
        }
        else if (code == name("eosio.token").value && action == name("transfer").value)
        {
            execute_action(name(receiver), name(code), &tablegame::transfer);
        }
    }
};
