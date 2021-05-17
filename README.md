## calc

This is a simple calculator written in C. It uses `readline` to get user input.

Initially written on January 25, 2021.
Current version can be found in `calc.c`,
or by running `make version` (see [Installation](#Installation))

If you plan to work on `calc`, be sure to read `INDENTATION`.

Support this project: https://saloniamatteo.top/donate.html

## Available commands
Currently, `calc` supports the following commands:

| Command      | Description                             |
|--------------|-----------------------------------------|
| `calc`       | Enter just-calculator mode (see [1])    |
| `clear`      | Clear the screen                        |
| `color`      | Enable colored output                   |
| `examples`   | Enable examples in help section         |
| `exit`       | Close this program                      |
| `flags`      | Enable flags in help section            |
| `help`       | Print help/usage                        |
| `nocalc`     | Disable just-calculator mode (see [1])  |
| `nocolor`    | Disable colored output                  |
| `noexamples` | Disable examples in help section        |
| `noflags`    | Disable flags in help section           |
| `operands`   | Print list of operands                  |
| `ops`        | Print list of operands                  |
| `quit`       | Close this program                      |
| `rpn`        | Enter [RPN mode](#RPN-mode)             |
| `specvals`   | Print [Special Values](#Special-Values) |

[1]: the `calc` command is only available outside of just-calculator mode, and
the `nocalc` command is only available inside just-calculator mode.

## Flags
Currently, `calc` supports the following command-line flags:

| Flag | Long Flag       | Description                      |
|------|-----------------|----------------------------------|
| `-c` | `--calc`        | Enter just-calculator mode       |
| `-e` | `--no-examples` | Disable examples in help section |
| `-f` | `--no-flags`    | Disable flags in help section    |
| `-h` | `--help`        | Print help and exit              |
| `-m` | `--no-cmp`      | Disable program compilation info |
| `-n` | `--no-color`    | Disable colored output           |
| `-r` | `--rpn`         | Enter [RPN mode](#RPN-mode)      |

NOTE: Flag order matters! ([cefmnh])

## Example operations

| Command    | Can be written as | Description          | Result        |
|------------|-------------------|----------------------|---------------|
| `1 + 1`    | `1 p 1`           | Addition             | Returns 2     |
| `1 - 1`    | `1 s 1`           | Subtraction          | Returns 0     |
| `2 * 2`    | `2 t 2`           | Multiplication       | Returns 4     |
| `4 / 2`    | `4 d 2`           | Division             | Returns 2     |
| `4 % 2`    | `4 m 2`           | Modulus              | Returns 0     |
| `1 < 16`   | `1 l 16`          | Bit-shifting (left)  | Returns 65536 |
| `4096 > 1` | `4096 r 4`        | Bit-shifting (right) | Returns 2048  |

*(See bit-shifting info [here](#Bit-Shifting))*

## RPN mode
`calc` includes a Reverse Polish Notation (RPN) mode, which you can access with
the `-r` and `--rpn` flags, as well as the `rpm` command.

In RPN, the following equation

```
((1 + 2) + (3 - 4)) * 2
```

would be written as

```
1 2 + 3 4 - + 2 *
```

giving the user the ability to write more complex equations, without the
difficulty of parsing parentheses.

At the time of writing, the special values are not yet implemented in this mode.

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

## Bit Shifting
In C, there are two kinds of bit-shifting:
- Bit-shifting to the left (`<<`)
- Bit-shifting to the right (`>>`)

___What is bit shifting?___
Bit shifting is, well, used to shift bits.
In the computer world, integers are represented using 0s and 1s;
For example, if we take the number `7`, it would be represented as follows:

| 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 |
|-----|----|----|----|---|---|---|---|
| 0   | 0  | 0  | 0  | 0 | 1 | 1 | 1 |

(That is `00000111` in binary; for simplicity's sake we'll just do a byte)

Let's say we want to convert `30` to binary. In this case, we'll have:
| 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 |
|-----|----|----|----|---|---|---|---|
| 0   | 0  | 0  | 1  | 1 | 1 | 1 | 0 |

(`00011110` in binary)

Now, let's talk about bit shifting.
When we "shift a bit", it means to move every bit, for example, 
one place to the right (`>>`) or to the left (`<<`).

Let's say I want to bit shift 7 one place to the right:

| 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 |
|-----|----|----|----|---|---|---|---|
| 0   | 0  | 0  | 0  | 0 | 0 | 1 | 1 |

Now, the only values left are `2` and `1`, which make up `3`.

Instead, let's try to do `30`:

| 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 |
|-----|----|----|----|---|---|---|---|
| 0   | 0  | 0  | 0  | 1 | 1 | 1 | 1 |

As you can see, the only digits left now are `8`, `4`, `2`, and `1`,
which make up `15`.

This time, let's do a bigger value, like `124`:

| 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 |
|-----|----|----|----|---|---|---|---|
| 0   | 1  | 1  | 1  | 1 | 1 | 0 | 0 |

Will become `62`:

| 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 |
|-----|----|----|----|---|---|---|---|
| 0   | 0  | 1  | 1  | 1 | 1 | 1 | 0 |

The same applies to bit-shifting to the right:
Let's try to bit-shift a fancy number: `92` (64 + 16 + 8 + 4):

| 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 |
|-----|----|----|----|---|---|---|---|
| 0   | 1  | 0  | 1  | 1 | 1 | 0 | 0 |

The result will be `46`:

| 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 |
|-----|----|----|----|---|---|---|---|
| 0   | 0  | 1  | 0  | 1 | 1 | 1 | 0 |

***

Bit shifting is useful to calculate powers of 2, for example:

- 2^16: `1 << 16`: 65536
- 2^30: `1 << 30`: 2147483648 (32 bit limit)
- 2^31: `1 << 31`: 4294967296 (64 bit limit)

## Installation
`calc` uses GNU AutoTools to increase its portabilty and flexibility.

Normally, users should run the following commands, to install `quiz`:

```bash
./configure
make
make install
```

If, for some reason, you cannot run the commands above, run `autoreconf --install`, then retry.

## Why was this written?
This was written because I was tired of using the slow Python as a command-line calculator.

I wanted something fast, that I could modify whenever I wanted to.

So, I quickly started writing the barebones of the calculator. It's nothing fancy, really, but a nice tool to have.

More features might be added, if necessary.

For any help, either:
- Create an issue;
- Open a pull request;
- Send me an email [(saloniamatteo@pm.me)](mailto:saloniamatteo@pm.me).
