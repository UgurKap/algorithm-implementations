//
// Created by ugur on 29.09.2019.
//

#ifndef ALGOI_SORTS_H
#define ALGOI_SORTS_H

#include <iostream>
#include <cmath>

template <class T>
void insertionSort(T *arr, size_t length, char mode = 'f'){

    /*
     * Uses the version in Skiena's book.
     * Uses insertion sort algorithm to sort the given array. It works in place.
     * First parameter is the array to sort.
     * Second parameter is the length of the array.
     * Third parameter is the mode for sorting. If user supplies a 'r' value, sorting is in descending order.
     */

    for(size_t i = 1; i < length; i++){
        if(mode == 'r'){
            for(size_t j = i; j > 0; j--){
                if(arr[j] >= arr[j - 1]){
                    std::swap(arr[j], arr[j - 1]);
                } else break;
            }
        } else {
            for(size_t j = i; j > 0; j--){
                if(arr[j] <= arr[j - 1]){
                    std::swap(arr[j], arr[j - 1]);
                } else break;
            }
        }

    }
}

template <class T>
void insertionSortv2(T *arr, size_t length, char mode = 'f'){

    /*
     * Uses the version in CLRS.
     * Uses insertion sort algorithm to sort the given array. It works in place.
     * First parameter is the array to sort.
     * Second parameter is the length of the array.
     * Third parameter is the mode for sorting. If user supplies a 'r' value, sorting is in descending order.
     */

    for(int j = 1; j < length; j++){
        T key = arr[j];
        int i = j - 1;
        while(i >= 0 && (arr[i] > key)){
            arr[i + 1] = arr[i];
            i = i - 1;

        }
        arr[i + 1] = key;
    }
}

template <class T>
void merge(T *arr, int start_point, int end_point1, int end_point2){
    /*
     * Merges two sorted arrays and creates a new sorted array. Process happens in place.
     */
    int first_length = end_point1 - start_point + 1;
    int second_length = end_point2 - end_point1;

    T left_arr[first_length];
    T right_arr[second_length];

    for(int i = 0; i < first_length; i++){
        left_arr[i] = arr[start_point + i];
    }

    for(int i = 0; i < second_length; i++){
        right_arr[i] = arr[end_point1 + 1 + i];
    }

    int i = 0;
    int j = 0;
    for(int k = start_point; k <= end_point2; k++){
        if(i == first_length){
            arr[k] = right_arr[j];
            j += 1;
            continue;
        }
        if(j == second_length){
            arr[k] = left_arr[i];
            i += 1;
            continue;
        }

        if(left_arr[i] <= right_arr[j]){
            arr[k] = left_arr[i];
            i += 1;
        } else {
            arr[k] = right_arr[j];
            j += 1;
        }
    }
}

template <class T>
void mergeSort(T *arr, int start_point, int end_point){

    /*
     * Uses the version in Uğur Ali Kaplan's head which is based on CLRS. (Merging part)
     * Recursively sorts the given array with merge sort.
     * @start_point: Starting point of the part we focus on in the given large array
     * @end_point: End point of the part we focus on in the given large array
     */

    if(start_point < end_point){
        int new_end_pt = int(floor(double(start_point + end_point) / 2.0));
        mergeSort(arr, start_point, new_end_pt);
        mergeSort(arr, new_end_pt + 1, end_point);
        merge(arr, start_point, new_end_pt, end_point);
    }
}

#endif //ALGOI_SORTS_H
