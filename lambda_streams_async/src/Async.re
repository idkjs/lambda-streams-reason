open Lambda_streams;

let (>>=) = Async_kernel.(>>=);

let first_to_async = stream => {
  let deferred = Async_kernel.Ivar.create();
  stream
  |> Async.listen(
       fun
       | Signal.Data(value) =>
         Async_kernel.Ivar.fill_if_empty(deferred, value)
       | _ => (),
     );
  Async_kernel.Ivar.read(deferred);
};

let last_to_async = stream => {
  let deferred = Async_kernel.Ivar.create()
  and last_value = ref(None);
  stream
  |> Async.listen(e =>
       switch (e, last_value^) {
       | (Signal.Data(value), _) => last_value := Some(value)
       | (EndOfSignal, Some(value)) =>
         Async_kernel.Ivar.fill_if_empty(deferred, value)
       | _ => ()
       }
     );
  Async_kernel.Ivar.read(deferred);
};

let to_async_list = stream => {
  let deferred = Async_kernel.Ivar.create()
  and list = ref([]);
  stream
  |> Async.listen(
       fun
       | Signal.Data(value) => list := [value, ...list^]
       | EndOfSignal =>
         Async_kernel.Ivar.fill_if_empty(deferred, List.rev(list^)),
     );
  Async_kernel.Ivar.read(deferred);
};

let from_async = async =>
  Async.make @@
  (
    cb =>
      async
      >>= (
        value => {
          cb @@ Signal.Data(value);
          cb(EndOfSignal);
          Async_kernel.return();
        }
      )
      |> ignore
  );

let to_async_stream = stream =>
  Async_kernel.Stream.create(tail =>
    stream
    |> Async.listen(
         fun
         | Signal.Data(value) => Async_kernel.Tail.extend(tail, value)
         | EndOfSignal => Async_kernel.Tail.close_exn(tail),
       )
  );

let from_async_stream = async_stream => {
  let is_closed = Async_kernel.Stream.closed(async_stream);
  Async.make @@
  (
    cb =>
      Async_kernel.Deferred.all_unit([
        async_stream
        |> Async_kernel.Stream.iter'(~f=value =>
             cb @@ Signal.Data(value) |> Async_kernel.return
           ),
        is_closed >>= (() => cb(EndOfSignal) |> Async_kernel.return),
      ])
      |> ignore
  );
};

let to_reader = stream =>
  Async_kernel.Pipe.create_reader(~close_on_exception=true, writer =>
    stream
    |> Async.listen(
         fun
         | Signal.Data(value) =>
           Async_kernel.Pipe.write(writer, value) |> ignore
         | EndOfSignal => Async_kernel.Pipe.close(writer),
       )
    |> Async_kernel.return
  );

let from_reader = reader => {
  let (>>=) = Async_kernel.(>>=);
  Async.make @@
  (
    cb =>
      reader
      |> Async_kernel.Pipe.iter(~f=value =>
           cb @@ Signal.Data(value) |> Async_kernel.return
         )
      >>= (() => cb(EndOfSignal) |> Async_kernel.return)
      |> ignore
  );
};
