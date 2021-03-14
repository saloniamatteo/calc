## calc

This is a simple calculator written in C. It uses `readline` to get user input.

Initially written on January 25, 2021.

## Available commands
Currently, `calc` supports the following commands:

| Command  | Description             |
|----------|-------------------------|
| clear    | Clear the screen        |
| color    | Enable colored output   |
| exit     | Close this program      |
| help     | Print help/usage        |
| operands | Print list of operands  |
| ops      | Print list of operands  |
| quit     | Close this program      |
| specvals | Print [Special Values](#Special-Values)  |

## Flags
Currently, `calc` supports the following command-line flags:

| Flag | Description            |
|------|------------------------|
| `-h` | Print help and exit    |
| `-n` | Disable colored output |

## Example operations

| Command | Can be written as | Description    | Result    |
|---------|-------------------|----------------|-----------|
| 1 + 1   | 1 p 1             | Addition       | Returns 2 |
| 1 - 1   | 1 s 1             | Subtraction    | Returns 0 |
| 2 * 2   | 2 t 2             | Multiplication | Returns 4 |
| 4 / 2   | 4 d 2             | Division       | Returns 2 |
| 4 % 2   | 4 m 2             | Modulus        | Returns 0 |

## Special Values
You can (optionally) use Special Values, thanks to the `<math.h>` library.

Here's a table containing the currently implemented values.

| Special Value | Description                 |
|---------------|-----------------------------|
| E             | The value of Euler's number |
| Pi            | The value of Pi             |
| Pi2           | Pi / 2                      |
| Pi4           | Pi / 4                      |
| 1Pi           | 1 / Pi                      |
| 2Pi           | 2 / Pi                      |
| Pisq          | Pi² (Pi * Pi)               |

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

It's also possible to just print a value/number, for example:

```
calc> pi
3.1415926536
calc> 7
7
calc> e
2.7182818285
calc> pisq
9.8696044011
calc> 3
3
```

## Why was this written?

This was written because I was tired of using the slow Python as a command-line calculator.

I wanted something fast, that I could modify whenever I wanted to.

So, I quickly started writing the barebones of the calculator. It's nothing fancy, really, but a nice tool to have.

More features might be added, if necessary.

For any help, either:
	1. Create an issue;
	2. Open a pull request;
	3. Send me an email [(saloniamatteo@pm.me)](mailto:saloniamatteo@pm.me).
