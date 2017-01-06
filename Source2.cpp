#include <iostream>
#include <string>
#include <fstream>
#include<ctime>
using namespace std;

const int TWENTY_ONE = 21;
const int SINGLE_PLAYER = 1;
const int MULTI_PLAYER = 2;
const string FILE_NAME("BLACK_JACK.txt");
int gasit[15], nrcarti0, nrcarti1, nrcarti2;
fstream database(FILE_NAME);

struct player {
	char name[100];
	int money;
};

struct cardSet{
	int cards[20];
	int nr;
};

void printCards(cardSet cards){
	for (int i = 1; i < cards.nr; i++){
		switch (cards.cards[i]){
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			cout << " " << cards.cards[i] << " ";
			break;
		case 11:
			cout << " Valet ";
			break;
		case 12:
			cout << " Dama ";
			break;
		case 13:
			cout << " Popa ";
			break;
		case 14:
			cout << " As ";
			break;
		default:
			cout << " ERROR! ";
			break;
		}
	}
	cout << endl;
}

void twoPlayersSetup(char player1Name[], int player1Money, char player2Name[], int player2Money){
	// ** setup the dabase for 2 player mode
	database.close();
	database.open(FILE_NAME, fstream::out | fstream::trunc);
	database << "2 " << endl;
	database << player1Name << endl << player1Money << endl;
	database << player2Name << endl << player2Money;
	database.close();
}

int getMinSum(cardSet cards){
	//compute the minimum sum possible with the cards in v
	int sum = 0;
	for (int i = 1; i <= cards.nr; i++)
	{
		if (cards.cards[i] < 11) {
			sum += cards.cards[i];
		}
		else {
			if (cards.cards[i] == 14) { sum++; }
			else sum += 10;
		}
	}
	return sum;
}

bool checkSum(cardSet cards){
	//check if 21 can be achieved
	int sum = 0, ases = 0;
	for (int i = 1; i <= cards.nr; i++){
		if (cards.cards[i] == 14){ ases++; continue; }
		if (cards.cards[i] <= 10){ sum += cards.cards[i]; }
		else { sum += 10; }
	}
	if (sum == TWENTY_ONE) { return true; }
	if (1 <= ases && ases <= 4){
		if (sum + ases == TWENTY_ONE || sum + 10 + ases == TWENTY_ONE){
			return true;
		}
	}
	return false;
}

void surrender(bool& handFinale, player surrendedPlayer, int currentBet)
{
	//needs CHECKING
	if (!handFinale)
	{
		handFinale = true; //asta il apelam doar pt joc1
		surrendedPlayer.money -= (currentBet / 2);
	}
}

cardSet split(cardSet first){
	cardSet newCardSet;
	int decision;
	cout << "SPLIT ? Apasati 1 sau 0" << endl;
	cin >> decision; cout << endl;
	if (decision)
	{
		first.nr--;
		newCardSet.nr = 1;
		newCardSet.cards[1] = first.cards[2];
		first.cards[2] = 0;
	}
	return newCardSet;
}

void hit(cardSet cards){
	int a;
	a = 2 + rand() % 13;
	while (gasit[a] == 4) {
		a = 2 + rand() % 13;
	}
	cards.cards[++cards.nr] = a;
	if (checkSum(cards) == 1) { cards.nr = 0; }
}

void stand();

bool insurance(cardSet cards, int& money, int bet){
	int k, valoare;
	cout << "INSURANCE ? Apasati 0 sau 1.";
	cin >> k; while (k != 1 && k != 0) { cout << "AGAIN" << endl; cin >> k; cout << endl; }
	if (k == 1)
	{
		cout << "Valoare insurance: " << endl;
		cin >> valoare; cout << endl; while (valoare + bet > money) { cout << "Nu aveti destui bani ! Introduceti o suma corecta" << endl; cin >> valoare; cout << endl; }
		if (checkSum(cards) == 1) {
			money = money + valoare; 
			cout << "YEAYY" << endl; 
			nrcarti1 = 0; nrcarti2 = 0; 
			return true;
		}
		else { money = money - valoare; cout << "NUUU" << endl; }
	}
	return false;
}

void checkSinglePlayerResult(){

}

