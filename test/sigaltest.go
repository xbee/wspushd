package main

import (
	"fmt"
	"os"
	"os/signal"
	"time"
)

func main() {
	interrupt := make(chan os.Signal, 1)
	signal.Notify(interrupt, os.Interrupt)

	println("Please press CTRL-C to exit.")

	go func() {
		for sig := range interrupt {
			fmt.Printf("\nreceived ctrl+c(signal %#v)\n", sig)
			os.Exit(0)
		}
	}()
	for {
		time.Sleep(1000)
	}
}
