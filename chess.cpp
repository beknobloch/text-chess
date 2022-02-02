/*
    2-PLAYER CHESS PROGRAM

    ENGR 116 Final Project
    by Benjamin Knobloch (solo project, Recitation Section RH)

    Program description:
        * Allows two players to play chess in the terminal.
        * The board is displayed as a grid of characters.
            White pieces are uppercase letters and black letters are lowercase.
            Empty squares are rendered as 0s.
        * The program will not allow players to make invalid moves.
        * The program automatically handles checks and checkmates.
        * You can save and load games to a text file. Multiple games can be saved by using user-chosen IDs.
        * NOTE: The program does not support castling, but does support promotion and en'passant.
        * NOTE: The program has an "undo move" feature, but it is currently buggy.
    
    How To Move Pieces:
        * Enter the square of the piece you would like to move, then enter the destination square.
        * Ranks are denoted by the numbers 1 through 8, and the files are denoted by letters 'a' through 'h'.
        * Enter the file and then the rank with no space. (For example, enter "e4");

    Sources Used:
        * http://tutors.ics.uci.edu/index.php/tutor-resources/81-cpp-resources/122-cpp-ref-pointer-operators 
        * https://stackoverflow.com/questions/12902751/how-to-clone-object-in-c-or-is-there-another-solution
        * All logic and implementation of chess mechanics is my own.

    To Do:
        * Implement castling
        * Fix undo issue
*/

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

// Class "Piece" is a base class for all pieces on the board.
class Piece
{
    public:
        bool is_white;
        bool vulnerable_to_en_passant;
        bool just_completed_en_passant;
        Piece()
        {
            is_white = true;
            vulnerable_to_en_passant = false;
            just_completed_en_passant = false;
        }
        // If true, the piece is white.
        Piece(bool color)
        {
            is_white = color;
        }
        // move() in each subclass takes origin, destination, and board state, and decides if the piece's move is valid.
        virtual bool move(int origin, int destination, Piece ** current_board) {return false;};
        // capture() exists for pawns, since they capture differently than they otherwise move.
        virtual bool capture(int location, int destination, Piece ** currentBoard) {return move(location, destination, currentBoard);};

        // An empty board still contains a Piece, but it's represented by a 0.
        virtual char display() {return '0';}

        virtual std::string name()  {return "undefined";}

};

// Class Pawn extends class Piece.
class Pawn : public Piece
{
    public:
        bool first_move = true;
        Pawn(bool color) : Piece(color) {};
        bool move(int location, int destination, Piece ** currentBoard) override
        {
            int direction = 1;
            if (!is_white)   direction = -1;
            if (first_move)
            {
                first_move = false;
                if (destination == location + 8 * direction)                                                                return true;
                if (destination == location + 16 * direction && currentBoard[location + 8 * direction]->display() == '0')
                {
                    vulnerable_to_en_passant = true;
                    return true;
                }
                return false;
            }
            vulnerable_to_en_passant = false;
            if (destination == location + 7 * direction && destination % 8 != 7)
            {
                if (currentBoard[location - 1]->name() == "pawn" && currentBoard[location - 1]->vulnerable_to_en_passant)
                {
                    just_completed_en_passant = true;
                    return true;
                }
            }
            if (destination == location + 9 * direction && destination % 8 != 0)
            {
                if (currentBoard[location + 1]->name() == "pawn" && currentBoard[location + 1]->vulnerable_to_en_passant)
                {
                    just_completed_en_passant = true;
                    return true;
                }
            }

            if (destination == location + 8 * direction)                                                                    return true;
            return false;
        }
        bool capture(int location, int destination, Piece ** currentBoard) override
        {
            int direction = 1;
            if (!is_white)   direction = -1;
            if ((destination == location + 7 * direction && destination % 8 != 7) || (destination == location + 9 * direction && destination % 8 != 0)) return true;
            else                                                                                                                                        return false;
            return false;
        }
        char display () override
        {
            if (is_white)   return 'P';
            else            return 'p';
        }
        std::string name () {return "pawn";}
};

