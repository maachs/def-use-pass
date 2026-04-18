#include <stdio.h>

extern int process_data(int a, int b);

int main() {
    int x = 10;
    int y = 20;

    int result = process_data(x, y);

    printf("Final result: %d\n", result);
    return 0;
}
