# Vaiven

Vaiven is a little JIT programming language that I plan to eventually make into
a little domain language for the terminal. Its codebase is currently under
15kloc, but it has an interpreter, a good-but-not-great compiler that profiles
itself for type info that's used to create SSA-optimized code, and that only
runs when the "hot" conditions are met.

## Building:

```
git clone https://github.com/asmjit/asmjit.git
make
```

Tested with clang and gcc

# Using

The current workflow of vaiven is to run over pipes, ie,

```
your_program | vvn
```

Frequently `your_program` will be a bash script. But if you have just a `.vvn`
file to run, you can simply `cat your_file.vvn | vvn`.

Vaiven is currently set to work as a REPL, so you can also just run `vvn` and
experiment there.

## Interpreting

Anything top-level in the vaiven program is evaluated and (currently) printed.

```
  1 + 1
  Int : 2

  "foo" + "bar"
  foobar
```

This is all run in an interpreter, which assumes the code never runs more than
once, making it the fastest execution of unknown code, once. You can define
loops in the interpreter, but you should not. You should put those in a
function where they will be compiled.

## Ints

Currently there are not yet doubles. But standard int operations `+`' `*`, `/`,
`-`, `>`, `<`, etc. work. Binary operations are not yet added though, nor is
modulo.

## Strings

Strings are much like javascript. You can not yet use single quotes for them,
but you can concatenate them with `+`, and they are immutable.

You can also call `toString` on any value to do type coercion.

## Functions

Functions can be defined with or without parameters. Parameters are not typed.
Like all control constructs in vaiven, it ends with "end".

Calling a function (such as `print` in this example) looks just like c syntax.

```
  fn foo
    print("hello!")
  end
```

Or, with a parameter,

```
  fn greet of name is
    print("hello " + name + "!")
  end
```

With multiple parameters, both definition and invocation can be comma separated:

```
fn twoArgs of x, y is
  ...
end

twoArgs(1, 2)
```

or newline separated:

```
fn twoArgs of
  x
  y
is
  ...
end

twoArgs(
  1
  2
)
```

(or both).

### Return

Functions will automatically return the final statement, so you don't need to
use a keyword to return a value in a function

```
  fn square of x is
    x * x
  end
```

This will not work yet, however, with if and for statements. So for early return
you can use the keyword `ret`.

```
  fn square of x is
    ret x * x
  end
```

In this case, these do the same thing.

Currently, you cannot simply `ret`, you must `ret` a value.

## Variables

Variables are scoped to their control structures and/or to functions, and/or
globally. Global variables are only accessable from outside functions, and
variables cannot share a name, even in subscopes (shadowing not allowed).

```
  var x = 4
```

Currently all variables must be initialized when declared.

## If statements

If statements do not require parenthesis or brackets in vaiven.

```
  if true
    print("true")
  end
```

You can also use `else` like so:

```
  if true
    print("true")
  else
    print("false")
  end
```

### Else if

You can also use `else if` as you would expect, and it does not have a special
keyword.

```
  if hasShirt
    print("has shirt")
  else if hasShoes
    print("has shoes")
  else
    print("no service")
  end
```

be careful that this is not the same:

```
  if hasShirt
    print("has shirt")
  else
  if hasShoes
    print("has shoes")
  else
    print("no service")
  end
```

This actually requires one more `end` to finish the statement. See the section
on semicolons for why.

### One line control structures

Control structures can be put on a single line, but you must use the `do`
keyword, semicolons, and `end`. This may be relaxed in the future.

```
  if true do print("true"); end

  if false
    ...
  else if foo do print("foo"); end
  else do print("false"); end
```

You can see the section on semicolons for why.

### For

There is only one type of loop so far in vaiven, and that's a for condition
loop. It simply runs the following block as long as the condition is true.

```
  for x < 10
    x = x + 1
  end
```

Like if and else if, this can be put on one line using `do`, semicolons, and
`end`.

```
  for x < 10 do x = x + 1; end
```

## Lists

Lists work just like javascript. You can create a list with literal-based
syntax, assign to indexes and retrieve from indexes.

```
  var list = [1, 2, 3]
  list[0] // 1
  list[0] = 2
  list[0] // 2
```

You can also ask for indexes that don't exist in a list, and you'll get back
`void`.

```
  list[5] // void
```

And you can assign anywhere in a list, regardless of its current length. If
needed, the list will resize and fill in the empty spaces with `void`.

```
  var list = [];
  list[2] = 2;
  list // [void, void, 2]
```

You will get a type error if you attempt to use a non-integer as an index.

## Objects

Object literals are not yet supported, but you can make an object via the
function `object`. It can then be used like an associative array, or using `.`
member syntax.

```
  var o = object()
  o.foo = "bar"
  o.foo // "bar"
  o["foo"] // "bar"

  o["baz"] // void
  o.baz // void

  o[myVar] = "dynamic property"
  o[myVar] // "dynamic property"
```

Soon object literals will be added.

## Type rules

Vaiven has some loosely defined type rules, and almost no type-coersion.



## Semicolons

