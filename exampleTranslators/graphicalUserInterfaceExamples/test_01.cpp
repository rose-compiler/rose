/*
 * TicTacToe by Eric Marcarelli 
 */

#include <stdlib.h>
#include <stdio.h>

class dummy {
  public:
    dummy operator <<(dummy & x) {
	return x;
    }
    dummy operator <<(const char *) {
	return dummy();
    }
    dummy operator <<(int) {
	return dummy();
    }
    dummy operator >>(int &) {
	return dummy();
    }
public:
   int n1;
   int n2;
protected:
    int a() {
        int x;
        x = 10;
        return x;
    }
private:
    int b();
    int m1;
    int m2;
};

int dummy::b() {
   return 0;
}



dummy cin, cout, endl;

int area[4][4] = { {0}, {0}, {0}, {0} };	// defined [4][4]

							// so row one can
							// be 1, not 0

namespace abc {
    int test() {
	cout << "hello";
	return 10;
    }
}

// see the functions for more info
void clear();
void wait(int);
int checkWin();
void draw(int);
void enter(int &);

int main()
{
    int i = 1;			// keep track of turns, end game after
    // nine
    int check = 0;		// hold the value returned by checkWin()
    int player = 1;		// keep track of who's playing (1 = you, 2 
    // = computer)
#pragma hello I am here
    clear();
    cout << "TicTacToe 1.0\n" << "By Eric Marcarelli";
    draw(checkWin() + 1);

    for (i = 1; i < 10; i += 1) {
        draw(i);
        enter(player);
        check = checkWin();
        if (check)
            i = 10;
    }
#pragma I am there
    draw(0);

    if (!check)
	cout << "\nNo winner!";
    else if (player == 2)	// since enter() changes player, 2 is now
	// you
	cout << "\nYou win!";
    else
	cout << "\nComputer wins!";

    wait(1);
    return 0;
}

void clear()
{
    system("cls");
}

void wait(int t)
{
    /*
     * waits for user to press enter.
     * 
     * usage:
     * 
     * wait(1) - display Press enter to continue...
     * 
     * wait(0) - just wait (other non-one numbers will also work) 
     */

    if (t == 1)
	cout << "\n\nPress enter to continue...";

    getchar();
}

int checkWin()
{

    if ((area[1][1] == area[1][2]) && (area[1][1] == area[1][3])
	&& (area[1][1]))
	return area[1][1];
    if ((area[2][1] == area[2][2]) && (area[2][1] == area[2][3])
	&& (area[2][1]))
	return area[2][1];
    if ((area[1][1] == area[2][1]) && (area[1][1] == area[3][1])
	&& (area[1][1]))
	return area[1][1];
    if ((area[3][3] == area[3][1]) && (area[3][3] == area[3][2])
	&& (area[3][3]))
	return area[3][3];
    if ((area[1][1] == area[2][2]) && (area[1][1] == area[3][3])
	&& (area[1][1]))
	return area[1][1];
    if ((area[3][3] == area[2][3]) && (area[3][3] == area[1][3])
	&& (area[3][3]))
	return area[3][3];
    if ((area[2][2] == area[1][2]) && (area[2][2] == area[3][2])
	&& (area[2][2]))
	return area[2][2];
    if ((area[2][2] == area[3][1]) && (area[2][2] == area[1][3])
	&& (area[2][2]))
	return area[2][2];
    else
	return 0;
}

