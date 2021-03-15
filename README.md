## calc

This is a simple calculator written in C. It uses `readline` to get user input.

Initially written on January 25, 2021, current version: `1.5`

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
| `specvals`   | Print [Special Values](#Special-Values) |

[1]: the `calc` command is only available outside of just-calculator mode, and
the `nocalc` command is only available inside just-calculator mode.

## Flags
Currently, `calc` supports the following command-line flags:

| Flag | Description                      |
|------|----------------------------------|
| `-c` | Enter just-calculator mode       |
| `-e` | Disable examples in help section |
| `-f` | Disable flags in help section    |
| `-h` | Print help and exit              |
| `-m` | Disable program compilation info |
| `-n` | Disable colored output           |

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

## Statically linking & building
If you want to statically link `calc` to share it, make sure you have `musl`, `musl-gcc` (not required on Android), and a copy of the source code of `libreadline` and `libncurses` (needed by `libreadline`).

Next, run the following:

```bash
# Getting Sources
# If you haven't cloned this repository yet
git clone https://github.com/saloniamatteo/calc; cd calc
mkdir -p libs/{readline,ncurses}
cd libs
# Get a copy of libreadline and libncurses' source code
git clone https://git.savannah.gnu.org/git/readline.git readline-src
git clone https://github.com/mirror/ncurses ncurses-src

# Compiling Sources
cd readline-src
# If compiling on Android, run the following command:
#./configure --prefix=$(pwd)/../readline CC=clang && make -kj$(nproc) && make install-static
./configure --prefix=$(pwd)/../readline CC=musl-gcc && make -kj$(nproc) && make install-static
cd ../ncurses-src
# If compiling on Android, run the following command:
#./configure --prefix=$(pwd)/../ncurses CC=clang && make -kj$(nproc) && make install
./configure --prefix=$(pwd)/../ncurses CC=musl-gcc && make -kj$(nproc) && make install

# Finally, Statically link & compile calc
cd ../..
make static-deb
```

It should compile succesfully; When compiling `static-deb` with `make`, the output should look like this:

```bash
[STATIC DEBUG]
gcc: warning: ./libs/readline/lib/libreadline.a: linker input file unused because linking not done
gcc: warning: ./libs/ncurses/lib/libncurses.a: linker input file unused because linking not done
CC calc.c
CC calc-debstatic.o
```

## Why was this written?
This was written because I was tired of using the slow Python as a command-line calculator.

I wanted something fast, that I could modify whenever I wanted to.

So, I quickly started writing the barebones of the calculator. It's nothing fancy, really, but a nice tool to have.

More features might be added, if necessary.

For any help, either:
- Create an issue;
- Open a pull request;
- Send me an email [(saloniamatteo@pm.me)](mailto:saloniamatteo@pm.me).
