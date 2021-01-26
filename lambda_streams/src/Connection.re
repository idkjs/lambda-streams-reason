/**
 Represents a generic connection-based stream. This is defined as a record which contains the
 input stream ({!stream}) as the source, as well as an output stream ({!close}) that is triggered
 to close resources and end the stream.
 This is useful for streams that have a notion of a connection, such as file descriptors.
 */

type t('input_stream, 'output_stream) = {
  stream: 'input_stream,
  close: 'output_stream,
};
