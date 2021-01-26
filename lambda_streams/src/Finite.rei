/** Represents the finite version of a {!Lambda_streams.Sync} stream. */
module Sync: {
  type input('a) = Sync.input(Signal.t('a));

  type output('a) = Sync.output('a);

  let make_output: ('a => unit) => output('a);

  let send: ('a, output('a)) => unit;

  let pure: 'a => input('a);

  let empty: unit => input('a);

  let from_list: list('a) => input('a);

  let from_array: array('a) => input('a);

  /** Pipes an input stream into an output stream. */

  let pipe: (output('a), input('a)) => unit;

  let map: ('a => 'b, input('a)) => input('b);

  let filter: ('a => bool, input('a)) => input('a);

  let take: (int, input('a)) => input('a);

  /* Takes [n] elements from an infinite stream and converts it to a finite one. */
  let take': (int, Sync.input('a)) => input('a);

  let skip: (int, input('a)) => input('a);

  let until: ('a => bool, input('a)) => input('a);

  let fold_left: (('a, 'b) => 'a, 'a, input('b)) => 'a;

  let concat: list(input('a)) => input('a);

  let flatten: input(input('a)) => input('a);

  let to_rev_list: input('a) => list('a);

  let to_list: input('a) => list('a);

  let to_array: input('a) => array('a);
};

/** Represents the finite version of an {!Lambda_streams.Async} stream. */

module Async: {
  type t('a) = Async.t(Signal.t('a));

  let pure: 'a => t('a);

  let empty: unit => t('a);

  let from_list: list('a) => t('a);

  let map: ('a => 'b, t('a)) => t('b);

  let filter: ('a => bool, t('a)) => t('a);

  let scan: (('b, 'a) => 'b, 'b, t('a)) => t('b);

  let take: (int, t('a)) => t('a);

  /**
   Takes [n] elements from an infinite stream and converts it to a finite one. Optionally takes an
   output stream ([?close]) to close the connection at {!Signal.EndOfSignal} if the async stream is
   connection-based.
   */

  let take': (~close: Sync.output(unit)=?, int, Async.t('a)) => t('a);
};
