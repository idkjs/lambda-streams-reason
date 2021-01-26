/** Generalizes the notion of a synchronous, readable input stream. */
type input('a) = pri unit => 'a;

/** Generalizes the notion of a synchronous, writable output stream. */

type output('a) = pri 'a => unit;

/** Represents a connection-based input stream with an output stream to close it. */

type connection('a) = Connection.t(input('a), output(unit));

let make_input: (unit => 'a) => input('a);

let make_output: ('a => unit) => output('a);

/**
 Creates a mutator: a pair of input and output streams that together behave like a mutable variable.
 Mutating the value is accomplished by sending data to the output stream. Fetching the current
 value is done by reading the input stream.
 */

let make_mutator: (~initial: 'a) => (input('a), output('a));

let pure: 'a => input('a);

/** Creates an input stream that enumerates the natural numbers. */

let enumerate: unit => input(int);

/** Gets the next value (or {i current} value if it's a behavior) from the input stream. */

let next: input('a) => 'a;

/**
 Sends a single value to the output stream. Semantically equivalent to:
 {[Finite.Sync.pure value |> Finite.Sync.pipe output_stream]}
 But it's more efficient because it doesn't involve instatiating an input stream.
 */

let send: ('a, output('a)) => unit;

/** Pipes an input stream into an output stream. */

let pipe: (output('a), input('a)) => unit;

/** Like fold_left but for infinite streams, ending at signal n. */

let accumulate: (int, ('b, 'a) => 'b, 'b, input('a)) => 'b;

let map: ('a => 'b, input('a)) => input('b);

/**
 Filters the input stream based on a predicate. This function returns a stream that, when invoked,
 will continue to invoke the original input stream until a value that satisfies the predicate is
 given, which is then returned. This means that an invocation of the returned stream may possibly
 never terminate.

 For example:

 {[enumerate () |> filter ((>=) 10)]}
 Will block and never terminate after [10] invocations, because after those invocations any future
 values of [enumerate ()] will never be less than [10].

 On the other hand,
 {[
  let is_even x = x mod 2 = 0 in
  enumerate () |> filter is_even
  ]}
 Will always return a value, because there will always be a successor to a natural number that is
 even (every other number).
 */

let filter: ('a => bool, input('a)) => input('a);

let scan: (('b, 'a) => 'b, 'b, input('a)) => input('b);
