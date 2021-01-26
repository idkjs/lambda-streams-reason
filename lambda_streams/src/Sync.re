type input('a) = unit => 'a

and output('a) = 'a => unit;

type connection('a) = Connection.t(input('a), output(unit));

let make_input = f => f;

let make_output = f => f;

let make_mutator = (~initial) => {
  let value = ref(initial);
  let input = () => value^
  and output = v => value := v;
  (input, output);
};

let pure = (value, ()) => value;

let enumerate = () => {
  let index = ref(0);
  () => {
    index := index^ + 1;
    index^;
  };
};

let next = stream => stream();

let send = (value, output_stream) => output_stream(value);

let pipe = (output_stream, input_stream) =>
  while (true) {
    input_stream() |> output_stream;
  };

let accumulate = (n, f, init, stream) => {
  let index = ref(0)
  and acc = ref(init);
  while (index^ < n) {
    index := index^ + 1;
    acc := f(acc^, stream());
  };
  acc^;
};

let map = (f, stream, ()) => stream() |> f;

let filter = (f, stream, ()) => {
  let value = ref @@ stream();
  while ((!) @@ f(value^)) {
    value := stream();
  };
  value^;
};

let scan = (f, init, stream) => {
  let acc = ref(init);
  () => {
    acc := f(acc^, stream());
    acc^;
  };
};
