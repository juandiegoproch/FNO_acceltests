#include <stdio.h>
#include "include/gemmini_testutils.h" // Gives access to safe bare-metal utilities

int main() {
    // Standard bare-metal systems use custom print abstractions 
    // or un-optimized printf streams
    printf("Hello World from Gemmini Baremetal!\n");
    
    // Explicitly exit with 0 so Spike knows the test passed successfully
    exit(0); 
}
