
/*
 Simple pipeing stdio to stdout
*/

package main
 
import (
    "io"
    "os"
)
 
func main() {
    var input, err = os.Stdin, error(nil)
    if len(os.Args) > 1 {
        input, err = os.Open(os.Args[1])
    }
    if err != nil {
        panic(err)
    }
    if _, err = io.Copy(os.Stdout, input); err != nil {
        panic(err)
    }
}
