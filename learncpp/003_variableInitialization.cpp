#include <iostream>

int main()
{

    int a = 10;

    // assignment of a value to a variable using assignment operator
    // By default, assignment copies the value on the right-hand side of the = operator to the variable on the left-hand side of the operator.
    // This is called copy assignment.

    // When a variable is defined, you can also provide an initial value for the variable at the same time.
    // This is called initialization. The value used to initialize a variable is called an initializer.

    // There are 6 basic ways to initialize variables in C++:

    int p;     // no initializer (default initialization)
    int b = 5; // initializer after equals sign (copy initialization)
    int c(6);  // initializer in parenthesis (direct initialization)

    // List initialization methods (C++11) (preferred)

    int d{7};    // initializer in braces (direct list initialization)
    int e = {8}; // initializer in braces after equals sign (copy list initialization)
    int f{};     // initializer is empty braces (value initialization) // value initialization / zero initialization to value 0

    int width{4.5}; // error: a number with a fractional value can't fit into an int

    /*

    1. std::cout is buffered

    Consider a rollercoaster ride at your favorite amusement park. Passengers show up (at some variable rate) and get in line. 
    Periodically, a train arrives and boards passengers (up to the maximum capacity of the train). 
    When the train is full, or when enough time has passed, the train departs with a batch of passengers, and the ride commences. Any passengers unable to board the current train wait for the next one.

    This analogy is similar to how output sent to std::cout is typically processed in C++. Statements in our program request that output be sent to the console. However, that output is typically not sent to the console immediately. Instead, the requested output “gets in line”, and is stored in a region of memory set aside to collect such requests (called a buffer). Periodically, the buffer is flushed, meaning all of the data collected in the buffer is transferred to its destination (in this case, the console).

    2. std::endl vs ‘\n’

    Using std::endl can be a bit inefficient, as it actually does two jobs: it moves the cursor to the next line of the console, and it flushes the buffer. When writing text to the console, we typically don’t need to flush the buffer at the end of each line. It’s more efficient to let the system flush itself periodically (which it has been designed to do efficiently).

    Because of this, use of the ‘\n’ character is typically preferred instead. The ‘\n’ character moves the cursor to the next line of the console, but doesn’t request a flush, so it will often perform better. The ‘\n’ character is also more concise since it’s both shorter and can be embedded into existing text.

    3. std::cin, std::cout, the insertion operator (<<) and the extraction operator (>>)


    4. A literal (also known as a literal constant) is a fixed value that has been inserted directly into the source code.. This is why literals are called constants.

    */


    return 0;
}
