// BridgeBidBuddy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <vector>

using namespace std;

static const string p = "PASS";

class Card {
public:
	Card() {}
	Card(int r, int s) { rank = r; suit = s; }
	int rank;
	int suit;	
};


class Deck {
public:
	Deck();
	Card deal_a_card();
private:
	int cards[52];
	int iCard;
	void shuffle();
};


Deck::Deck() {
	srand(time(NULL));
	for (int i = 0; i < 52; ++i) {
		cards[i] = i;
	}
	shuffle();
}

void Deck::shuffle() {
	iCard = 0;
	random_shuffle(cards, cards + 52);
}

Card Deck::deal_a_card() {
	if (iCard > 51) {		
		shuffle();
	}
	int r = cards[iCard] % 13;
	int s = cards[iCard++] / 13;
	return Card(r, s);
}

class Display {
public:
	int a_suit;
	vector<int> iclubs, idiamonds, ihearts, ispades;
	vector<string> clubs, diamonds, hearts, spades;
	void sort_n_display(Card* pCards, int a_suit);
};

void Display::sort_n_display(Card* pCards, int a_suit) {
	static const string aRanks[] = { " 2", " 3", " 4 ", " 5", " 6", " 7", " 8", " 9", " 10" , " J", " Q",
		" K", " A" };	
	vector<int> i_suits;
	vector<string> s_suits;
	for (int i = 0; i < 13; ++i) {	//	Init arrays
		int r = pCards[i].rank;
		int s = pCards[i].suit;
		switch (s)
		{
		case 0: iclubs.push_back(r);		break;
		case 1: idiamonds.push_back(r);		break;
		case 2: ihearts.push_back(r);		break;
		case 3: ispades.push_back(r);		break;
		}		
	}

	switch (a_suit)
	{
	case 0: i_suits = iclubs; s_suits = clubs;			break;
	case 1: i_suits = idiamonds; s_suits = diamonds;	break;
	case 2: i_suits = ihearts; s_suits = hearts;		break;
	case 3: i_suits = ispades; s_suits = spades;		break;
	}
	sort(i_suits.begin(), i_suits.end());
	for (int j = 0; j < i_suits.size(); ++j) {
		s_suits.push_back(aRanks[i_suits[j]]);
	}
	for (int k = 0; k < s_suits.size(); ++k) {
		cout << s_suits[k];
	}
}

class Eval {
public:
	Eval(Card* pCards);		
	int bid_tracker(int n, int points, int bids[4], Card* pCards, int *bidMax);
	vector <string> path;
	string add_path;
	void paths(vector <string> path, string add_path);
	string bidding(int n, int points, int bids[4], Card* pCards, int level, int *bidMax);		
	string level_bidding_r(int n, int points, int bids[4], Card* pCards, int level, int *bidMax);
	string suit_bid(int n, int points, int bids[4], Card* pCards, int *plevel);	
	string club_response(int n, int points, int bids[4], Card* pCards, int *plevel);
	string club_r_range_1(int n, int points, int bids[4], Card* pCards, int *plevel);
	string club_r_range_2(int n, int points, int bids[4], Card* pCards, int *plevel);
	string club_r_range_3(int n, int points, int bids[4], Card* pCards, int *plevel);
	string diamond_response(int n, int points, int bids[4], Card* pCards, int *plevel);
	string diamond_r_range_1(int n, int points, int bids[4], Card* pCards, int *plevel);
	string diamond_r_range_2(int n, int points, int bids[4], Card* pCards, int *plevel);
	string diamond_r_range_3(int n, int points, int bids[4], Card* pCards, int *plevel);
	string heart_response(int n, int points, int bids[4], Card* pCards, int *plevel);
	string heart_r_range_1(int n, int points, int bids[4], Card* pCards, int *plevel);
	string heart_r_range_2(int n, int points, int bids[4], Card* pCards, int *plevel);
	string spade_response(int n, int points, int bids[4], Card* pCards, int *plevel);	
	string spade_r_range_1(int n, int points, int bids[4], Card* pCards, int *plevel);
	string spade_r_range_2(int n, int points, int bids[4], Card* pCards, int *plevel);
	string NT_response(int n, int points, int bids[4], Card* pCards, int level);
	string one_NT_response(int n, int points, int bids[4], Card* pCards, int level);
	string one_NT_r_range_1(int n, int points, int bids[4], Card* pCards, int level);	
	string trump_or_NT(int n, int points, int bids[4], Card* pCards, int level);	
	int points();	
	void suit_count();
	int longest_suit(int n, int points, int eliminate_suits, Card* pCards, int *plevel);
	int longest_suit_after_NT(int n, int points, int eliminate_suits, Card* pCards, int level);
private:
	int rankCounts[13];
	int suitCounts[4];	
	bool is_NT(int points, Card* pCards, int *plevel);
	bool is_NT_r(int points, int *level);
	bool is_no_voids();
	bool is_no_singletons();
	bool is_no_two_doubletons();		
	bool is_suit_stopper(Card* pCards, int a_suit);
	bool is_three_stopper(Card* pCards);
};

