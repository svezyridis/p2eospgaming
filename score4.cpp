#include <eosiolib/eosio.hpp>
#include "game_interface.hpp"
using namespace eosio;
class score4game : game_base
{
  public:
    score4game()
    {
        set_rows(6);
        set_columns(7);
    }

    bool is_valid_move(string2dvector board, string move_params, string player, string &error_message)
    {
        int column = std::stoi(move_params);
        //player picks from 1-7, vector counts 0-6
        --column;
        if (column < 0 || column > 6)
        {
            error_message = "Out of range, board is(6*7)";
            return false;
        }
        vector<string> column_to_search = board.at(column);
        for (auto itr = column_to_search.begin(); itr != column_to_search.end(); itr++)
        {
            if (*itr == " ")
                return true;
        }
        error_message = "The column provided is full";
        return false;
    }
    string2dvector updated_state(string2dvector board, string move_params, string player)
    {
        print("updating state \n");
        int column = std::stoi(move_params);
        --column;
        for (auto itr = board.at(column).begin(); itr != board.at(column).end(); itr++)
        {
            if (*itr == " ")
            {
                *itr = player;
                break;
            }
        }
        return board;
    }

    string2dvector init_state()
    {
        string2dvector state = string2dvector(columns, vector<string>(rows, " "));
        return state;
    }

    bool winning_condition(string2dvector board, string move_params, string player)
    {
        int column = std::stoi(move_params);
        return (check_vertical(board, column, player) || check_horizontal(board, column, player) || check_left_diagonal(board, column, player) || check_right_diagonal(board, column, player));
    }

  private:
    bool check_vertical(string2dvector board, int column, string player)
    {
        print("Checking vertical \n");
        int count = 0;
        for (auto itr = board.at(column - 1).rbegin(); itr != board.at(column - 1).rend(); itr++)
        {
            if (*itr == player)
            {
                count++;
            }
            else
                count = 0;
            if (count == 4)
                return true;
        }
        return false;
    }
    bool check_horizontal(string2dvector board, int column, string player)
    {
        print("Checking horizontal \n");
        int r_count = 0; //right
        int l_count = 0; //left
        int row_to_search;
        auto itr = std::find(board.at(column - 1).rbegin(), board.at(column - 1).rend(), player);
        row_to_search = rows - 1 - (itr - board.at(column - 1).rbegin());
        printi(row_to_search);

        //count right
        print("right \n");
        for (auto itr = board.begin() + column - 1; itr != board.end(); itr++)
        {
            print(itr->at(row_to_search), "\n");
            if (itr->at(row_to_search) == player)
                r_count++;
            else
                break;
        }
        //count left
        print("left \n");
        for (auto itr = board.rbegin() + (columns - column + 1); itr != board.rend(); itr++)
        {
            print(itr->at(row_to_search), "\n");
            if (itr->at(row_to_search) == player)
                l_count++;
            else
                break;
        }
        if (r_count + l_count >= 4)
            return true;
        return false;
    }

    bool check_left_diagonal(string2dvector board, int column, string player)
    {
        int u_count = 0; //up
        int d_count = 0; //down
        int last_move_row;
        auto itr = std::find(board.at(column - 1).rbegin(), board.at(column - 1).rend(), player);
        last_move_row = rows - 1 - (itr - board.at(column - 1).rbegin());
        ;

        //count up
        print("up \n");
        int row_to_search = last_move_row;
        print("Checking diagonal \n");
        for (auto itr = board.rbegin() + (columns - column + 1); itr != board.rend(); itr++)
        {
            if (++row_to_search > rows - 1)
                break;
            print(itr->at(row_to_search), "\n");
            printi(row_to_search);
            print("\n");

            if (itr->at(row_to_search) == player)
                u_count++;
            else
                break;
        }
        //count down
        print("down \n");
        row_to_search = last_move_row;
        for (auto itr = board.begin() + column - 1; itr != board.end(); itr++)
        {
            if (row_to_search < 0)
                break;
            print(itr->at(row_to_search), "\n");
            printi(row_to_search);
            print("\n");
            if (itr->at(row_to_search) == player)
                d_count++;
            else
                break;

            row_to_search--;
        }
        if (u_count + d_count >= 4)
            return true;
        return false;
    }

    bool check_right_diagonal(string2dvector board, int column, string player)
    {
        int u_count = 0; //up
        int d_count = 0; //down
        int last_move_row;
        auto itr = std::find(board.at(column - 1).rbegin(), board.at(column - 1).rend(), player);
        last_move_row = rows - 1 - (itr - board.at(column - 1).rbegin());
        ;

        //count down
        print("up \n");
        int row_to_search = last_move_row;
        print("Checking diagonal \n");
        for (auto itr = board.rbegin() + (columns - column + 1); itr != board.rend(); itr++)
        {
            if (--row_to_search < 0)
                break;
            print(itr->at(row_to_search), "\n");
            printi(row_to_search);
            print("\n");

            if (itr->at(row_to_search) == player)
                d_count++;
            else
                break;
        }
        //count down
        print("down \n");
        row_to_search = last_move_row;
        for (auto itr = board.begin() + column - 1; itr != board.end(); itr++)
        {
            if (row_to_search > rows-1)
                break;
            print(itr->at(row_to_search), "\n");
            printi(row_to_search);
            print("\n");
            if (itr->at(row_to_search) == player)
                u_count++;
            else
                break;

            row_to_search++;
        }
        if (u_count + d_count >= 4)
            return true;
        return false;
    }
};