void enter(int &player)
{
    /*
     * lets the player enter a number
     * 
     * the first part is the human player, then the computer player goes.
     * the computer chooses a move by checking if there is a spot where
     * either it or the human player will win next turn. if there is no
     * way to win or block it will first go for the center space, then the 
     * others. on **very** slow computers the computer player will
     * actually look like it's entering a number.
     * 
     * usage: enter(player-number) 
     */


    int answ;
    cout << endl << endl;

  again:

    if (player == 1) {
	cout << "Your turn, enter a number: ";
	cin >> answ;
    }

    else if (player == 2) {
        cout << "Computer turn, enter a number: ";
        if ((area[1][1] > 0) && (area[2][2] == area[1][1])
            && (area[3][3] == 0))
            answ = 9;
        else if ((area[1][2] > 0) && (area[1][2] == area[2][2])
                 && (area[3][2] == 0))
            answ = 8;
        else if ((area[1][1] > 0) && (area[2][1] == area[1][1])
                 && (area[3][1] == 0))
            answ = 7;
        else if ((area[1][1] > 0) && (area[1][1] == area[1][2])
                 && (area[1][3] == 0))
            answ = 3;
        else if ((area[1][2] > 0) && (area[1][2] == area[1][3])
                 && (area[1][1] == 0))
            answ = 1;
        else if ((area[1][3] > 0) && (area[2][2] == area[1][3])
                 && (area[3][1] == 0))
            answ = 7;
        else if ((area[2][1] > 0) && (area[2][2] == area[2][1])
                 && (area[2][3] == 0))
            answ = 6;
        else if ((area[2][2] > 0) && (area[2][2] == area[2][3])
                 && (area[2][1] == 0))
            answ = 4;
        else if ((area[1][3] > 0) && (area[2][3] == area[1][3])
                 && (area[3][3] == 0))
            answ = 9;
        else if ((area[2][1] > 0) && (area[2][1] == area[3][1])
                 && (area[1][1] == 0))
            answ = 1;
        else if ((area[3][1] > 0) && (area[2][2] == area[3][1])
                 && (area[1][3] == 0))
            answ = 3;
        else if ((area[3][1] > 0) && (area[3][2] == area[3][1])
                 && (area[3][3] == 0))
            answ = 9;
        else if ((area[3][2] > 0) && (area[2][2] == area[3][2])
                 && (area[1][2] == 0))
            answ = 2;
        else if ((area[3][3] > 0) && (area[3][3] == area[2][2])
                 && (area[1][1] == 0))
            answ = 1;
        else if ((area[3][3] > 0) && (area[3][3] == area[3][2])
                 && (area[3][1] == 0))
            answ = 7;
        else if ((area[3][3] > 0) && (area[2][3] == area[3][3])
                 && (area[1][3] == 0))
            answ = 3;
        else if ((area[1][2] > 0) && (area[1][2] == area[3][2])
                 && (area[2][2] == 0))
            answ = 5;
        else if ((area[3][1] > 0) && (area[3][1] == area[1][3])
                 && (area[2][2] == 0))
            answ = 5;
        else if ((area[1][1] > 0) && (area[3][3] == area[1][1])
                 && (area[2][2] == 0))
            answ = 5;
        else if ((area[1][1] > 0) && (area[1][1] == area[3][1])
                 && (area[2][1] == 0))
            answ = 4;
        else if ((area[1][3] > 0) && (area[3][3] == area[1][3])
                 && (area[2][3] == 0))
            answ = 6;
        else if ((area[2][1] > 0) && (area[2][3] == area[2][1])
                 && (area[2][2] == 0))
            answ = 5;
        else if ((area[1][2] == 2) && (area[1][1] == 0)
                 && (area[1][3] == 0))
            answ = 1;
        else if ((area[1][2] == 2) && (area[2][2] == 0)
                 && (area[3][2] == 0))
            answ = 5;
        else if ((area[1][1] == 2) && (area[1][2] == 0)
                 && (area[1][3] == 0))
            answ = 2;
        else if ((area[1][1] == 2) && (area[2][1] == 0)
                 && (area[3][1] == 0))
            answ = 4;
        else if ((area[2][2] == 2) && (area[1][3] == 0)
                 && (area[3][1] == 0))
            answ = 3;
        else if ((area[2][2] == 2) && (area[1][1] == 0)
                 && (area[3][3] == 0))
            answ = 1;
        else if ((area[2][2] == 2) && (area[1][2] == 0)
                 && (area[3][2] == 0))
            answ = 2;
        else if ((area[3][1] == 2) && (area[2][2] == 0)
                 && (area[1][3] == 0))
            answ = 5;
        else if ((area[3][1] == 2) && (area[3][2] == 0)
                 && (area[3][3] == 0))
            answ = 8;
        else if ((area[3][1] == 2) && (area[2][1] == 0)
                 && (area[1][1] == 0))
            answ = 4;
        else if ((area[1][1] == 2) && (area[2][2] == 0)
                 && (area[3][3] == 0))
            answ = 5;
        else if ((area[3][3] == 2) && (area[2][3] == 0)
                 && (area[1][3] == 0))
            answ = 6;
        else if ((area[3][3] == 2) && (area[2][2] == 0)
                 && (area[1][1] == 0))
            answ = 5;
        else if ((area[3][3] == 2) && (area[3][2] == 0)
                 && (area[3][1] == 0))
            answ = 8;
        else if ((area[3][2] == 2) && (area[2][2] == 0)
                 && (area[1][2] == 0))
            answ = 5;
        else if ((area[1][3] == 2) && (area[2][2] == 0)
                 && (area[1][1] == 0))
            answ = 5;
        else if ((area[1][3] == 2) && (area[2][3] == 0)
                 && (area[3][3] == 0))
            answ = 6;
        else if ((area[1][3] == 2) && (area[1][1] == 0)
                 && (area[1][2] == 0))
            answ = 1;
        else if ((area[3][2] == 2) && (area[3][1] == 0)
                 && (area[3][3] == 0))
            answ = 9;
        else if (!area[2][2])
            answ = 5;
        else if (!area[1][1])
            answ = 1;
        else if (!area[1][2])
            answ = 2;
        else if (!area[1][3])
            answ = 3;
        else if (!area[2][1])
            answ = 4;
        else if (!area[2][3])
            answ = 6;
        else if (!area[3][1])
            answ = 7;
        else if (!area[3][2])
            answ = 8;
        else if (!area[3][3])
            answ = 9;

        cout << answ;
    }

    if ((answ == 1) && (area[1][1] == 0))
	area[1][1] = player;
    else if ((answ == 2) && (area[1][2] == 0))
	area[1][2] = player;
    else if ((answ == 3) && (area[1][3] == 0))
	area[1][3] = player;
    else if ((answ == 4) && (area[2][1] == 0))
	area[2][1] = player;
    else if ((answ == 5) && (area[2][2] == 0))
	area[2][2] = player;
    else if ((answ == 6) && (area[2][3] == 0))
	area[2][3] = player;
    else if ((answ == 7) && (area[3][1] == 0))
	area[3][1] = player;
    else if ((answ == 8) && (area[3][2] == 0))
	area[3][2] = player;
    else if ((answ == 9) && (area[3][3] == 0))
	area[3][3] = player;
    else {
	cout <<
	    "\nSorry, space is taken or number is out of range. Choose again.\n\n";
	goto again;
    }
    if (player == 1)
	player = 2;
    else
	player = 1;
}

