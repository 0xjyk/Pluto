void addscalar(int n, int m, double a[n][n*m+300], double x);

/*
int main() {
    double b[4][308];
    addscalar(4, 2, b, 2.17);
    return 0;
}
*/
void addscalar(int n, int m, double a[n][n*m+300], double x) {
    for (int i = 0; i < n; i++)
        for (int j = 0, k = n*m+300; j < k; j++)
            // a is a pointer to a VLA with n*m+300 elements
            a[i][j] += x;
}
