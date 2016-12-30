#include <iostream>
#include <string>
using namespace std;

const int TWENTY_ONE = 21;
const string FILE_NAME("BLACK_JACK.txt");

void printCardName(int card){
	switch (card){
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		cout << " " << card << " ";
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

int getMinSum(int length, int v[]){
	//compute the minimum sum possible with the cards in v
	int sum = 0;
	for (int i = 1; i <= length; i++)
	{
		if (v[i] < 11) {
			sum += v[i];
		}
		else {
			if (v[i] == 14) { sum++; }
			else sum += 10;
		}
	}
	return sum;
}

bool checkSum(int length, int v[]){
	//check if 21 can be achieved
	int sum = 0, ases = 0;
	for (int i = 1; i <= length; i++){
		if (v[i] == 14){ ases++; continue; }
		if (v[i] <= 10){ sum += v[i]; }
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

int main(){
	int x;
	cout << FILE_NAME;
	cin >> x;
}