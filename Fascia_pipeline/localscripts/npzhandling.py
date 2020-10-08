import numpy as np
import tensorflow as tf
import os
FLAGS = tf.app.flags.FLAGS
tf.app.flags.DEFINE_string('file', 'SC4001E0.npz', """File to load.""")

def _load_npz_file_single_epoch(channel, npz_file):
    """Load data and labels from a npz file."""
    with np.load(npz_file) as f:
        data = f[channel]
        labels = f["labels"]
    return data, labels

def _load_npz_file(npz_file):
    """Load data and labels from a npz file."""
    with np.load(npz_file) as f:
        data = f["x"]
        labels = f["y"]
        sampling_rate = f["fs"]
    return data, labels, sampling_rate

def main(argv=None):
    for channel in ['eeg_fpz_cz', 'eeg_pz_oz', 'emg', 'eog', 'resp_oro_nasal', 'temp']:
        npz_f = os.path.join(FLAGS.data_dir, channel, FLAGS.file)
        print("Loading {} ...".format(npz_f))
        data, labels, sampling_rate = _load_npz_file(npz_f)
        data = np.vstack(data)
        labels = np.hstack(labels)
        print(data)
        print(data.shape, labels.shape)
        save_dict = {
            channel: data[:3000],
            "labels": labels[0]
        }
        save_path = os.path.join(FLAGS.data_dir, '30sec'+channel+FLAGS.file)
        np.savez(save_path, **save_dict)

def iterate_batch_seq_minibatches_single_epoch(inputs, targets, batch_size, seq_length):
    assert len(inputs) == len(targets)
    n_inputs = len(inputs)
    batch_len = n_inputs // batch_size
    epoch_size = batch_len // seq_length
    if epoch_size == 0:
        raise ValueError("epoch_size == 0, decrease batch_size or seq_length")
    seq_inputs = np.zeros((batch_size, batch_len) + inputs.shape[1:],
                          dtype=inputs.dtype)
    seq_targets = np.zeros((batch_size, batch_len) + targets.shape[1:],
                           dtype=targets.dtype)
    for i in range(batch_size):
        seq_inputs[i] = inputs[i*batch_len:(i+1)*batch_len]
        seq_targets[i] = targets[i*batch_len:(i+1)*batch_len]
    for i in range(epoch_size):
        x = seq_inputs[:, i*seq_length:(i+1)*seq_length]
        y = seq_targets[:, i*seq_length:(i+1)*seq_length]
        flatten_x = x.reshape((-1,) + inputs.shape[1:])
        flatten_y = y.reshape((-1,) + targets.shape[1:])
        yield flatten_x, flatten_y

def load_single_epoch(channel, npz_f):
    print("Loading {} ...".format(npz_f))
    data, labels = _load_npz_file_single_epoch(channel, npz_f)
    return data, labels
    
if __name__ == "__main__":
    main()