Vaiven has been made to have optional semicolons for pretty syntax, with
predictable effects and minimal whitespace significance. It has also been
designed so that newlines are special, not
[else if](http://www.mikedrivendevelopment.com/2016/06/else-if-is-not-special-except-in-python.html).

You can write vaiven code with no special newline handling, by always using
semicolons and `do`.

```
  if true do
    print("stuff");
  end

  for false do
    print("doesn't execute");
  end
```

This is how the language looked originally.

Now, in places where `do` or `semicolon` are expected, a newline will suffice.
Therefore, the above code is exactly equivalent to

```
  if true
    print("stuff")
  end

  for false
    print("doesn't execute")
  end
```

and this is why the oneline versions of if/for/else look like the original
grammar; `do` is the semicolon of control structors, and so both are optional.
And commas are the semicolons of parameter lists, so we made those optional too.

```
  foo(
    1 + 2
    3 + 4
  )
```

Newlines can also fill in for `is`, and thats why the shorthand function syntax
works:

```
fn foo
  print("foo")
end
```

Oneline functions with no arguments require `is`.

```
fn foo is print("foo") end
```

and once you start listing arguments, we need `is` to know when those arguments
end.

### Else if isn't special

I am of the opinion that
[else if should not be special](http://www.mikedrivendevelopment.com/2016/06/else-if-is-not-special-except-in-python.html),
and so in vaiven, it is not.

You may have noticed, if `do` is the semicolon of statements, and `else if` is
not special, then how does `else if` work without a `do` or a newline?

Well, `else` can be followed by any control structure. This allows the `else` to
not require an `end`, which gives the appearance of creating a hybrid conrol
structure. It is not unique to else if, as you can also do else for.

```
  if ...
    ...
  else for x
    ...
    x = x + 1
  end
```

and the same logic will soon be applied to `if` and `for` as well:

```
  if x for y
    ...
    y = y + 1
  end

  for z if q
    ...
  end
```

So I guess for the moment `else if` is kinda special. But no more special than
`else for`.

## Performance

Feel free to run `bench/bench.sh` for numbers specific to your system.

The one cross language benchmark we have, recursive fib, is barely faster than
v8 though a bit slower than dart. *But* this is not to say that vaiven is twice
as fast as v8. Its just to say that the right code is exactly as performant as
you would expect from a high-quality JIT -- see optimization tips for details
about what we *don't* do well, as its still a pretty long list.

In general, compiled code is up to 100x faster than interpreted code. And
usually the hot code is about 2x faster than that, though it can be even higher.

For instance, the recursion based test currently does a better job reducing
polymorphic + overhead, and finishes in 0.000 seconds (too low to measure) vs
about 20 second for the equivalent interpreted code on my system.

## Under the hood: Optimization tips

Firstly, anything that will be executed more than once should be put in a
function. Anything executed only once should not. The interpreter is about 2x
faster than compilation and execution.

However, for anything that runs multiple times (ie has loops, recursion, etc),
the compilation is much much much faster than the interpreter. The interpreter
is deliberately using the slowest interpreting pattern out there, a visitor.
This is because it is faster than compiling to bytecode before running, but only
for one run.

After a certain count of times, function will self optimize using argument type
data. This is tracked on previous executions in a bitmap of, has x been an int,
string, etc.

Currently, when an argument has been multiple types, no optimization is done
against that parameter. It is therefore critical that in the warm-up stage, you
call functions monomorphically where that is how they will run.

On the other hand, after optimization occurs, code violating the profiler's
data will run even slower. Its not as slow as full blown dynamic deoptimization,
its simply a guard check in the function header, but its still slower than the
original code. So if you have a function which may accept strings or ints, and
only feed it ints during the warm up phase, it will be slower when it gets
strings later.

Vaiven also will not yet throw away optimized code when it seems to be missed
frequently, so be careful that you don't teach it bad tricks.

At the very least, even if a function is completely polymorphic and has no
optimizability, its still best to optimize it as that. Some optimizations (such
as inlining) are still possible, and the data collection will be stripped out as
well.

Since vaiven doesn't yet have true dynamic deoptimization, only guarded
optimizations, be wary of polymorphic code in the middle of your function.
Things like fetching from an object or an array will not be profiled and
optimized, since we won't have any fall back strategy for when we don't get what
we expected.

For the same reason, slow loops cannot optimize themselves mid-loop like v8 and
other JITs. Its best to warm up a loop ahead of time, perhaps with an argument
for how many iterations should run, before running a long slow loop.

Inlining is also slightly controllable, in a few ways. If you see something is
being inlined which shouldn't, you can add a bunch of dead code to trick the
inlining into fear of code explosion. On the flip side, you can add arguments
which are unused, since those arguments represent a chance at optimization. With
the right compilation flags, you can track the compilers inlining decisions to
make use of these tricks.

# To Do 

### Core language:

* void as a keyword (and decide void, null, nil, nada?)
* support doubles
* object literals
* unary -/+
* += style operators
* binary operators?
* &&, || with short circuiting
* stack traces
* imports
* postfix function syntax
* void return
* if for / for if
* for over lists (and objects?)
* larger std library
* falsy values (at the very least void and empty string?)
* have `end` act as a semicolon?
* check type of object at runtime
* perhaps more type coercions, like toString() before property access or
parseInt before array access

### Performance:

* Common Subexpression Elimination
* Loop Invariant Code Motion
* Bounds check elimination
* track "constant" lists and objects
* Precise GC and/or better allocator for heap
* Custom string/list/object implementation for inlinability
* fuzz test for security/stability
* dynamic deoptimization/on-stack optimization
* profile jumps to reduce branching
* profile jumps to make better inlining decisions
* profile hash lookups to make for faster property access
* better benchmarks
* align jumps

### Domain language

Once vaiven runs as a TUI framework, it needs these things. Currently, vaiven is
just a little VM that doesn't need any domain-related aspects.

* Choose a strategy: fork the core language? Plugin model? Neither?
* fully spec language
* ...

# Legal

*This is not an official google product*. Please see LICENSE for more.
