module Sync = {
  type input('a) = Sync.input(Signal.t('a));

  type output('a) = Sync.output('a);

  let make_output = Sync.make_output;

  let send = Sync.send;

  let pure = value => {
    let was_sent = ref(false);
    Sync.make_input @@
    (
      () =>
        was_sent^
          ? Signal.EndOfSignal
          : {
            was_sent := true;
            Data(value);
          }
    );
  };

  let empty = (): input('a) => Sync.make_input @@ (() => Signal.EndOfSignal);

  let from_list = list => {
    let list' = ref(list);
    Sync.make_input @@
    (
      () =>
        switch (list'^) {
        | [value, ...rest] =>
          list' := rest;
          Signal.Data(value);
        | [] => EndOfSignal
        }
    );
  };

  let from_array = array => {
    let index = ref(0);
    Sync.make_input @@
    (
      () =>
        switch (index^) {
        | index' when index' < Array.length(array) =>
          index := index' + 1;
          Signal.Data(array[index']);
        | _ => EndOfSignal
        }
    );
  };

  let pipe = (output_stream, input_stream) => {
    let more = ref(true);
    while (more^) {
      switch (Sync.next(input_stream)) {
      | Signal.Data(value) => output_stream |> Sync.send(value)
      | EndOfSignal => more := false
      };
    };
  };

  let map = (f, stream) =>
    Sync.make_input @@ (() => stream |> Sync.next |> Signal.map(f));

  let filter = (f, stream) =>
    Sync.make_input @@
    (
      () => {
        let value = ref @@ Sync.next(stream);
        while (switch (value^) {
               | Signal.Data(v) => (!) @@ f(v)
               | EndOfSignal => false
               }) {
          value := Sync.next(stream);
        };
        value^;
      }
    );

  let take = (n, stream) => {
    let index = ref(0);
    Sync.make_input @@
    (
      () => {
        let value =
          if (index^ < n) {
            Sync.next(stream);
          } else {
            Signal.EndOfSignal;
          };
        index := index^ + 1;
        value;
      }
    );
  };

  let take' = (n, stream) => {
    let index = ref(0);
    Sync.make_input @@
    (
      () => {
        let value =
          if (index^ < n) {
            Signal.Data(Sync.next(stream));
          } else {
            Signal.EndOfSignal;
          };
        index := index^ + 1;
        value;
      }
    );
  };

  let skip = (n, stream) => {
    let index = ref(0)
    and skipped = ref(false);
    Sync.make_input @@
    (
      () => {
        while (! skipped^) {
          if (index^ >= n) {
            skipped := true;
          } else {
            Sync.next(stream) |> ignore;
          };
          index := index^ + 1;
        };
        Sync.next(stream);
      }
    );
  };

  let until = (f, ma) =>
    Sync.make_input @@ (() => ma |> Sync.next |> Signal.filter(f));

  let fold_left = (f, init, stream) => {
    let more = ref(true)
    and result = ref(init);
    while (more^) {
      switch (Sync.next(stream)) {
      | Signal.Data(value) => result := f(result^, value)
      | EndOfSignal => more := false
      };
    };
    result^;
  };

  let concat = streams => {
    let streams' = ref(streams);
    let rec fetch_signal = s =>
      switch (s) {
      | [stream, ...rest] =>
        switch (Sync.next(stream)) {
        | Signal.Data(value) => Signal.Data(value)
        | EndOfSignal =>
          streams' := rest;
          fetch_signal(rest);
        }
      | [] => EndOfSignal
      };

    Sync.make_input @@ (() => fetch_signal(streams'^));
  };

  let flatten = stream_of_streams => {
    let current_stream = ref @@ Sync.next(stream_of_streams);
    let rec fetch = s =>
      switch (s) {
      | Signal.Data(stream) =>
        switch (Sync.next(stream)) {
        | Signal.EndOfSignal =>
          let stream' = Sync.next(stream_of_streams);
          current_stream := stream';
          fetch(stream');
        | value => value
        }
      | EndOfSignal => EndOfSignal
      };

    Sync.make_input @@ (() => fetch(current_stream^));
  };

  let to_rev_list = stream => {
    let list = ref([])
    and more = ref(true);
    while (more^) {
      switch (Sync.next(stream)) {
      | Signal.Data(value) => list := [value, ...list^]
      | EndOfSignal => more := false
      };
    };
    list^;
  };

  let to_list = x => to_rev_list(x) |> List.rev;

  let to_array = x => to_list(x) |> Array.of_list;
};

module Async = {
  type t('a) = Async.t(Signal.t('a));

  let pure = value => {
    let was_sent = ref(false);
    Async.make @@
    (
      cb =>
        was_sent^
          ? cb(Signal.EndOfSignal)
          : {
            cb(Data(value));
            was_sent := true;
          }
    );
  };

  let empty = () => Async.make @@ (cb => cb(Signal.EndOfSignal));

  let from_list = list => {
    let rec go = cb =>
      fun
      | [value, ...rest] => {
          cb(Signal.Data(value));
          go(cb, rest);
        }
      | [] => cb(EndOfSignal);

    Async.make @@ (cb => go(cb, list));
  };

  let map = (f, stream) =>
    Async.make @@
    (cb => stream |> Async.listen(value => cb(Signal.map(f, value))));

  let filter = (f, stream) =>
    Async.make @@
    (
      cb =>
        stream
        |> Async.listen(
             fun
             | Signal.Data(value) when f(value) => cb(Signal.Data(value))
             | Data(_) => ()
             | EndOfSignal => cb(EndOfSignal),
           )
    );

  let scan = (f, init, stream) => {
    let acc = ref(init);
    Async.make @@
    (
      cb =>
        stream
        |> Async.listen(
             fun
             | Signal.Data(value) => {
                 acc := f(acc^, value);
                 cb(Signal.Data(acc^));
               }
             | EndOfSignal => cb(EndOfSignal),
           )
    );
  };

  let take = (n, stream) => {
    let index = ref(0);
    Async.make @@
    (
      cb =>
        stream
        |> Async.listen(
             fun
             | Signal.Data(value) when index^ < n => {
                 index := index^ + 1;
                 cb(Signal.Data(value));
               }
             | _ => cb(EndOfSignal),
           )
    );
  };

  let take' = (~close=?, n, stream) => {
    let index = ref(0);
    Async.make @@
    (
      cb =>
        stream
        |> Async.listen(value =>
             switch (index^ < n, close) {
             | (true, _) =>
               index := index^ + 1;
               cb(Signal.Data(value));
             | (false, Some(close')) =>
               close' |> Sync.send();
               cb(EndOfSignal);
             | (false, None) => cb(EndOfSignal)
             }
           )
    );
  };
};
