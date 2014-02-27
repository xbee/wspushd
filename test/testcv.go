package main

import opencv "github.com/lazywei/go-opencv/opencv"

func main() {
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

