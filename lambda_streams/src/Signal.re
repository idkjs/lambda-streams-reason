type t('a) =
  | Data('a)
  | EndOfSignal;

let pure = (value: 'a): t('a) => Data(value);

let empty = (): t('a) => EndOfSignal;

let default = default_value =>
  fun
  | Data(value) => value
  | EndOfSignal => default_value;

let satisfies = f =>
  fun
  | Data(value) => f(value)
  | EndOfSignal => false;

let map = (f: 'a => 'b, ma: t('a)): t('b) =>
  switch (ma) {
  | Data(value) => Data(f(value))
  | EndOfSignal => EndOfSignal
  };

let filter = (f: 'a => bool, ma: t('a)): t('a) =>
  switch (ma) {
  | Data(value) when f(value) => Data(value)
  | Data(_)
  | EndOfSignal => EndOfSignal
  };

let fold = (f: ('a, 'b) => 'a, init: 'a, ma: t('b)): 'a =>
  switch (ma) {
  | Data(value) => f(init, value)
  | EndOfSignal => init
  };

let from_option =
  fun
  | Some(value) => Data(value)
  | None => EndOfSignal;

let to_option =
  fun
  | Data(value) => Some(value)
  | EndOfSignal => None;
