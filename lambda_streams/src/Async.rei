/** Generalizes the notion of an asynchronous stream. */
type t('a) = pri ('a => unit) => unit;

type input('a) = t('a);

/** Asynchronous output streams are repesented identically to input streams ({i push}-based) .*/

type output('a) = t('a);

/** Represents a connection-based input stream with a synchronous output stream to close it. */

type connection('a) = Connection.t(input('a), Sync.output(unit));

/** Represents a connection-based input stream with an asynchronous output stream to close it. */

type connection'('a) = Connection.t(input('a), output(unit));

let make: (('a => unit) => unit) => t('a);

/**
 Listens for values from an asynchronous stream, somewhat similar to {!Sync.next} but push-based
 instead of pull-based.
 */

let listen: ('a => unit, t('a)) => unit;

let pure: 'a => t('a);

let map: ('a => 'b, t('a)) => t('b);

let filter: ('a => bool, t('a)) => t('a);

let scan: (('b, 'a) => 'b, 'b, t('a)) => t('b);

module type INTERVAL = {
  /**
   This functor is for cross-platform compatibility. The functionality needs to work like
   {{:https://bucklescript.github.io/bucklescript/api/Js.Global.html#VALsetInterval} Js.Global.setInterval}.
   */;

  type interval_id;

  let set_interval: (unit => unit, int) => interval_id;

  let clear_interval: interval_id => unit;
};

module Interval:
  (I: INTERVAL) =>
   {
    /**
   Creates an infinite async stream that enumerates the natural numbers every [ms].

   If this stream is converted to a finite stream with {!Lambda_streams.Finite.Async.take'} it will
   still be considered active (the interval will not be cleared). It will just stop sending signals
   after whatever [n] was passed to [take'].

   To get an infinite async stream that can be converted later into a finite stream that ends, use
   {!make} and pass the output stream that closes it to {!Lambda_streams.Finite.Async.take'}.
   */

    let forever: (~ms: int) => t(int);

    /**
   Creates an async stream that enumerates the natural numbers every [ms]. This is a
   connection-based version that get closed (no more signals are sent) when the output stream is
   called.
   */

    let make: (~ms: int) => connection(int);
  };
