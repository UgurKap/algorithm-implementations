#include <iostream>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <random>

using namespace std;

void find_duplications(unsigned int *arr, unsigned int arr_length, vector<unsigned int> &dup_vec) {
    for (unsigned int i = 0; i < arr_length; i++) {
        for (unsigned int j = i + 1; j < arr_length; j++) {
            if (arr[i] == arr[j]) {
                dup_vec.push_back(i);
                dup_vec.push_back(j);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // (a)
    unsigned int arr_length; // User will provide the array length and maximum number we can generate
    arr_length = stoul(argv[1]);
    unsigned int *random_arr = new unsigned int[arr_length];

    // (b)
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(1, arr_length);

    for (unsigned int i = 0; i < arr_length; i++) {
        random_arr[i] = distribution(generator); // Element i is between 1 and arr_length(included)
    }

    // (c)

    cout << "ARRAY:";
    for (unsigned int i = 0; i < arr_length; i++) {
        if (i % 10 == 0) cout << "\n"; // 10 elements per line
        cout << random_arr[i] << "\t";
    }
    cout << endl;

    // (d)
    cout << "\nEXPECTED NUMBER OF DUPLICATIONS = " << (arr_length - 1) / 2.0 << endl;

    // (e)
    vector<unsigned int> dup_vec;
    find_duplications(random_arr, arr_length, dup_vec);

    int c = 0;
    for (auto i = dup_vec.begin(); i != dup_vec.end(); advance(i, 2)) {
        if (c % 10 == 0) cout << "\n"; // 10 elements per line
        cout << "(" << *i + 1 << ", " << *(i + 1) + 1 << ") ";
        c++;
    }

    // (f)
    cout << endl << "\nENCOUNTERED NUMBER OF DUPLICATIONS = " << dup_vec.size() / 2 << endl;

    delete[] random_arr;

    return 0;
}
