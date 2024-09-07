// Function to add two integers
int add(int a, int b) {
    return a + b;
}

// Function to subtract two integers
int subtract(int a, int b) {
    return a - b;
}

// Function to multiply two integers
int multiply(int a, int b) {
    int result = 0;
    for (int i = 0; i < b; i++) {
        result = add(result, a);  // Reusing the add function
    }
    return result;
}

// Function to divide two integers (integer division)
int divide(int a, int b) {
    int count = 0;
    while (a >= b) {
        a = subtract(a, b);  // Reusing the subtract function
        count++;
    }
    return count;
}

// Entry point of the program
int main() {
    // Test the functions
    int x = 10;
    int y = 5;

    // Add
    int sum = add(x, y);
    // Subtract
    int diff = subtract(x, y);
    // Multiply
    int product = multiply(x, y);
    // Divide
    int quotient = divide(x, y);

    // Printing results
    // Since we are not using any libraries, printing is not included

    return 0;
}
