#include <random>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 3) {
		cerr << "Usage: " << argv[0] << " <random_seed> <num_iterations>\n";
		return 1;
	}

	int seed = std::atoi(argv[1]);
	int num_iterations = std::atoi(argv[2]);

	std::mt19937 gen(seed);
	std::uniform_int_distribution<int> op_dist(1, 5);
	std::uniform_int_distribution<int> num_dist(-10000000, 10000000);

	// Create an instance of your order statistic tree

	for (int i = 0; i < num_iterations; ++i) {
		int operation = op_dist(gen);
		cout << "OP:" << operation << endl;

		switch (operation) {
			case 1: {
					// Insert a random number into the order statistic tree
					int num = num_dist(gen);
					cout << num << endl;
					bst.insert(num);
					break;
				} case 2: {
					// Delete a random number from the order statistic tree
					int num = num_dist(gen);
					cout << num << endl;
					auto it = bst.find(num);
					if(it != bst.end() &&  bst.size()>0) bst.erase(it);
					break;
				} case 3: {
					// Print size of the tree
					cout << bst.size() << endl;
					break;
				} case 4: {
					// Find the kth smallest element in the order statistic tree
					if(bst.size()==0) {
						cout << "None" << endl;
						break;
					}
					uniform_int_distribution<int> k_dist(0, bst.size()-1);
					int k = k_dist(gen);
					cout << *bst.find_by_order(k) << endl;
					break;
				} case 5: {
					// Find the number of elements smaller than value k
					int k = num_dist(gen);
					cout << bst.order_of_key(k) << endl;
					break;
				}
		}
	}

	return 0;
}
