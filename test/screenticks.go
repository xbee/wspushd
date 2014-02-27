package main

import (
	"code.google.com/p/go.net/websocket"
	// "code.google.com/p/snappy-go/snappy"
	"bytes"
	"fmt"
	opencv "github.com/lazywei/go-opencv/opencv"
	"github.com/vova616/screenshot"
	"image/png"
	"log"
	"os"
	"os/signal"
	"syscall"
	"time"
)

func testcv() {
	filename := "bert.png"
	srcImg := opencv.LoadImage(filename)
	if srcImg == nil {
		panic("Loading Image failed")
	}
	defer srcImg.Release()
	resized1 := opencv.Resize(srcImg, 400, 0, 0)
	resized2 := opencv.Resize(srcImg, 300, 500, 0)
	resized3 := opencv.Resize(srcImg, 300, 500, 2)
	opencv.SaveImage("resized1.png", resized1, 0)
	opencv.SaveImage("resized2.png", resized2, 0)
	opencv.SaveImage("resized3.png", resized3, 0)
}

func ss(t time.Time, ws *websocket.Conn) {
	img, err := screenshot.CaptureScreen()
	if err != nil {
		panic(err)
	}

	// fn := fmt.Sprintf("%d-%d-%d_%d:%d:%d.png", t.Year(), t.Month(), t.Day(), t.Hour(), t.Minute(), t.Second())
	// f, err := os.Create(fn)
	// if err != nil {
	// 	panic(err)
	// }
	var b bytes.Buffer
	err = png.Encode(&b, img)
	if err != nil {
		panic(err)
	}
	// send it to server

	if err = websocket.Message.Send(ws, b.Bytes()); err != nil {
		panic(err)
	}
	// f.Close()
}

func main() {

	interrupt := make(chan os.Signal, 1)
	signal.Notify(interrupt, os.Interrupt)
	signal.Notify(interrupt, syscall.SIGTERM)

	println("Please press CTRL-C to exit.")

	origin := "http://localhost/"
	url := "ws://localhost:5555/ss"
	ws, err := websocket.Dial(url, "", origin)
	if err != nil {
		log.Fatal(err)
	}

	ticker := time.NewTicker(time.Millisecond * 30000)
	go func() {
		for t := range ticker.C {
			fn := fmt.Sprintf("%d-%d-%d %02d:%02d:%02d", t.Year(), t.Month(), t.Day(), t.Hour(), t.Minute(), t.Second())
			fmt.Println("Tick at ", fn)
			ss(t, ws)
		}
	}()

	go func() {
		for sig := range interrupt {
			fmt.Printf("\nreceived ctrl+c(signal %#v)\n", sig)
			ticker.Stop()
			fmt.Println("Ticker stopped")
			os.Exit(0)
		}
	}()

	// time.Sleep(time.Millisecond * 35000)
	// testcv()
	// ticker.Stop()
	// fmt.Println("Ticker stopped")

	for {
		time.Sleep(1000)
	}
}
