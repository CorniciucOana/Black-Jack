#include <iostream>
#include <string>
#include <fstream>
#include<ctime>
using namespace std;

const int TWENTY_ONE = 21;
const int SINGLE_PLAYER = 1;
const int MULTI_PLAYER = 2;
const string FILE_NAME("BLACK_JACK.txt");
int found[15];
fstream database(FILE_NAME);

struct player {
	char name[100];
	int money;
	int bet;
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
		surrendedPlayer.money -= (surrendedPlayer.bet / 2);
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
	while (found[a] == 4) {
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
			playerInfo.money -= playerInfo.bet / 2;

			if (!playerSplit.nr) { playerInfo.money -= playerInfo.bet / 2; cout << "You lost!" << endl; }
			else cout << "You lost with the first hand!" << endl;
		}
		else  { cout << "You got your money back!" << endl; }

		if (getMinSum(playerSplit) > getMinSum(dealer)){
			playerInfo.money -= playerInfo.bet / 2;  cout << "You lost with the second hand!" << endl;
		}
		else if (playerSplit.nr) { cout << "You got the money back from the second hand!"; }

		ab1 = 1; ab2 = 1;
	}
	else {
		if (getMinSum(player) > 21){
			playerInfo.money -= playerInfo.bet / 2; ab1 = 1; cout << "You lost with the first hand!" << endl;
			if (!playerSplit.nr) { ab2 = 1; playerInfo.money -= playerInfo.bet / 2; } //daca nu am facut split atunci pierd tot pariul
		}
		if (getMinSum(playerSplit) > 21) { playerInfo.money -= playerInfo.bet / 2; ab2 = 1; cout << "You lost with the second hand " << endl; }
	}
	clearHand(player);
	clearHand(playerSplit);
	if (ab1 == 1 && ab2 == 1) { return true; }
	return false;
}
bool checkMultiPlayerDecision(player& firstPlayer, player& secondPlayer, cardSet& dealer, cardSet& player1, cardSet& player1Split, cardSet& player2, cardSet& player2Split){
	
	if (!player1.nr && !player1Split.nr && !player2.nr && !player2Split.nr) {
		return true;
	}
	if (getMinSum(dealer) > 21 || getMinSum(player1) > 21 || getMinSum(player1Split) > 21 || getMinSum(player2) > 21 || getMinSum(player2Split) > 21)
	{
		if (getMinSum(dealer) > 21)
		{
			if (getMinSum(player1) > getMinSum(dealer) || getMinSum(player1Split) > getMinSum(dealer))
			{
				if (getMinSum(player1) > getMinSum(dealer)) {
					if (player1Split.nr && getMinSum(player1Split) < getMinSum(dealer)){ 
						firstPlayer.money -= firstPlayer.bet / 2;
						cout << firstPlayer.name << " you lost with your first hand." << endl; 
					}
					else  {
						firstPlayer.money -= firstPlayer.bet;
						player1.nr = 0; player1Split.nr = 0;
						cout << firstPlayer.name << ": you lost!" << endl;
					}
				}
				else { 
					firstPlayer.money -= firstPlayer.bet / 2; 
					player1Split.nr = 0; 
					cout << firstPlayer.name << ": you lost with the second hand!" << endl; 
				}
			}
			else if (getMinSum(player1) != 0 || getMinSum(player1Split) != 0) {
				cout << endl << firstPlayer.name << ": you got your money back!" << endl;
			}

			if (getMinSum(player2) > getMinSum(dealer) || getMinSum(player2Split) > getMinSum(dealer)){
				if (getMinSum(player2) > getMinSum(dealer)) {
					if (player2Split.nr  && getMinSum(player2Split) < getMinSum(dealer)) { 
						secondPlayer.money -= secondPlayer.bet / 2; player2.nr = 0; 
						cout << secondPlayer.name << ": you lost with the first hand!" << endl; 
					}
					else {
						secondPlayer.money -= secondPlayer.bet;
						player2.nr = 0; player2Split.nr = 0;
						cout << secondPlayer.name << ": you lost!" << endl;
					}
				}
				else { 
					secondPlayer.money -= secondPlayer.bet / 2; player2Split.nr = 0; 
					cout << secondPlayer.name << ": you lost with the second hand!" << endl; 
				}
			}
			else if (getMinSum(player2) != 0 || getMinSum(player2Split) != 0) {
				cout << endl << secondPlayer.name << ": you got your money back!" << endl;
			}
			return true;

		}
		else {
			if (getMinSum(player1) > 21 || getMinSum(player1Split) > 21)
			{
				if (getMinSum(player1) > 21)
				{
					if (player1Split.nr && getMinSum(player1Split) <21) {
						firstPlayer.money -= firstPlayer.bet / 2; 
						player1.nr = 0; 
						cout << firstPlayer.name << ": you lost with your first hand." << endl;
					}
					else { 
						firstPlayer.money -= firstPlayer.bet; 
						player1.nr = 0; player1Split.nr = 0; 
						
						cout << firstPlayer.name << ": you lost!" << endl; }
				}
				else { 
					firstPlayer.money -= firstPlayer.bet / 2; player1Split.nr = 0;
					cout << firstPlayer.name << ": you lost with your second hand!" << endl; 
				}
			}
			if (getMinSum(player2) > 21 || getMinSum(player2Split) > 21)
			{
				if (getMinSum(player2) > 21)
				{
					if (player2Split.nr && getMinSum(player2Split) < 21) {
						secondPlayer.money -= secondPlayer.bet / 2; 
						player2.nr = 0; cout << secondPlayer.name << ": you lost with your first hand." << endl;
					}
					else { 
						secondPlayer.money -= secondPlayer.bet; 
						player2.nr = 0; player2Split.nr = 0; 
						cout << secondPlayer.name << " you lost!" << endl; }
				}
				else { 
					secondPlayer.money -= secondPlayer.bet / 2; player2Split.nr = 0; 
					cout << secondPlayer.name << ": you lost with your second hand!" << endl; }
			}
		}
	}
	if (!player1.nr && !player1Split.nr && !player2.nr && !player2Split.nr) {
		return true;
	}
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
			playerInfo.money += playerInfo.bet * 3 / 2;
			return true;
		}
	}
	else if (strstr(s, "STAND") != NULL && strlen(s) == 5) stand();
	else if (strstr(s, "DOUBLE DOWN") != NULL && strlen(s) == 11) {
		hit(cards);
		if (!cards.nr) {
			cout << "BLACKJACK !" << endl;
			playerInfo.money += playerInfo.bet * 3;
			return true;
		}
		else  playerInfo.bet = playerInfo.bet * 2;
	}
	else cout << "Wrong command!" << endl;
	return false;
}