Eval::Eval(Card* pCards) {
	for (int i = 0; i < 13; ++i) {	//	Clear arrays
		rankCounts[i] = 0;
	}
	for (int i = 0; i < 4; ++i) {
		suitCounts[i] = 0;
	}
	for (int i = 0; i < 13; ++i) {	//	Init arrays
		int r = pCards[i].rank;
		int s = pCards[i].suit;
		++rankCounts[r];
		++suitCounts[s];
	}
}

int Eval::bid_tracker(int n, int points, int bids[4], Card* pCards, int *bidMax) {
	int level = 1, s; 		
	if (n == 0) {
		s = longest_suit(0, points, -5, pCards, &level);
	}
	else {	
		bidMax = max_element(bids, bids + n );		
		s = longest_suit(n, points, *bidMax , pCards, &level);			
	}
	return s;
}

void Eval::paths(vector <string> path, string add_path) {
	path.push_back(add_path);	
}

string Eval::bidding(int n, int points, int bids[4], Card* pCards, int level, int *bidMax) {		
	if (n == 0 || n == 1) {
		if (points < 12) {			
			bids[n] = -6;
			return suit_bid(n, points, bids, pCards, &level);
		}
		else {
			return trump_or_NT(n, points, bids, pCards, level);			
		}		
	}
	else {
		//	Responsonse to partner n = 2, or n = 3.
		if (bids[n - 2] == -6 && points >= 12) {
			return trump_or_NT(n, points, bids, pCards, level);
		}
		if (points < 7) {
			if (bids[n - 2] == 4) {
				return NT_response(n, points, bids, pCards, level);
			}
			else if (points < 6) {
				bids[n] = -6;				
				return suit_bid(n, points, bids, pCards, &level);
			}
			else {
				return level_bidding_r(n, points, bids, pCards, level, bidMax);
			}
		}
		else {
			return level_bidding_r(n, points, bids, pCards, level, bidMax);
		}		
	}
}


string Eval::level_bidding_r(int n, int points, int bids[4], Card* pCards, int level, int *bidMax) {	
	switch (bids[n - 2])
	{
	case 0: return club_response(n, points, bids, pCards, &level);			break;
	case 1: return diamond_response(n, points, bids, pCards, &level);		break;
	case 2: return heart_response(n, points, bids, pCards, &level);			break;
	case 3: return spade_response(n, points, bids, pCards, &level);			break;
	case 4: return NT_response(n, points, bids, pCards, level);				break;
	case -6: ;																break;
	} 
	if (bidMax >= 0) {
		
		bids[n] = longest_suit(n, points, *bidMax, pCards, &level);
	}
	return trump_or_NT(n, points, bids, pCards, level);
}

string Eval::club_response(int n, int points, int bids[4], Card* pCards, int *plevel) {
	if (points >= 6 && points <= 10) {
		return club_r_range_1(n, points, bids, pCards, plevel);
	}
	else if (points >= 11 && points <= 16) {
		return club_r_range_2(n, points, bids, pCards, plevel);
	}
	else if (points >= 17) {
		return club_r_range_3(n, points, bids, pCards, plevel);
	}
	else {
		return "club response 4!";
	}
}