// Class Rook extends class Piece.
class Rook : public Piece
{
    public:
        Rook(bool color) : Piece(color) {};
        bool move(int location, int destination, Piece ** currentBoard) override
        {
            if (destination % 8 == location % 8)
            {
                int direction = 1;
                if (destination < location) direction = -1;
                for (int i = location + 8 ; i != destination ; i += 8 * direction)
                {
                    if (currentBoard[i]->display() != '0')  return false;
                }
                return true;
            }
            if (destination / 8 == location / 8)
            {
                int direction = 1;
                if (destination < location) direction = -1;
                for (int i = location + direction ; i != destination ; i += direction)
                {
                    if (currentBoard[i]->display() != '0') return false;
                }
                return true;
            }
            return false;
        }
        char display() override
        {
            if (is_white)   return 'R';
            else            return 'r';
        }
        std::string name() {return "rook";}
};

// Class Knight extends class Piece.
class Knight : public Piece
{
    public:
        Knight(bool color) : Piece(color) {};
        bool move(int location, int destination, Piece ** currentBoard) override
        {
            int w = std::abs((location % 8) - (destination % 8));
            int l = std::abs((location / 8) - (destination / 8));
            if (w == 1 && l == 2) return true;
            if (w == 2 && l == 1) return true;
            return false;
        }
        char display () override
        {
            if (is_white)   return 'N';
            else            return 'n';
        }
        std::string name () {return "knight";}
};

// Class Bishop extends class Piece.
class Bishop : public Piece
{
    public:
        Bishop(bool color) : Piece(color) {};
        bool move(int location, int destination, Piece ** currentBoard) override
        {
            int w = std::abs(location % 8 - destination % 8);
            int l = std::abs(location / 8 - destination / 8);

            if (w == 0) return false;
            if (w != l) return false;

            int direction_w = 1;
            if (location % 8 > destination % 8) direction_w = -1;
            int direction_l = 1;
            if (location / 8 > destination / 8) direction_l = -1;

            for (int i = location + direction_w + 8 * direction_l ; i != destination ; i += direction_w + 8 * direction_l)
            {
                if (i < 8 || i > 55 || i % 8 == 0 || i % 8 == 7)    return false;
                if (currentBoard[i]->display() != '0')              return false;
            }
            return true;
        }
        char display () override
        {
            if (is_white)   return 'B';
            else            return 'b';
        }
        std::string name () {return "bishop";}
};

// Class Queen extends class Piece.
class Queen : public Piece
{
    public:
        Queen(bool color) : Piece(color) {};
        bool move(int location, int destination, Piece ** currentBoard) override
        {
            Rook * test_rook = new Rook(true);
            Bishop * test_bishop = new Bishop(true);
            if (test_rook->move(location, destination, currentBoard))    return true;
            if (test_bishop->move(location, destination, currentBoard))  return true;
            return false;
        }
        char display () override
        {
            if (is_white)   return 'Q';
            else            return 'q';
        }
        std::string name () {return "queen";}
};

// Class King extends class Piece.
class King : public Piece
{
    public:
        King(bool color) : Piece(color) {};
        bool move(int location, int destination, Piece ** currentBoard) override
        {
            int w = std::abs(location % 8 - destination % 8);
            int l = std::abs(location / 8 - destination / 8);

            if (w > 1)  return false;
            if (l > 1)  return false;
            return true;
        }
        char display () override
        {
            if (is_white)   return 'K';
            else            return 'k';
        }
        std::string name () {return "king";}
};

// Class "Board" represents the board
class Board
{
    public:
        // Creates an array of length 64 where each element represents a square on the board. Numbering travels down the ranks and then the files.
        Piece * squares [64];

        Board()
        {
            // Create empty board.
            for (int i = 0 ; i < 64 ; i++)  squares[i] = new Piece();

            // Populate board with pieces in standard initial positions.            
            
            squares[0] = new Rook(true);
            squares[1] = new Knight(true);
            squares[2] = new Bishop(true);
            squares[3] = new Queen(true);
            squares[4] = new King(true);
            squares[5] = new Bishop(true);
            squares[6] = new Knight(true);
            squares[7] = new Rook(true);
            for (int i = 8 ; i < 16 ; i++)      squares[i] = new Pawn(true);

            for (int i = 48 ; i < 56 ; i++)     squares[i] = new Pawn(false);
            squares[56] = new Rook(false);
            squares[57] = new Knight(false);
            squares[58] = new Bishop(false);
            squares[59] = new Queen(false);
            squares[60] = new King(false);
            squares[61] = new Bishop(false);
            squares[62] = new Knight(false);
            squares[63] = new Rook(false);
            

        }

