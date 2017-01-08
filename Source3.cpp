#include <iostream>
#include <string>
#include <fstream>
#include<ctime>
using namespace std;

const int TWENTY_ONE = 21;
const int SINGLE_PLAYER = 1;
const int MULTI_PLAYER = 2;
const string FILE_NAME("BLACK_JACK.txt");
int gasit[15], bet;
fstream database(FILE_NAME);

struct player {
	char name[100];
	int money;
};

struct cardSet{
	int cards[20];
	int nr = 0;
};

void printCards(cardSet cards){
	for (int i = 1; i <= cards.nr; i++){
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
			cout << " JACK ";
			break;
		case 12:
			cout << " QUEEN ";
			break;
		case 13:
			cout << " KING ";
			break;
		case 14:
			cout << " ACE ";
			break;
		default:
			cout << " ERROR! ";
			break;
		}
	}
	cout << endl;
}

void resetGame(){

}

void twoPlayersSetup(player firstPlayer, player secondPlayer){
	// ** setup the database for 2 player mode
	database.close();
	database.open(FILE_NAME, fstream::out | fstream::trunc);
	database << "2 " << endl;
	database << firstPlayer.name << endl << firstPlayer.money << endl;
	database << secondPlayer.name << endl << secondPlayer.money;
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

void surrender(bool& handFinale, player& surrendedPlayer)
{
	//needs CHECKING
	if (handFinale)
	{
		surrendedPlayer.money -= (bet / 2);
	}
}

cardSet split(cardSet& first){
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

void hit(cardSet& cards){
	int a;
	a = 2 + rand() % 13;
	while (gasit[a] == 4) {
		a = 2 + rand() % 13;
	}
	cards.cards[++cards.nr] = a;
	if (checkSum(cards)) { 
		cards.nr = 0; 
	}
}

void stand(){

}

void clearHand(cardSet& hand){
	hand.nr = 0;
	for (int i = 0; i < 20; i++){
		hand.cards[i] = 0;
	}
}

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
			clearHand(cards);
			return true;
		}
		else { money = money - valoare; cout << "NUUU" << endl; }
	}
	return false;
}

bool checkSinglePlayerResult(player& playerInfo, cardSet& dealer, cardSet& player, cardSet& playerSplit){
	//d = getMinSum(dealer); v = getMinSum(player); m = getMinSum(playerSplit);
	int ab1 = 0, ab2 = 0;
	if (getMinSum(dealer) > 21)
	{
		if (getMinSum(player) > getMinSum(dealer)) {
			playerInfo.money -= bet / 2;

			if (!playerSplit.nr) { playerInfo.money -= bet / 2; cout << "You lost!" << endl; }
			else cout << "You lost with the first hand!" << endl;
		}
		else  { cout << "You got your money back!" << endl; }

		if (getMinSum(playerSplit) > getMinSum(dealer)){
			 playerInfo.money -= bet / 2;  cout << "You lost with the second hand!" << endl;
		}
		else if (playerSplit.nr) { cout << "You got the money back from the second hand!"; }

		ab1 = 1; ab2 = 1;
	} else {
		if (getMinSum(player) > 21){
			 playerInfo.money -= bet / 2; ab1 = 1; cout << "You lost with the first hand!" << endl;
			if (!playerSplit.nr) { ab2 = 1; playerInfo.money -= bet / 2; } //daca nu am facut split atunci pierd tot pariul
		}
		if (getMinSum(playerSplit) > 21) { playerInfo.money -= bet / 2; ab2 = 1; cout << "Cu a doua mana ai pierdut " << endl; }
	}
	clearHand(player);
	clearHand(playerSplit);
	if (ab1 == 1 && ab2 == 1) { return true; }
	return false;
}

