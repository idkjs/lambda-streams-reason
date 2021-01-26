/** Gets the first value of a lambda stream and turns it into a deferred value */
let first_to_async:
  Lambda_streams.Finite.Async.t('a) => Async_kernel.Deferred.t('a);

/** Gets the last value of a lambda stream and turns it into a deferred value */

let last_to_async:
  Lambda_streams.Finite.Async.t('a) => Async_kernel.Deferred.t('a);

let to_async_list:
  Lambda_streams.Finite.Async.t('a) => Async_kernel.Deferred.t(list('a));

let from_async:
  Async_kernel.Deferred.t('a) => Lambda_streams.Finite.Async.t('a);

let to_async_stream:
  Lambda_streams.Finite.Async.t('a) => Async_kernel.Stream.t('a);

let from_async_stream:
  Async_kernel.Stream.t('a) => Lambda_streams.Finite.Async.t('a);

let to_reader:
  Lambda_streams.Finite.Async.t('a) => Async_kernel.Pipe.Reader.t('a);

let from_reader:
  Async_kernel.Pipe.Reader.t('a) => Lambda_streams.Finite.Async.t('a);