        // If the move results in a promotion, execute that promotion.
        void check_promotion(Piece p, int location, bool is_white)
        {
            if (((is_white && location >= 56) || (!is_white && location <= 7)) && (p.display() == 'p' || p.display() == 'P'))
            {
                std::string choice = "";
                std::cout << "\n\nPromotion! What piece would you like to promote to?";
                while (choice != "q" && choice != "b" && choice != "n" && choice != "r")
                {
                    std::cout << "\nType \"q\" for queen, \"b\" for bishop, etc.: ";
                    std::cin >> choice;
                }
                if (choice == "q")      squares[location] = new Queen(is_white);
                else if (choice == "b") squares[location] = new Bishop(is_white);
                else if (choice == "n") squares[location] = new Knight(is_white);
                else                    squares[location] = new Rook(is_white);
            }
        }

        // move() in Board handles the move on each turn.
        int move(bool white_turn, int origin, int destination)
        {
            // If the origin and destination locations are identical, return error code -1.
            if (origin == destination)
            {
                return -1;
            }
            // If the origin location contains no piece, return error code -2.
            if (squares[origin]->display() == '0')
            {
                return -2;
            }
            // If the origin location contains a piece of the wrong color, return error code -3.
            if (white_turn != squares[origin]->is_white)
            {
                return -3;
            }
            // If the destination location is empty, use the piece's .move() method.
            if (squares[destination]->display() == '0')
            {
                if (squares[origin]->move(origin, destination, squares))
                {
                    squares[destination] = squares[origin];
                    check_promotion(squares[origin], destination, white_turn);
                    squares[origin] = new Piece();
                    
                    // Handle en_passant situations.
                    if (squares[destination]->just_completed_en_passant)
                    {
                        if (destination > origin)
                        {
                            squares[destination - 8] = new Piece();
                            (*squares)[destination].just_completed_en_passant = false;
                        }
                        else
                        {
                            squares[destination + 8] = new Piece();
                            (*squares)[destination].just_completed_en_passant = false;
                        }
                    }
                    
                    return 1;
                }
                // If the piece can't move to the destination, return error code -4.
                return -4;
            }
            else
            {
                // If the destination location contains a piece of the same color as the current player's piece, return error code -5.
                if (white_turn == squares[destination]->is_white)
                {
                    return -5;
                }
                // If the destination location contains a piece of the opposite color, use the piece's .capture() method.
                if (squares[origin]->capture(origin, destination, squares))
                {
                    squares[destination] = squares[origin];
                    check_promotion(squares[origin], destination, white_turn);
                    squares[origin] = new Piece();
                    return 2;
                }
                return -7;
            }
            return -6;
        }
        
        // Display the board by printing each piece's char symbol using .display().
        void render() {
            std::cout << "\n\t\t\t    a b c d e f g h";
            std::cout << "\n\t\t\t  -------------------";
            for (int i = 56 ; i > -1 ; i-= 8)
            {
                for (int j = 0 ; j < 8 ; j++)
                {
                    if (j % 8 == 0) std::cout << "\n\t\t\t" << (i / 8) + 1 << " | ";
                    std::cout << squares[i + j]->display() << " ";
                    if (j % 8 == 7) std::cout << "|";
                }
            }
            std::cout << "\n\t\t\t  -------------------\n";
        }
        
        // Return a copy, or a clone, of the board.
        Board clone()
        {
            return (*this);
        }

};

// chess_notation_to_integer() accepts a string in chess notation (i.e. "e4") and returns the corresponding array location.
int chess_notation_to_integer(std::string code) {
    int file = -1;
    std::string str_file = code.substr(0, 1);
    std::string letters = "abcdefgh";
    std::string u_letters = "ABCDEFGH";
    
    file = letters.find(str_file, 0);
    if (file == std::string::npos)   file = u_letters.find(str_file, 0);

    int rank = std::stoi(code.substr(1, 1));

    return (8 * (rank - 1)) + file;
}

// check_for_check() returns true if the king of the given color is in check.
bool check_for_check(Board * currentBoard, bool white) {
    
    int king_location = -1;
    for (int i = 0 ; i < 64 ; i++)
    {
        if ((white && currentBoard->squares[i]->display() == 'K') || (!white && currentBoard->squares[i]->display() == 'k')) {
            king_location = i;
        }
    }

    Piece king = currentBoard->squares[king_location];

    for (int i = 0 ; i < 64 ; i++)
    {
        if (currentBoard->squares[i]->display() != '0' && currentBoard->squares[i]->is_white != white)
        {
            if (currentBoard->squares[i]->capture(i, king_location, currentBoard->squares))   return true;
        }
    }

    return false;

}

