#include <stdio.h>

char square[10] = { 'o', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

int checkwin();
void board();

int main()
{
    int player = 1, i = -1;

    char choice[2];
    char mark;
    char *winner;
    char *playerStr;
    while (i == -1)
    {
        board();
        player = (player % 2) ? 1 : 2;

        itoa(player, 10, playerStr);

        interrupt(0x21, 0, "Player ", 0, 0); interrupt(0x21, 0, playerStr, 0, 0); interrupt(0x21, 0, ", enter a number: ", 0, 0); 
        interrupt(0x21, 1, choice, 0, 0);

        mark = (player == 1) ? 'X' : 'O';

        if (choice[0] == '1' && square[1] == '1')
            square[1] = mark;
            
        else if (choice[0] == '2' && square[2] == '2')
            square[2] = mark;
            
        else if (choice[0] == '3' && square[3] == '3')
            square[3] = mark;
            
        else if (choice[0] == '4' && square[4] == '4')
            square[4] = mark;
            
        else if (choice[0] == '5' && square[5] == '5')
            square[5] = mark;
            
        else if (choice[0] == '6' && square[6] == '6')
            square[6] = mark;
            
        else if (choice[0] == '7' && square[7] == '7')
            square[7] = mark;
            
        else if (choice[0] == '8' && square[8] == '8')
            square[8] = mark;
            
        else if (choice[0] == '9' && square[9] == '9')
            square[9] = mark;
            
        else
        {
            interrupt(0x21, 0, "Invalid move\r\n", 0, 0);

            player--;
        }
        i = checkwin();

        player++;
    }
    
    board();
    
    if (i == 1)
        itoa(player, 10, winner);
        interrupt(0x21, 0, "==>Player ", 0, 0); interrupt(0x21, 0, winner, 0, 0); interrupt(0x21, 0, " win\r\n", 0, 0);
    else
        interrupt(0x21, 0, "==>Game draw\r\n", 0, 0);

    return 0;
}

/*********************************************

FUNCTION TO RETURN GAME STATUS
1 FOR GAME IS OVER WITH RESULT
-1 FOR GAME IS IN PROGRESS
O GAME IS OVER AND NO RESULT
 **********************************************/

int checkwin()
{
    if (square[1] == square[2] && square[2] == square[3])
        return 1;
        
    else if (square[4] == square[5] && square[5] == square[6])
        return 1;
        
    else if (square[7] == square[8] && square[8] == square[9])
        return 1;
        
    else if (square[1] == square[4] && square[4] == square[7])
        return 1;
        
    else if (square[2] == square[5] && square[5] == square[8])
        return 1;
        
    else if (square[3] == square[6] && square[6] == square[9])
        return 1;
        
    else if (square[1] == square[5] && square[5] == square[9])
        return 1;
        
    else if (square[3] == square[5] && square[5] == square[7])
        return 1;
        
    else if (square[1] != '1' && square[2] != '2' && square[3] != '3' &&
        square[4] != '4' && square[5] != '5' && square[6] != '6' && square[7] 
        != '7' && square[8] != '8' && square[9] != '9')

        return 0;
    else
        return  - 1;
}


/*******************************************************************
FUNCTION TO DRAW BOARD OF TIC TAC TOE WITH PLAYERS MARK
 ********************************************************************/


void board()
{
    interrupt(0x21, 0, "\n\n\tTic Tac Toe\r\n\n", 0, 0);

    interrupt(0x21, 0, "Player 1 (X)  -  Player 2 (O)\r\n\n\n", 0, 0);


    interrupt(0x21, 0, "     |     |     \r\n", 0, 0);
    interrupt(0x21, 0, "  ", 0, 0); interrupt(0x21, 0, square[1], 0, 0); interrupt(0x21, 0, "  |  ", 0, 0); interrupt(0x21, 0, square[2], 0, 0); interrupt(0x21, 0, "  |  ", 0, 0); interrupt(0x21, 0, square[3], 0, 0); interrupt(0x21, 0, " \r\n", 0, 0);

    interrupt(0x21, 0, "_____|_____|_____\r\n", 0, 0);
    interrupt(0x21, 0, "     |     |     \r\n", 0, 0);

    interrupt(0x21, 0, "  ", 0, 0); interrupt(0x21, 0, square[4], 0, 0); interrupt(0x21, 0, "  |  ", 0, 0); interrupt(0x21, 0, square[5], 0, 0); interrupt(0x21, 0, "  |  ", 0, 0); interrupt(0x21, 0, square[6], 0, 0); interrupt(0x21, 0, " \r\n", 0, 0);

    interrupt(0x21, 0, "_____|_____|_____\r\n", 0, 0);
    interrupt(0x21, 0, "     |     |     \r\n", 0, 0);

    interrupt(0x21, 0, "  ", 0, 0); interrupt(0x21, 0, square[7], 0, 0); interrupt(0x21, 0, "  |  ", 0, 0); interrupt(0x21, 0, square[8], 0, 0); interrupt(0x21, 0, "  |  ", 0, 0); interrupt(0x21, 0, square[9], 0, 0); interrupt(0x21, 0, " \r\n", 0, 0);

    interrupt(0x21, 0, "     |     |     \r\n\n", 0, 0);
}