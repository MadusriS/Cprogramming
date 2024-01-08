#include <stdio.h>
int binsearch(int x, int arr[], int n);
int main(void) {
    int arr[] = {1, 5, 6, 7, 9, 19, 44, 48, 49, 50, 63};
    printf("%d", binsearch(7, arr, 10));
    return 0;
}
int binsearch(int x, int arr[], int n) {
    int low, high, mid;
    low = 0;
    high = n - 1;
    mid = (low + high) / 2;
    while (low < high && x != arr[mid]) {
        if (x > arr[mid])
            low = mid + 1;
        else
            high = mid - 1;

        mid = (low + high) / 2;
    }
    if (x == arr[mid])
        return mid;
    else
        return -1;
}

