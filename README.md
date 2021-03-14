## calc

This is a simple calculator written in C. It uses `readline` to get user input.

Initially written on January 25, 2021.

The available commands are:
+ clear
+ exit
+ help
+ operands
+ ops
+ quit
+ specvals

You can also use the following flags:
+ `-h`: print help and exit
+ `-n`: disable colored output

Here are a few examples:

| Command | Can be written as | Description    | Result    |
|---------|-------------------|----------------|-----------|
| 1 + 1   | 1 p 1             | Addition       | Returns 2 |
| 1 - 1   | 1 s 1             | Subtraction    | Returns 0 |
| 2 * 2   | 2 t 2             | Multiplication | Returns 4 |
| 4 / 2   | 4 d 2             | Division       | Returns 2 |
| 4 % 2   | 4 m 2             | Modulus        | Returns 0 |

## Special Values
You can (optionally) use Special Values, thanks to the `<math.h>` library.

The current implemented values are:
+ E: The value of Euler's number
+ Pi: The value of Pi
+ Pi2: Pi / 2
+ Pi4: Pi / 4
+ 1Pi: 1 / Pi
+ 2Pi: 2 / Pi
+ PiSq: Pi² (Pi * Pi)

NOTE: these are case-insensitive, so don't worry about writing them all lowercase, uppercase, etc.

For example:

```
calc> Pi + 2
5.1415926536
calc> piSq / pi
3.1415926536
calc> e + Pi2
4.2890781553
calc> pi4 * 1pI
0.2500000000
```

## Why was this written?

This was written because I was tired of using the slow Python as a command-line calculator.

I wanted something fast, that I could modify whenever I wanted to.

So, I quickly started writing the barebones of the calculator. It's nothing fancy, really, but a nice tool to have.

More features might be added, if necessary.
