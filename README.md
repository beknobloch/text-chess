# Text-Based 2-Player Chess Game
a 2-player chess game you can play in your terminal, made with C++

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
