type t('a) = ('a => unit) => unit;

type input('a) = t('a);

type output('a) = t('a);

type connection('a) = Connection.t(input('a), Sync.output(unit));

type connection'('a) = Connection.t(input('a), output(unit));

let make = f => f;

let listen = (cb, stream) => stream(cb);

let pure = (value, cb) => cb(value);

let map = (f, stream, cb) => stream(value => cb(f(value)));

let filter = (f, stream, cb) =>
  stream(value =>
    if (f(value)) {
      cb(value);
    }
  );

let scan = (f, init, stream) => {
  let acc = ref(init);
  cb =>
    stream(value => {
      acc := f(acc^, value);
      cb(acc^);
    });
};

module type INTERVAL = {
  type interval_id;

  let set_interval: (unit => unit, int) => interval_id;

  let clear_interval: interval_id => unit;
};

module Interval = (I: INTERVAL) => {
  let forever = (~ms) => {
    let i = ref(0);
    cb =>
      I.set_interval(
        () => {
          i := i^ + 1;
          cb(i^);
        },
        ms,
      )
      |> ignore;
  };

  let make = (~ms) => {
    let i = ref(0)
    and interval = ref(None);
    let stream = cb =>
      interval :=
        Some(
          I.set_interval(
            () => {
              i := i^ + 1;
              cb(i^);
            },
            ms,
          ),
        );

    {
      Connection.stream,
      close:
        Sync.make_output(() =>
          switch (interval^) {
          | Some(interval') => I.clear_interval(interval')
          | _ => ()
          }
        ),
    };
  };
};
