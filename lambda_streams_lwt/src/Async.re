open Lambda_streams;

let (>>=) = Lwt.(>>=);

let to_lwt_stream = stream => {
  let (lwt_stream, push) = Lwt_stream.create();
  stream |> Async.listen(value => value |> Signal.to_option |> push);
  lwt_stream;
};

let first_to_lwt = stream => {
  let mvar = Lwt_mvar.create_empty()
  and fulfilled = ref(false);
  stream
  |> Async.listen(value =>
       switch (value, fulfilled^) {
       | (Signal.Data(value'), false) =>
         Lwt_mvar.put(mvar, value')
         >>= (
           () => {
             fulfilled := true;
             Lwt.return();
           }
         )
         |> ignore
       | _ => ()
       }
     );
  Lwt_mvar.take(mvar);
};

let last_to_lwt = stream => stream |> to_lwt_stream |> Lwt_stream.last_new;

let to_lwt_list = stream => stream |> to_lwt_stream |> Lwt_stream.to_list;

let from_lwt = promise =>
  Async.make @@
  (
    cb =>
      Lwt.on_success(
        promise,
        value => {
          cb @@ Signal.Data(value);
          cb(EndOfSignal);
        },
      )
  );

let from_lwt_stream = lwt_stream => {
  let rec iterate = (cb, s) =>
    try(
      Lwt_stream.next(s)
      >>= (
        value => {
          cb @@ Signal.Data(value);
          iterate(cb, s);
        }
      )
    ) {
    | Lwt_stream.Empty
    | Lwt_stream.Closed =>
      cb(EndOfSignal);
      Lwt.return();
    };

  Async.make @@ (cb => lwt_stream |> iterate(cb) |> ignore);
};
