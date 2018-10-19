#include <string>
#include <vector>
using std::vector;
using std::string;
typedef vector<vector<string> > string2dvector;

class tablegame {
    public:
    virtual int get_rows()= 0;
    virtual int get_columns()=0;
    virtual bool is_valid_movement(string2dvector game_state, string move_params)=0;
    virtual void update_state (vector<vector<string> >& game_state, string move_params)=0;
     
    protected:
    int rows;
    int columns;

};