string Eval::club_r_range_1(int n, int points, int bids[4], Card* pCards, int *plevel) {
	//	points <= 6 && points <= 10
	if (*plevel == 1 && bids[n - 1] < 2 && (suitCounts[2] >= 4 || suitCounts[3] >= 4)) {
		if (suitCounts[2] >= 4 && (suitCounts[2] > suitCounts[3]) && bids[n - 1] < 2) {
			bids[n] = 2;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else if (bids[n - 1] < 3) {
			bids[n] = 3;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else {
			bids[n] = -6;
			return suit_bid(n, points, bids, pCards, plevel);
		}
	}
	else if (*plevel == 1) {
		{
			if (suitCounts[0] >= 5) {
				bids[n] = 0;
				*plevel = 2;
				return suit_bid(n, points, bids, pCards, plevel);
			}
			else if (is_NT(points, pCards,  plevel) && *plevel == 1) {
				bids[n] = 4;
				return suit_bid(n, points, bids, pCards, plevel);
			}
			else if (suitCounts[1] >= 4 && bids[n -1] < 1) {
				bids[n] = 1;
				return suit_bid(n, points, bids, pCards, plevel);
			}
			else {
				bids[n] = -6;
				return suit_bid(n, points, bids, pCards, plevel);
			}
		}
	}
	else {
		bids[n] = -6;
		return suit_bid(n, points, bids, pCards, plevel);
	}
}

string Eval::club_r_range_2(int n, int points, int bids[4], Card* pCards, int *plevel) {
	if (suitCounts[0] >= 5 && *plevel == 1) {
		bids[n] = 0;
		*plevel = 2;
		return suit_bid(n, points, bids, pCards, plevel);
	}
	if (points == 11 || points == 12) {						//	There is a hole here!!
		if (suitCounts[2] < 4 || suitCounts[3] < 4) {
			if (suitCounts[0] >= 5 && *plevel <3) {
				bids[n] = 0;
				*plevel = 3;
				return suit_bid(n, points, bids, pCards, plevel);
			}
			else {
				if (is_NT_r(points, plevel) && *plevel < 3) {
					bids[n] = 4;
					*plevel = 2;
					return suit_bid(n, points, bids, pCards, plevel);
				}
				else {
					return club_r_range_1(n, points, bids, pCards, plevel);
				}
			}
		}
		else {
			return club_r_range_1(n, points, bids, pCards, plevel);
		}	
	}
	else if (points >= 13 && points <= 16) {
		if (is_NT_r(points, plevel)) {
			bids[n] = 4;
			*plevel = 3;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else {
			return club_r_range_1(n, points, bids, pCards, plevel);
		}
	}
	else {
		return club_r_range_1(n, points, bids, pCards, plevel);
	}
}

string Eval::club_r_range_3(int n, int points, int bids[4], Card* pCards, int *plevel) {
	//	points >= 17 &&  5+ in any suit other than clubs. Jump to two level! Slam interest
	int LS;
	if (*plevel < 3) {
		if (*plevel == 1) {
			LS = longest_suit(n, points, -5, pCards, plevel);
			bids[n] = LS;
			*plevel = 2;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		if (*plevel == 2) {
			LS = longest_suit(n, points, bids[n - 1], pCards, plevel);
			bids[n] = LS;
			*plevel = 2;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else {
			bids[n] = -6;
			return suit_bid(n, points, bids, pCards, plevel);
		}
	}
	else {
		bids[n] = -6;
		return suit_bid(n, points, bids, pCards, plevel);
	}
}

string Eval::diamond_response(int n, int points, int bids[4], Card* pCards, int *plevel) {
	if (points >= 6 && points <= 10) {
		return diamond_r_range_1(n, points, bids, pCards, plevel);
	}
	else if (points >= 11 && points <= 16) {
		return diamond_r_range_2(n, points, bids, pCards, plevel);
	}
	else if (points >= 17) {
		return diamond_r_range_3(n, points, bids, pCards, plevel);
	}
	else {
		return "Diamond response 4!";
	}
}

string Eval::diamond_r_range_1(int n, int points, int bids[4], Card* pCards, int *plevel) {
	//	points >= 6 && points <= 10
	if (*plevel == 1 && bids[n - 1] < 2 && (suitCounts[3] > 4 || suitCounts[2] > 4)) {	
		if (suitCounts[2] > 4 && bids[n - 1] < 2) {
			bids[n] = 2;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else if (bids[n - 1] < 3) {
			bids[n] = 3;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else {
			bids[n] = -6;
			return suit_bid(n, points, bids, pCards, plevel);
		}
	}
	else if (*plevel == 1) {
		{
			if (suitCounts[1] >= 5 && bids[n - 1] < 1) {
				bids[n] = 1;
				*plevel = 2;
				return suit_bid(n, points, bids, pCards, plevel);
			}
			else if (is_NT(points, pCards, plevel) && *plevel == 1) {
				bids[n] = 4;
				return suit_bid(n, points, bids, pCards, plevel);
			}
			else {
				bids[n] = -6;
				return suit_bid(n, points, bids, pCards, plevel);
			}
		}
	}
	else {
		bids[n] = -6;
		return suit_bid(n, points, bids, pCards, plevel);
	}
}

string Eval::diamond_r_range_2(int n, int points, int bids[4], Card* pCards, int *plevel) {	
	if (points == 11 || points == 12) {						//	Need add a possible 2 C bid for points >= 11 and 5 clubs!
		if (suitCounts[2] < 4 || suitCounts[3] < 4) {
			if (suitCounts[1] >= 5) {
				bids[n] = 1;
				*plevel= 3;
				return suit_bid(n, points, bids, pCards, plevel);
			}
			else {
				if (is_NT_r(points, plevel)) {
					bids[n] = 4;
					*plevel = 2;
					return suit_bid(n, points, bids, pCards, plevel);
				}
				else {
					if (suitCounts[0] >= 5 && *plevel == 1) {
						bids[n] = 0;
						*plevel = 2;
						return suit_bid(n, points, bids, pCards, plevel);
					}
					return diamond_r_range_1(n, points, bids, pCards, plevel);
				}
			}
		}
		else {
			return diamond_r_range_2(n, points, bids, pCards, plevel);
		}
	}
	else if (points >= 13 && points <= 16) {
		if (is_NT_r(points, plevel)) {
			bids[n] = 4;
			*plevel = 3;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else {
			return diamond_r_range_1(n, points, bids, pCards, plevel);
		}
	}
	else {
		return diamond_r_range_1(n, points, bids, pCards, plevel);
	}
}

string Eval::diamond_r_range_3(int n, int points, int bids[4], Card* pCards, int *plevel) {
	int LS;
	if (*plevel < 3) {
		if (suitCounts[2] >= 5 || suitCounts[3] >= 5) {
			if (*plevel == 1) {
				LS = longest_suit_after_NT(n, points, 1, pCards, *plevel);
				bids[n] = LS;
				*plevel = 2;
				return suit_bid(n, points, bids, pCards, plevel);
			}
			else if (suitCounts[2] >= 5 && bids[n - 1] < 2) {
				bids[n] = 2;
				suit_bid(n, points, bids, pCards, plevel);
			}
			else if (suitCounts[3] >= 5 && bids[n - 1] < 3) {
				bids[n] = 3;
				suit_bid(n, points, bids, pCards, plevel);
			}
			else {
				bids[n] = -6;
				return suit_bid(n, points, bids, pCards, plevel);
			}
		}
		else if (suitCounts[0] >= 5) {
			bids[n] = 0;
			*plevel = 3;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else {
			bids[n] = -6;
			return suit_bid(n, points, bids, pCards, plevel);
		}
	}
	else {
		bids[n] = -6;
		return suit_bid(n, points, bids, pCards, plevel);
	}
}

string Eval::heart_response(int n, int points, int bids[4], Card* pCards, int *plevel) {
	if (points < 6) {
		bids[n] = -6;
		return suit_bid(n, points, bids, pCards, plevel);
	}
	if (points >= 6 && points <= 10) {
		return heart_r_range_1(n, points, bids, pCards, plevel);
	}
	else if (points >= 11 && points <= 16) {
		return heart_r_range_2(n, points, bids, pCards, plevel);
	}
	else {
		return "A higher heart response!";
	}
}

string Eval::heart_r_range_1(int n, int points, int bids[4], Card* pCards, int *plevel) {
	if (*plevel < 3 && suitCounts[2] >= 3 && bids[n - 1] < 2) {
		bids[n] = 2;
		*plevel = 2;
		return suit_bid(n, points, bids, pCards, plevel);
	}
	else if (*plevel == 1 && suitCounts[3] >= 4 && bids[n - 1] < 3) {
		bids[n] = 3;
		return suit_bid(n, points, bids, pCards, plevel);
	}
	else if (is_NT(points, pCards, plevel) && *plevel == 1) {
		bids[n] = 4;
		return suit_bid(n, points, bids, pCards, plevel);
	}
	else {
		bids[n] = -6;
		return suit_bid(n, points, bids, pCards, plevel);
	}
}

string Eval::heart_r_range_2(int n, int points, int bids[4], Card* pCards, int *plevel) {
	if (points == 11 || points == 12) {
		if (*plevel < 3 && suitCounts[2] >= 4) {
			bids[n] = 2;
			*plevel = 3;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else if (*plevel < 3 && suitCounts[1] >= 4) {
			bids[n] = 1;
			*plevel = 2;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else if (*plevel < 3 && suitCounts[0] >= 4) {
			bids[n] = 0;
			*plevel = 2;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else {
			bids[n] = -6;
			return suit_bid(n, points, bids, pCards, plevel);
		}
	}
	else if (*plevel < 4 && suitCounts[2] == 2 && points >= 13 && points <= 16) {
		if (is_NT_r(points, plevel)) {
			bids[n] = 4;
			*plevel = 3;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else {
			bids[n] = -6;
			return suit_bid(n, points, bids, pCards, plevel);
		}
	}
	else {
		bids[n] = -6;
		return suit_bid(n, points, bids, pCards, plevel);
	}
}

string Eval::spade_response(int n, int points, int bids[4], Card* pCards, int *plevel) {
	if (points < 6) {
		bids[n] = - 6;
		return suit_bid(n, points, bids, pCards, plevel);
	}
	if (points >= 6 && points <= 10) {
		return spade_r_range_1(n, points, bids, pCards, plevel);
	}
	else if (points >= 11 && points <= 16){
		return spade_r_range_2(n, points, bids, pCards, plevel);
	}
	else {
		return "A higher spade response!";
	}
}

string Eval::spade_r_range_1(int n, int points, int bids[4], Card* pCards, int *plevel) {
	if (suitCounts[3] >= 3 && bids[n - 1] < 3) {
		bids[n] = 3;
		*plevel = 2;
		return suit_bid(n, points, bids, pCards, plevel);
	}
	else {
		bids[n] = 4;
		return suit_bid(n, points, bids, pCards, plevel);
	}
}

string Eval::spade_r_range_2(int n, int points, int bids[4], Card* pCards, int *plevel) {
	if (points == 11 || points == 12) {
		if (*plevel < 3 && suitCounts[3] >= 4 && bids[n - 1] < 3) {
			bids[n] = 3;
			*plevel = 3;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else if (*plevel < 3 && suitCounts[2] >= 5 && bids[n - 1] < 2) {
			bids[n] = 2;
			*plevel = 2;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else if (*plevel < 3 && suitCounts[1] >= 4) {
			bids[n] = 1;
			*plevel = 2;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else if (*plevel < 3 && suitCounts[0] >= 4) {
			bids[n] = 0;
			*plevel = 2;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else {
			bids[n] = -6;
			return suit_bid(n, points, bids, pCards, plevel);
		}
	}
	else if (*plevel < 4 && suitCounts[2] == 2 && points >= 13 && points <= 16) {
		if (is_NT_r(points, plevel)) {
			bids[n] = 4;
			*plevel = 3;
			return suit_bid(n, points, bids, pCards, plevel);
		}
		else {
			bids[n] = -6;
			return suit_bid(n, points, bids, pCards, plevel);
		}
	}
	else {
		bids[n] = -6;
		return suit_bid(n, points, bids, pCards, plevel);
	}
}

string Eval::NT_response(int n, int points, int bids[4], Card* pCards, int level) {	
	if (level == 1) {
		return one_NT_response(n, points, bids, pCards, level);
		}
	else {
		return "A higher level NT response!";
	}
}

string Eval::one_NT_response(int n, int points, int bids[4], Card* pCards, int level) {
	if (points <= 7) {
		return one_NT_r_range_1(n, points, bids, pCards, level);
	}
	else {
		return "A higher points response to 1 NT!";
	}
}

string Eval::one_NT_r_range_1(int n, int points, int bids[4], Card* pCards, int level) {
	int LS;	
	if ((suitCounts[1] >= 5 || suitCounts[2] >= 5 || suitCounts[3] >= 5) && level == 1) {
		LS = longest_suit_after_NT(n, points, bids[n - 1], pCards, level);
		bids[n] = LS;
		level = 2;
		return suit_bid(n, points, bids, pCards, &level);
	}
	else {
		bids[n] = -6;
		return suit_bid(n, points, bids, pCards, &level);
	}
}

int Eval::points() {
	int points_p;
	points_p = rankCounts[9] + 2 * rankCounts[10] + 3 * rankCounts[11] + 4 * rankCounts[12];
	return points_p;
}

void Eval::suit_count() {
	static const string aSuit[] = {
		"Clubs", "Diamonds", "Hearts", "Spades" };
	cout << endl << "This player has ";
	for (int i = 0; i < 3; ++i) {
		cout << suitCounts[i] << " " << aSuit[i] << ", ";
	}
	cout <<	suitCounts[3] << " " << aSuit[3] << "\n\n";
}

int Eval::longest_suit(int n, int points, int eliminate_suits, Card* pCards, int *plevel) {
	//	Need a safety net!
	if (points < 12) {
		return -6;
	}
	else {
		int e_suitCounts[4];		//	Effective suitCounts...
		for (int i = 0; i < 4; ++i) {
			e_suitCounts[i] = suitCounts[i];
		}
		switch (eliminate_suits)
		{
		case 0:	e_suitCounts[0] = 0;															break;
		case 1: e_suitCounts[0] = e_suitCounts[1] = 0;											break;
		case 2: e_suitCounts[0] = e_suitCounts[1] = e_suitCounts[2] = 0;						break;
		case 3: e_suitCounts[0] = e_suitCounts[1] = e_suitCounts[2] = e_suitCounts[3] = 0;		break;		//	Consider NT bid!
		case 4:;																				break;
		case -5:;																				break;
		}
		if (is_NT(points, pCards, plevel)) {
			return 4;
		}
		if (e_suitCounts[3] >= 5 || e_suitCounts[2] >= 5) {
			if (e_suitCounts[3] >= e_suitCounts[2]) {
				return 3;
			}
			else {
				return 2;
			}
		}
		else {
			if ((e_suitCounts[1] >= e_suitCounts[0]) & e_suitCounts[1] != 0) {
				return 1;
			}
			else {
				if (e_suitCounts[0] != 0) {
					return 0;
				}
				else {
					return -6;
				}
			}
		}
	}
}
	
int Eval::longest_suit_after_NT(int n, int points, int eliminate_suits, Card* pCards, int level) {
	int e_suitCounts[4];		//	Effective suitCounts...
	for (int i = 0; i < 4; ++i) {
		e_suitCounts[i] = suitCounts[i];
	}
	switch (eliminate_suits)
	{
	case 0:	e_suitCounts[0] = 0;															break;
	case 1: e_suitCounts[0] = e_suitCounts[1] = 0;											break;
	case 2: e_suitCounts[0] = e_suitCounts[1] = e_suitCounts[2] = 0;						break;
	case 3: e_suitCounts[0] = e_suitCounts[1] = e_suitCounts[2] = e_suitCounts[3] = 0;		break;	
	case -5:;																				break;
	case -6:;																				break;
	}
	if (e_suitCounts[3] >= 5) {
		return 3;
	}
	else if (e_suitCounts[2] >= 5) {
		return 2;
	}
	else if (e_suitCounts[1] >= 5) {
		return 1;
	}
	else {
		return 0;
	}
}

string Eval::suit_bid(int n, int points,int bids[4], Card* pCards, int *plevel) {
	int suitChose;	
	string s;	
	suitChose = bids[n];
	//	suitChose = longest_suit(n, points, eliminate_suits);
	switch (*plevel)
	{
	case 1: s = "1 ";		break;
	case 2: s = "2 ";		break;
	case 3: s = "3 ";		break;
	case 4: s = "4 ";		break;
	case 5: s = "5 ";		break;
	case 6: s = "6 ";		break;
	case 7: s = "7 ";		break;
	}

	switch (suitChose)
	{
	case 0: return s + "C";		break;
	case 1: return s + "D";		break;
	case 2: return s + "H";		break;
	case 3: return s + "S";		break;
	case 4: return s + "NT";	break;
	case -6: return p;			break;
	}	
}

string Eval::trump_or_NT(int n, int points, int bids[4], Card* pCards, int level) {
	
	if (suitCounts[0] > 5 || suitCounts[1] > 5 || suitCounts[2] > 5 || suitCounts[3] > 5) {		
		return suit_bid(n, points, bids, pCards, &level);
	}
	else if (is_NT(points, pCards, &level)) {
		bids[n] = 4;
		return suit_bid(n, points, bids, pCards, &level);
	}
	else {		
		return suit_bid(n, points, bids, pCards, &level);
	}
}

bool Eval::is_no_voids() {
	if (suitCounts[0] == 0 || suitCounts[1] == 0 || suitCounts[2] == 0 || suitCounts[3] == 0) {
		return false;
	}
	return true;
}

bool Eval::is_no_singletons() {
	if (suitCounts[0] == 1 || suitCounts[1] == 1 || suitCounts[2] == 1 || suitCounts[3] == 1) {
		return false;
	}
	return true;
}	

bool Eval::is_no_two_doubletons(){
	int doulbletonCounts = 0;
	for (int i = 0; i < 4; ++i) {
		if (suitCounts[i] == 2) {
			++doulbletonCounts;
		}
	}
	if (doulbletonCounts < 2) {
		return true;
	}
	else {
		return false;
	}
}

bool Eval::is_NT(int points, Card* pCards, int *plevel) {
	if ((is_no_voids() && is_no_singletons() && is_no_two_doubletons()) && is_three_stopper(pCards) && points >= 15) {
		if (points >= 15 && points <= 17) {
			*plevel = 1;
			return true;
		}
		else if (points >= 18 && points <= 21) {
			*plevel = 2;
			return true;
		}
		else {
			*plevel = 3;
			return true;
		}
	}
	else {
		return false;
	}
}

bool Eval::is_NT_r(int points, int *level) {
	if (is_no_voids() && is_no_singletons() && is_no_two_doubletons()) {
		return true;
	}
	else {
		return false;
	}
}


bool Eval::is_suit_stopper(Card* pCards, int a_suit) {
	bool ace, king, queen, jack;
	ace = king = queen = jack = false;
	for (int i = 0; i < 13; ++i) {
		int r = pCards[i].rank;
		int s = pCards[i].suit;
		if (s == a_suit) {
			switch (r)
			{
			case 12: ace = true;	break;
			case 11: king = true;	break;
			case 10: queen = true;	break;
			case 9:  jack = true;	break;
			default:	 			break;
			}
		}
	}
	if (ace || (king && queen) || (king && jack)) {
		return true;
	}
	else {
		return false;
	}
}

bool Eval::is_three_stopper(Card* pCards) {
	bool ics, ids, ihs, iss;
	ics = is_suit_stopper(pCards, 0);
	ids = is_suit_stopper(pCards, 1);
	ihs = is_suit_stopper(pCards, 2);
	iss = is_suit_stopper(pCards, 3);
	if ((ics && ids && ihs) || (ics && ids && iss) || (ics && ihs && iss) || (ids && ihs && iss)) {
		return true;
	} 
	else {
		return false;
	}	
}

Deck my_deck;
Card aCards[13];
Card aPlayer[4][13];

void play_game(int level);

int main()
{
	string s;
	while (true)
	{
		int level = 1;			//	level reset!
		play_game(level);
		cout << "\nPlay again? (Y or N): ";
		getline(cin, s);			
		if (s[0] == 'N' || s[0] == 'n') {
			break;
		}
	}
	return 0;
}

void play_game(int level) {
	static const string a_Player[] = { "South", "West", "North", "East" };	
	static const string aSuit[] = {
		"Clubs", "Diamonds", "Hearts", "Spades" };
	int bids[4] = { -6, -6, -6, -6 };
	int bidMax = -7;
	int points;
	
	for (int n = 0; n < 4; ++n) {
		cout << "\n\n" << a_Player[n] << ":" << endl;
		for (int i = 0; i < 13; ++i) {
			aCards[i] = aPlayer[n][i] = my_deck.deal_a_card();						
		}
		//	Seperating each's play's hand into suits.
		for (int a_suit = 0; a_suit < 4; ++a_suit) {
			cout << aSuit[a_suit] << ": ";
			Display my_disply;
			my_disply.sort_n_display(aCards, a_suit);			
			cout << endl;
		}		
		Eval my_eval(aCards);
		points = my_eval.points();
		cout << endl << "points = " << points;	
		my_eval.suit_count();		
		bids[n] = my_eval.bid_tracker(n, points, bids, aCards, &bidMax);		
		cout << a_Player[n] << " Bids " << my_eval.bidding(n, points, bids, aCards, level, &bidMax); 			
	}		
}

	


