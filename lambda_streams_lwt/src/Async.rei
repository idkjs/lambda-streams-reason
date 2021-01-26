open Lambda_streams;

/** Gets the first value from a lambda stream and turns it into a lwt promise */

let first_to_lwt: Finite.Async.t('a) => Lwt.t('a);

/** Gets the last value from a lambda stream and turns it into a lwt promise */

let last_to_lwt: Finite.Async.t('a) => Lwt.t('a);

let to_lwt_list: Finite.Async.t('a) => Lwt.t(list('a));

let from_lwt: Lwt.t('a) => Finite.Async.t('a);

let to_lwt_stream: Finite.Async.t('a) => Lwt_stream.t('a);

let from_lwt_stream: Lwt_stream.t('a) => Finite.Async.t('a);
