#include <iostream>

#define HIT 0
#define STAND 1
#define DOUBLE 2 
#define SPLIT 3  

/* NOTE: The strategy implemented in this program is a modified version of the "Super-Easy" Blackjack strategy accessed from http://blackjackcalculation.com/.
The getRecommendedPlayerAction function follows these guidelines:
    * [1] If you have a pair of Aces or 8's, split them. 
    * [2] Double down on 9 or 10 vs. a dealer low card (i.e. 2, 3, 4, 5 , 6) and always double down on 11. 
    * [3] Hit on soft 17 or less and stand on soft 18 or more. 
    * [4] Against a dealer low card (i.e. 2, 3, 4, 5, 6), "never bust" (i.e. stand on 12 or more). 
    * [5] Against a dealer high card (i.e. 7, 8, 9, 10, A), "mimic the dealer" (i.e. hit until you get at least 17). 
    */ 

/* Function: getRecommendedPlayerAction
Input(s): 
    * int *playercards: a pointer to an array of integer values of the player's cards, ranging from 1-10 (A = 1, J = Q = K = 10)
    * int numPlayerCards: number of cards a player has in his/her hand
    * int dealerCard: an integer representing the value of the dealer's up card
Output(s):
    * HIT, STAND, DOUBLE, SPLIT
*/

int getRecommendedPlayerAction(int *playerCards, int numPlayerCards, int dealerCard) { 
    
    int handValue = 0 // value of player's current hand
    int softValue = -1 

    for (int i = 0; i < numPlayerCards; i++) {
        handValue += playerCards[i];
        if (playerCards[i] == 1) { 
            softValue = 0;
        } 
    }

    if (softValue == 0) { 
        softValue = handValue + 10
    }

    // Split
    if ((playerCards[0] == playersCards[1]) && (numPlayerCards == 2)) {
        if ((playersCards[0]) == 1) || (playerCards[0] == 8)) 
        {
            return SPLIT; // [1]
        }
    }

    // Double Down
    if ((numPlayerCards == 2) && ((handValue >= 0) && (handValue <= 21))){
        if ((handValue == 9) && (dealerCard < 7) && (dealerCard != 1)) {
            return DOUBLE; // [2]
        }
        if ((handValue == 10) && (dealerCard < 7) && (dealerCard != 1)) {
            return DOUBLE; // [2]
        }
        if ((handValue == 11) {
            return DOUBLE; // [2]
        }
    }

    // Hit and Stand
    if ((handValue >= 17) || (softValue >= 18)) {
        return STAND;
    }
    else if (handValue <= 11) {
        return HIT;
    }
    else if ((dealerCard < 7) && (dealerCard != 1)) {
        return STAND;
    }
    else {
        return HIT;
    }
}