void draw(int turn)
{
    /*
     * draws the board, an X is for you, an O is the computer's.
     * 
     * usage:
     * 
     * draw(0) - just display the board
     * 
     * draw(othernumber) - displays that it is turn <othernumber> 
     */

    clear();
    if (turn != 0)
	cout << "Turn: " << turn << "\n";
    cout << "+---+---+---+\n";
    cout << "| ";
    if (area[1][1] == 0)
	cout << "1";
    else if (area[1][1] == 1)
	cout << "X";
    else
	cout << "O";
    cout << " | ";
    if (area[1][2] == 0)
	cout << "2";
    else if (area[1][2] == 1)
	cout << "X";
    else
	cout << "O";
    cout << " | ";
    if (area[1][3] == 0)
	cout << "3";
    else if (area[1][3] == 1)
	cout << "X";
    else
	cout << "O";
    cout << " |\n";
    cout << "+---+---+---+\n";
    cout << "| ";
    if (area[2][1] == 0)
	cout << "4";
    else if (area[2][1] == 1)
	cout << "X";
    else
	cout << "O";
    cout << " | ";
    if (area[2][2] == 0)
	cout << "5";
    else if (area[2][2] == 1)
	cout << "X";
    else
	cout << "O";
    cout << " | ";
    if (area[2][3] == 0)
	cout << "6";
    else if (area[2][3] == 1)
	cout << "X";
    else
	cout << "O";
    cout << " |\n";
    cout << "+---+---+---+\n";
    cout << "| ";
    if (area[3][1] == 0)
	cout << "7";
    else if (area[3][1] == 1)
	cout << "X";
    else
	cout << "O";
    cout << " | ";
    if (area[3][2] == 0)
	cout << "8";
    else if (area[3][2] == 1)
	cout << "X";
    else
	cout << "O";
    cout << " | ";
    if (area[3][3] == 0)
	cout << "9";
    else if (area[3][3] == 1)
	cout << "X";
    else
	cout << "O";
    cout << " |\n";
    while (1) {
        cout << "+---+---+---+\n";
    }
}