void singlePlayer(player firstPlayer){

	char s[255];
	bool playing = true;

	cardSet dealer, player, playerSplit;



	cout << "You've got " << firstPlayer.money << " dollars." << endl;
	if (firstPlayer.money > 0)
	while (playing && firstPlayer.money > 0) {
		bool handFinale = false;

		int a;
		srand(time(NULL));
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13; 	dealer.cards[1] = a; found[a]++;
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13; 	dealer.cards[2] = a; found[a]++;
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13;		player.cards[1] = a; found[a]++;
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13;		player.cards[2] = a; found[a]++;
		dealer.nr = 2; player.nr = 2;

		cout << "How much you want to bet ?" << endl;
		cin >> firstPlayer.bet; cout << endl;

		while (firstPlayer.bet > firstPlayer.money || firstPlayer.bet <= 0) {
			cout << "You don't have enough money to bet " << firstPlayer.bet << " dollars. Choose another amount. " << endl;
			cin >> firstPlayer.bet; cout << endl;
		}

		printCards(dealer);  //ar trebui sa afisam doar o carte a dealerului dar in scop de verificare le afisam pe ambele
		printCards(player);

		cout << endl << "SURRENDER ? Press 0 sau 1" << endl;
		cin >> handFinale; cout << endl;
		surrender(handFinale, firstPlayer);
		if (!handFinale && player.cards[1] == player.cards[2]) {
			playerSplit = split(player);
		}

		while (!handFinale)
		{
			if (dealer.cards[1] == 14) { handFinale = insurance(dealer, firstPlayer.money, firstPlayer.bet); }
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
					firstPlayer.money -= firstPlayer.bet;
					cout << "You lost! The dealer has BlackJack!" << endl;
					handFinale = true;
				}
				else {
					cout << "You've got BlackJack first so you win!" << endl;
				}
			}
			if (!handFinale) {
				handFinale = checkSinglePlayerResult(firstPlayer, dealer, player, playerSplit);
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
	bool playing = true;
	bool handFinale = false;
	cardSet dealer, player1, player1Split, player2, player2Split;
	cout << firstPlayer.name << " has " << firstPlayer.money << " money." << endl;
	cout << firstPlayer.name << " has " << firstPlayer.money << " money." << endl;

	if (firstPlayer.money > 0 || secondPlayer.money > 0)
	while (playing && (firstPlayer.money > 0 || secondPlayer.money > 0))
	{
		//facut1 = 0; facut2 = 0; facut3 = 0;

		if (firstPlayer.money == 0) {
			cout << "Game over for " << firstPlayer.name << endl;
		}
		if (secondPlayer.money == 0) {
			cout << "Game over for " << secondPlayer.name << endl;
		}

		//shuffle the deck
		int a;
		srand(time(NULL));
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13; 	dealer.cards[1] = a; found[a]++;
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13; 	dealer.cards[2] = a; found[a]++;
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13;		player1.cards[1] = a; found[a]++;
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13;		player1.cards[2] = a; found[a]++;
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13;		player2.cards[1] = a; found[a]++;
		a = 2 + rand() % 13; while (found[a] == 4) a = 2 + rand() % 13;		player2.cards[2] = a; found[a]++;
		dealer.nr = 2; player1.nr = 2; player2.nr = 2;

		//daca unu nu mai are bani nu mai pariaza
		if (firstPlayer.money > 0) {
			cout << firstPlayer.name << ": how much you want to bet?" << endl;
			cin >> firstPlayer.bet;
		}
		if (secondPlayer.money > 0){
			cout << secondPlayer.name << ": how much you want to bet?" << endl;
		}

		printCards(dealer);
		if (firstPlayer.money > 0) { printCards(player1); }
		if (secondPlayer.money > 0) { printCards(player2); }

		bool surrender1, surrender2;
		//k1, k2 -> surrender1,surrender2
		if (firstPlayer.money > 0) {
			cout << firstPlayer.name << " SURRENDER ? Press 0 or 1" << endl;
			cin >> surrender1; surrender(surrender1, firstPlayer);
		}
		if (secondPlayer.money > 0) {
			cout << secondPlayer.name << " SURRENDER ? Press 0 or 1" << endl;
			cin >> surrender2; surrender(surrender2, secondPlayer);
		}
		if (surrender1 && surrender2) { handFinale = true; }

		if (!handFinale)
		{
			//Split?
			if (firstPlayer.money > 0 && player1.cards[1] == player1.cards[2]) {
				player1Split = split(player1);
			}
			if (secondPlayer.money > 0 && player2.cards[1] == player2.cards[2]) {
				player2Split = split(player2);
			}
		}

		while (!handFinale)
		{
			if (dealer.cards[1] == 14) {
				if (player1.nr || player1Split.nr){
					player1.nr = (insurance(dealer, firstPlayer.money, firstPlayer.bet)) ? 0 : player1.nr;
					player1Split.nr = player1.nr;
				}
				if (player2.nr || player2Split.nr){
					player2.nr = (insurance(dealer, secondPlayer.money, secondPlayer.bet)) ? 0 : player2.nr;
					player2Split.nr = player2.nr;
				}
				if (!player1.nr && !player1Split.nr && !player2.nr && !player2Split.nr) { handFinale = true; }
			}

			if (player1.nr != 0){
				handFinale = handFinale || playerDecision(firstPlayer, player1);
			}
			if (player1Split.nr != 0){
				handFinale = handFinale || playerDecision(firstPlayer, player1Split);
			}
			//second player
			if (player2.nr != 0){
				handFinale = handFinale || playerDecision(secondPlayer, player2);
			}
			if (player2Split.nr != 0){
				handFinale = handFinale || playerDecision(secondPlayer, player2Split);
			}


			if (getMinSum(dealer) < 17) hit(dealer);
			
			if (!dealer.nr) { 
				if (!handFinale){ 
					cout << "You both lost! The dealer has BLACKJACK!" << endl; 
					firstPlayer.money -= firstPlayer.bet; 
					secondPlayer.money -= secondPlayer.bet; 
					handFinale = true; 
				} else { 
					cout << "One of you, or both beat the PC! HAHA!"; 
				} 
			}

			if (!handFinale) {
				handFinale = checkMultiPlayerDecision(firstPlayer, secondPlayer, dealer, player1, player1Split, player2, player2Split);
			}

			if (!handFinale){
				printCards(dealer);
				printCards(player1);
				printCards(player1Split);
				printCards(player2);
				printCards(player2Split);
			}

		}
		printCards(dealer);
		printCards(player1);
		printCards(player1Split);
		printCards(player2);
		printCards(player2Split);

		cout << endl << "Play again?" << endl;
		cin >> playing;

	}
	else {
		cout << "You are both broke! Game over!" << endl;
	}

	database.close();
	database.open("FILE.txt", fstream::in | fstream::trunc);
	database.close();
	database.open("FILE.txt", fstream::out | fstream::trunc);
	database << MULTI_PLAYER << endl << firstPlayer.name << endl << firstPlayer.money << endl << secondPlayer.name << endl << secondPlayer.money;
	database.close();
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