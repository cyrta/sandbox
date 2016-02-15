/**
 * primefinder.go
 *
 * Author: Ryan Clough (caco@csh.rit.edu)
 *
 * Prime finder takes in a number as a command line argument,
 * and finds that number prime. Ex: argument "6" would find the 
 * 6th prime.
 *
 * flag -p prints the primes as they are discovered
 */
package main

import (
    "math"
    "container/list"
    "fmt"
    "flag"
    "strconv"
)

// Flag for printing the numbers it verifies
var printTrail = flag.Bool("p",false,"Print the trail of primes")

// Determine if a particular number is prime.
func IsPrime(number int, primes *list.List) (isPrime bool) {

    // For each currently found prime, check until the current prime is greater
    // than sqrt(number) (because that prime^2 will be greater than number)
    for i := primes.Front(); 
            i != nil && i.Value.(int) <= int(math.Sqrt(float64(number))); 
            i = i.Next() {

        // Use modulo to verify remainder
        if ((number % i.Value.(int)) == 0) { return false }
    }

    return true

}

// Start indexing discovered primes, return the desired index
func DiscoverPrime(index int) (result int){
    // Set up variables
    primes := list.New()
    primes.PushBack(2)

    // For 
    for i := 1; primes.Len()< index; i++ {
        current := primes.Back().Value.(int) + i
        if IsPrime(current, primes) {
            primes.PushBack(current)
            if *printTrail {
                fmt.Printf("%d is prime number %d\n", current, primes.Len())
            }
            i = 1
        }
    }

    return primes.Back().Value.(int)
}

func main() {
    // Deal with Flags
    flag.Parse()
    if flag.NArg() == 1 && (flag.NFlag() == 0 || flag.NFlag() == 1) {
        // Convert string argument to int
        number,err:= strconv.Atoi(flag.Arg(0))

        if err == nil {
            result := DiscoverPrime(number)
            fmt.Printf("The %dth prime is: %d\n", number, result);
        } else { // Deal with errors
            fmt.Printf("Encountered an error parsing the number you gave\n")
        }
    } else {
        fmt.Printf("You messed up your comand line args\n")
    }
}