bool playerDecision(player& playerInfo, cardSet& cards){
	char s[255];
	cout << " Your move!" << '\n';
	cin.get();
	cin.get(s, 255);
	_strupr_s(s);
	if (strstr(s, "HIT") != NULL && strlen(s) == 3) {
		hit(cards);
		if (cards.nr == 0) {
			cout << "BLACKJACK ! " << endl;
			playerInfo.money += bet * 3 / 2;
			return true;
		}
	}
	else if (strstr(s, "STAND") != NULL && strlen(s) == 5) stand();
	else if (strstr(s, "DOUBLE DOWN") != NULL && strlen(s) == 11) {
		hit(cards);
		if (!cards.nr) {
			cout << "BLACKJACK !" << endl;
			playerInfo.money += bet * 3;
			return true;
		}
		else  bet = bet * 2;
	}
	else cout << "Wrong command!" << endl;
	return false;
}

void singlePlayer(player firstPlayer){

	int  facut2;
	char s[255];
	bool playing = true;

	cardSet dealer, player, playerSplit;

	facut2 = 0;


	cout << "You've got " << firstPlayer.money << " dollars." << endl;
	if (firstPlayer.money > 0)
	while (playing && firstPlayer.money > 0) {
		bool handFinale = false;
		int found[20];
		
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
		surrender(handFinale, firstPlayer);
		if (!handFinale && player.cards[1] == player.cards[2]) { 
			playerSplit = split(player); 
			facut2 = 1; 
		}

		while (!handFinale)
		{
			if (dealer.cards[1] == 14) { handFinale =  insurance(dealer, firstPlayer.money, bet); }
			if (player.nr){
				handFinale = playerDecision(firstPlayer, player);
			}

			if (playerSplit.nr){
				handFinale = playerDecision(firstPlayer, playerSplit);				
			}

			//decizia casei
			if (getMinSum(dealer) < 17) { hit(dealer); } //tre sa vad ca daca si jucatorii si dealerul are BLACKJACK atunci cine castiga ??
			if (!player.nr) { 
				if (!handFinale){ 
					firstPlayer.money -= bet; 
					cout << "You lost! The dealer has BlackJack!" << endl; 
					handFinale = true; 
				} 
				else { 
					cout << "You've got BlackJack first so you win!" << endl;
				} 
			}
			if (!handFinale) {
				handFinale =  checkSinglePlayerResult(firstPlayer, dealer, player, playerSplit);
			}
			if (!handFinale){
				printCards(dealer);
				printCards(player);
				printCards(playerSplit);
			}
		}
		printCards(dealer);
		printCards(player);
		printCards(playerSplit);

		cout << "You've got  " << firstPlayer.money << " dollars." << endl;
		cout << endl << "Play again? Press 0 or 1." << endl;
		cin >> playing; cout << endl;
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
	int game;

	if (database){
		player firstPlayer, secondPlayer;
		database.close();
		database.open(FILE_NAME, fstream::in);

		database >> game;
		if (game == 1)
		{
			database.getline(firstPlayer.name, 255);
			database.getline(firstPlayer.name, 255);
			database >> firstPlayer.money;
			cout << "Are you " << firstPlayer.name << " ? Press 0 or 1" << endl;
			cin >> game;
			if (game)
			{
				cout << "Continue alone or invite someone?" << endl;
				cin >> game; 
				if (game) singlePlayer(firstPlayer);
				else {
					cout << "Please write your friend's name and cash amount!" << endl << endl;
					
					cin.get(); cin.getline(secondPlayer.name, 255);
					cin >> secondPlayer.money;
					cout << endl;
					
					twoPlayersSetup(firstPlayer, secondPlayer);
				}
			}
			else {
				//resetGame();
			}
		}
		else
		{
			database.getline(firstPlayer.name, 255);  database.getline(firstPlayer.name, 255);
			database >> firstPlayer.money;
			database.getline(secondPlayer.name, 255);  database.getline(secondPlayer.name, 255);
			database >> secondPlayer.money;
			cout << "Are you " << firstPlayer.name << " and " << secondPlayer.name << " ?" << endl;
			cin >> game; 
			if (game == 1) multiPlayer(firstPlayer, secondPlayer);
			else {
				//resetGame();
			}
		}
		database.close();

	}
	else {
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
	cout << "Type anything to exit the console!";
	cin >> game;
	return 0;
	
}