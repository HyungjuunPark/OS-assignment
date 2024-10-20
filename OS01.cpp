#include <iostream>
using namespace std;

int main() {
	int n;
	cin >> n;
	for (int start = 1; start <= n; start += 5) {
		for (int i = 1; i <= n; i++) {
			for (int j = start; j < start + 5 && j <= n; j++) {
				if (j < i)
					cout << '\t' << '\t';
				else
					cout << j << " x " << i << " = " << i * j << '\t';
			}
			cout << '\n';
		}
		cout << '\n';
	}
	return 0;
}
