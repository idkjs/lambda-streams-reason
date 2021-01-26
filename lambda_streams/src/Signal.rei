/** Represents a signal that either has data or is terminated. Used for {!Finite} streams. */;

type t('a) =
  | Data('a)
  | EndOfSignal;

let pure: 'a => t('a);

let empty: unit => t('a);

/** Get the signal data or the provided default value */

let default: ('a, t('a)) => 'a;

/** Check whether the signal satisfies the predicate. Returns [false] if it's {!EndOfSignal} */

let satisfies: ('a => bool, t('a)) => bool;

let map: ('a => 'b, t('a)) => t('b);

let filter: ('a => bool, t('a)) => t('a);

let fold: (('a, 'b) => 'a, 'a, t('b)) => 'a;

let from_option: option('a) => t('a);

let to_option: t('a) => option('a);