// check_for_checkmate() returns true if the king of the given color is in checkmate.
bool check_for_checkmate(Board currentBoard, bool whites_turn) {

    int king_location = -1;
    for (int i = 0 ; i < 64 ; i++)
    {
        if ((whites_turn && currentBoard.squares[i]->display() == 'K') || (!whites_turn && currentBoard.squares[i]->display() == 'k'))
        {
            king_location = i;
        }
    }

    Piece king = currentBoard.squares[king_location];

    for (int i = 0 ; i < 64 ; i++)
    {
        for (int j = 0 ; j < 64 ; j++)
        {
            Board b = currentBoard.clone();
            if (b.squares[i]->is_white == !whites_turn)
            {
                if (b.squares[i]->move(i, j, b.squares) || b.squares[i]->capture(i, j, b.squares))
                {
                    b.move(!whites_turn, i, j);
                    if (!check_for_check(&b, king_location))    return false;
                }
            }
        }
    }

    return true;

}

bool save_to_file(std::vector <std::string> moves, bool whites_turn, std::string id) {
    try
    {
        std::ofstream save_file;
        save_file.open("saved_chess_game_" + id, std::ios::trunc);

        if (whites_turn)
        {
            save_file << "w";
        } else
        {
            save_file << "b";
        }
        save_file << "\n";

        int counter = 0;
        for (std::string move : moves)
        {
            if (counter == 2) {
                save_file << "\n";
                counter = 0;
            }
            else if (counter == 1)    save_file << " ";

            save_file << move;

            counter++;
        }

        save_file.close();
        
        return true;
    }
    catch (...)
    {
        return false;
    }
    return false;
}

std::vector <std::string> load_from_file(std::string id) {
    try
    {
        std::ifstream load_file("saved_chess_game_" + id);

        std::vector <std::string> moves = {};
        std::string line;
        if (load_file.is_open())
        {
            while (std::getline (load_file, line))
            {
                moves.push_back(line);
            }
            load_file.close();
        }

        return moves;

    }
    catch (...)
    {
        return {};
    }
    
    return {};

}

