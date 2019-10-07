#include <iostream>
#include "sorts.h"

using namespace std;

int main() {
    int arr[11] = {1, 5, 2, 6, 0, -5, 3, 93, 1, 10, 27};
    string names[] = {"Uğur", "Ali", "Kaplan", "Hazım", "Kemal", "Ekenel"};
    int arr2[] = {1, 5, 2, 6, 0, -5, 3, 93, 1, 10};
    mergeSort(arr, 0, 10);
    mergeSort(names, 0, 5);
    insertionSortv2(arr2, 10);

    for(int i = 0; i < 11; i++){
        cout << arr[i] << " ";
    }
    cout << endl;

    for(int i = 0; i < 10; i++){
        cout << arr2[i] << " ";
    }
    cout << endl;

    for(int i = 0; i < 6; i++){
        cout << names[i] << " ";
    }
    cout << endl;
    return 0;
}