void singlePlayer(player firstPlayer){

	int  facut2, nrcarti1, nrcarti2;
	char s[255];
	bool playing = true;

	cardSet dealer, player, playerSplit;

	facut2 = 0;


	cout << "You've got " << firstPlayer.money << " dollars." << endl;
	if (firstPlayer.money > 0)
	while (playing && firstPlayer.money > 0) {
		bool handFinale = false;
		int found[20], bet;
		
		int a;
		srand(time(NULL));
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13; 	dealer.cards[1] = a; found[a]++;
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13; 	dealer.cards[2] = a; found[a]++;
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13;		player.cards[1] = a; found[a]++;
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13;		player.cards[2] = a; found[a]++;
		dealer.nr = 2; player.nr = 2;

		cout << "How much you want to bet ?" << endl;
		cin >> bet; cout << endl;

		while (bet > firstPlayer.money || bet <= 0) {
			cout << "You don't have enough money to bet " << bet << " dollars. Choose another amount. " << endl;
			cin >> bet; cout << endl;
		}

		printCards(dealer);  //ar trebui sa afisam doar o carte a dealerului dar in scop de verificare le afisam pe ambele
		printCards(player);

		cout << endl << "SURRENDER ? Press 0 sau 1" << endl;
		cin >> handFinale; cout << endl;
		surrender(handFinale, firstPlayer, bet);
		if (!handFinale && player.cards[1] == player.cards[2]) { 
			playerSplit = split(player); 
			facut2 = 1; 
		}

		while (!handFinale)
		{
			if (dealer.cards[1] == 14) { handFinale =  insurance(dealer, firstPlayer.money, bet); }
			if (nrcarti1 != 0)
			{
				cout << " Your move!" << '\n';
				cin.get();
				cin.get(s, 255);
				_strupr_s(s);
				if (strstr(s, "HIT") != NULL && strlen(s) == 3) { hit(player); if (player.nr == 0) { handFinale = true;  cout << "BLACKJACK ! " << endl; firstPlayer.money += bet * 3 / 2; } }
				else if (strstr(s, "STAND") != NULL && strlen(s) == 5) stand();
				else if (strstr(s, "DOUBLE DOWN") != NULL && strlen(s) == 11) { hit(player); if (nrcarti1 == 0) { handFinale = true; cout << "BLACKJACK !" << endl; firstPlayer.money += bet * 3; } else  bet = bet * 2; }
				else cout << "Ai introdus o comanda necunoscuta care nu ti se va lua in considerare." << endl;
			}
			//else cout << "Nu mai aveti ce sa faceti cu primul set de carti" << endl;

			if (nrcarti2 != 0)
			{
				cout << " DECIZIE 2 ?" << endl;
				cin.get();
				cin.get(s, 255);
				_strupr_s(s);
				if (strstr(s, "HIT") != NULL && strlen(s) == 3) { hit(playerSplit); if (nrcarti2 == 0){ handFinale = true;  cout << "BLACKJACK !!" << endl; firstPlayer.money += bet * 3 / 2; } }
				else if (strstr(s, "STAND") != NULL && strlen(s) == 5) stand();
				else if (strstr(s, "DOUBLE DOWN") != NULL && strlen(s) == 11) { hit(playerSplit); if (nrcarti2 == 0){ handFinale = true; cout << "BLACKJACK !" << endl; firstPlayer.money += bet * 3; } else { bet = bet * 2; } }
				else cout << "Ai introdus o comanda necunoscuta care nu ti se va lua in considerare." << endl;
			}
			//else cout << "Nu mai ai ce face cu al doilea set de cati" << endl;

			//decizia casei
			//D[1] = 9; D[2] = 5; D[3] = 7; nrcarti0 = 3;
			if (getMinSum(dealer) < 17) { hit(dealer); } //tre sa vad ca daca si jucatorii si dealerul are BLACKJACK atunci cine castiga ??
			//	if (check(D,nrcarti0)== 1) nrcarti0 = 0;
			if (nrcarti0 == 0) { if (!handFinale){ firstPlayer.money -= bet; cout << "Ai pierdut. Dealerul a facut BLAKCJACK" << endl; handFinale = true; } else { cout << "amandoi ati facut BLACKJACK dar tu primul deci castigi." << endl; } }
			if (!handFinale) {
				int d, v, m;
				d = getMinSum(dealer); v = getMinSum(player); m = getMinSum(playerSplit);
				//checkSinglePlayerResult(d, v, m, facut2);
			}
			if (handFinale == 0){
				printCards(dealer);
				printCards(player);
				printCards(playerSplit);
			}
		}
		printCards(dealer);
		printCards(player);
		printCards(playerSplit);

		cout << "Mai ai  " << firstPlayer.money << " bani." << endl;
		cout << endl << "Mai jucati ? Apasati 1 sau 0" << endl;
		cin >> playing; cout << endl; while (playing != 1 && playing != 0){ cout << "AGAIN !" << endl; cin >> playing; cout << endl; }
	}
	else {
		cout << "You are broke! Game over!" << endl;
	}
	database.close();
	database.open(FILE_NAME, fstream::in | fstream::trunc);
	database.close();
	database.open(FILE_NAME, fstream::out | fstream::trunc);
	database << SINGLE_PLAYER << endl << firstPlayer.name << endl << firstPlayer.money;
	database.close();

}


void multiPlayer(player firstPlayer, player secondPlayer){

}

int main(){
	if (database){
		database.close();
		database.open(FILE_NAME, fstream::in);

	}
	else {
		int game;
		//for (a = 1; a <= 15; a++) gasit[a] = 0;
		database.open(FILE_NAME, ios::out); //creez fisierul;

		cout << "Welcome to the BlackJack table!" << endl << endl << "Press '1' for single player or '2' for multiplayer!" << endl << endl;

		cin >> game; cout << endl;
		database << game; cout << endl;

		if (game == SINGLE_PLAYER)
		{
			player firstPlayer;

			cout << "Please write your name and cash amount!" << endl << endl;
			cin.get(); cin.getline(firstPlayer.name, 255);
			cin >> firstPlayer.money;
			cout << endl;
			database << endl << firstPlayer.name << endl << firstPlayer.money;
			database.close();
			singlePlayer(firstPlayer);
		}
		else {
			player firstPlayer, secondPlayer;
			cout << "Please write the name of the first player and it's cash amount!" << endl << endl;
			cin.get(); cin.getline(firstPlayer.name, 255);
			cin >> firstPlayer.money;
			cout << endl << "Please write the name of the second player and it's cash amount!" << endl << endl;
			cin.get();
			cin.getline(secondPlayer.name, 255);
			cin >> secondPlayer.money;
			cout << endl;
			database << endl << firstPlayer.name << endl << firstPlayer.money << endl << secondPlayer.name << endl << secondPlayer.money;
			multiPlayer(firstPlayer, secondPlayer);
			database.close();
		}
	}
}