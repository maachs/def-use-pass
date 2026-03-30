int foo(int a, int b);

int main() {
    foo(1,2);
}

int foo(int a, int b) {
    int c = a + b;
    int d = c * 2;
    if (d > 10) {
        return d;
    }
    return c;
}
