#include <string>
#include <vector>
using std::vector;
using std::string;
typedef vector<vector<string> > string2dvector;

class game_base {
    public:
    void set_rows(int r){rows=r;}
    void set_columns(int col){columns=col;}
    virtual bool is_valid_movement(string2dvector game_state, string move_params, string player, string& error_message)=0;
    virtual string2dvector updated_state (string2dvector game_state, string move_params, string player)=0;
    virtual string2dvector init_state()=0;
    protected:
    int rows;
    int columns;
};