// Main program loop allows players to create a board and play a game.
int main() {

    bool game_in_progress = true;

    std::cout << "\n\n\n\t\t\tHello! Welcome to Benjamin Knobloch's C++ Chess Program.\n\n\n";

    bool need_start_response = true;
    Board my_board;
    std::vector <std::string> move_list = {};
    bool whites_turn;

    std::string start_response = "";
    while (need_start_response)
    {
        std::cout << "\n\n\t\t\tWould you like to start a new game? (Y)es, (L)oad existing game, or (N)o: ";
        std::cin >> start_response;

        if (start_response == "l" || start_response == "L")
        {
            std::string saved_id;
            std::cout << "\n\n\t\t\tType the ID of a saved game to load it: ";
            std::cin >> saved_id;

            std::vector <std::string> saved_data = load_from_file(saved_id);
            if (saved_data.size() == 0)
            {
                std::cout << "\n\t\t\tError loading saved game.";
            }
            else
            {
                std::cout << "\n\t\t\tSaved game successfully located. Loading...";
                need_start_response = false;
                int i = 0;
                std::string o, d;
                bool alternator = true;
                for (std::string l : saved_data)
                {
                    if (i == 0)
                    {
                        if (l == "w")   whites_turn = true;
                        if (l == "b")   whites_turn = false;
                    }
                    else
                    {
                        o = l.substr(0, 2);
                        d = l.substr(3, 2);
                        if (my_board.move(alternator, chess_notation_to_integer(o), chess_notation_to_integer(d)) < 0)
                        {
                            std::cout << "\n\t\t\tThat save doesn't describe a valid game!";
                            need_start_response = true;
                            break;
                        }
                        else
                        {
                            move_list.push_back(o);
                            move_list.push_back(d);
                        }

                        alternator = !alternator;
                    }

                    i++;
                }
            }

        }
        if (start_response == "n" || start_response == "N")
        {
            std::cout << "\n\n\t\t\tOkay. Have a good day!";
            game_in_progress = false;
            need_start_response = false;
        }
        if (start_response == "y" || start_response == "Y")
        {
            std::cout << "\n\n\t\t\tIf you wish to save your game at any point, type \"S\" instead of a move.";
            whites_turn = true;
            need_start_response = false;
        }
    }

    

    std::cout << "\n\n";
    my_board.render();
    
    Board board_copy;
    while (game_in_progress)
    {

        std::cout << "\n\t====================\n";

        if (whites_turn)    std::cout << "\n\tIt is white's turn. (uppercase characters)";
        else                std::cout << "\n\tIt is black's turn. (lowercase characters)";

        bool looking_for_valid_move = true;

        std::string o, d;
        while (looking_for_valid_move)
        {

            std::cout << "\n\tWhat piece would you like to move? Or would you like to (U)ndo, (V)iew move list, or (S)ave this game?: ";
            std::cin >> o;

            if (o == "s" || o == "S")
            {             // Save game.
                std::string save_id;
                std::cout << "\n\nPlease give your saved game a string ID. When you want to load this game, use the ID to do so: ";
                std::cin >> save_id;
                while (save_id == "")
                {
                    std::cout << "\nPlease enter an ID: ";
                    std::cin >> save_id;
                }
                if(save_to_file(move_list, whites_turn, save_id))   std::cout << "\n\n\t\t\tSave successful.";
                else                                                std::cout << "\n\n\t\t\tSave failed. Please try again.";
            }
            else if (o == "u" || o == "U")
            {
                my_board = board_copy.clone();
                for (int i = 0 ; i < 2 ; i ++)
                {
                    move_list.pop_back();
                }
                std::cout << "\n\tUndo move successful.\n";
                looking_for_valid_move = false;
            }
            else if (o == "v" || o == "V")
            {
                int iterator = 0;
                bool alternator = true;
                std::cout << "\n\n\t====================\n";
                for (std::string move : move_list)
                {
                    if (iterator == 2)
                    {
                        std::cout << "\n";
                        iterator = 0;
                    }
                    if (iterator == 1)
                    {
                        std::cout << " to ";
                    }
                    if (iterator == 0)
                    {
                        if (alternator) std::cout << "\tWhite moved from ";
                        else            std::cout << "\tBlack moved from ";
                        alternator = !alternator;
                    }

                    std::cout << move;

                    iterator++;
                }
                if (move_list.size() == 0) {
                    std::cout << "This is a brand new game! There have been no moves so far.";
                }
                std::cout << "\n\t====================\n\n";
                whites_turn = !whites_turn;
                looking_for_valid_move = false;
            }
            else
            {
                std::cout << "\tWhere do you move it?: ";
                std::cin >> d;

                board_copy = my_board.clone();

                try
                {
                    int move_result = my_board.move(whites_turn, chess_notation_to_integer(o), chess_notation_to_integer(d));
                    if (move_result > 0)
                    {
                        move_list.push_back(o);
                        move_list.push_back(d);
                        looking_for_valid_move = false;
                    }
                    else
                    {
                        std::cout << "\n\n\tThat move is not valid.";
                        if (move_result == -1)  std::cout << " Origin square and destination square must be distinct.";
                        if (move_result == -2)  std::cout << " The origin square must contain a piece.";
                        if (move_result == -3)  std::cout << " The origin square must contain a piece of your color.";
                        if (move_result == -4)  std::cout << " That piece cannot be moved to the designated destination square.";
                        if (move_result == -5)  std::cout << " The destination square cannot contain one of your own pieces.";
                        if (move_result == -7)  std::cout << " That piece can't capture like that.";
                    }
                }
                catch (...)
                {
                    std::cout << "\n\n\tThat move is not valid. Enter piece locations as \"A1\", \"E4\", etc.";
                }

                if (check_for_check(&my_board, whites_turn))
                {
                    std::cout << "\n\n\tYou can't end your turn in check.";
                    my_board = board_copy;
                    looking_for_valid_move = true;
                }
            }
        }
        
        my_board.render();

        if (check_for_check(&my_board, !whites_turn))
        {
            if (check_for_checkmate(my_board, whites_turn))
            {
                std::cout << "\n\n\t\t======================================";
                std::cout << "\n\t\t\t\tCHECKMATE!\n\n";
                std::cout << "\n\t\t======================================";
                game_in_progress = false;
            } else  std::cout << "\n\t\t\t\tCheck!\n";
        }

        whites_turn = !whites_turn;
    }

    return 1;
}