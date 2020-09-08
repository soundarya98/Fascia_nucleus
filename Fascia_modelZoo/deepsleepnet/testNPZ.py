import numpy as np
import tensorflow as tf

FLAGS = tf.app.flags.FLAGS

tf.app.flags.DEFINE_string('file', '/Fascia_nucleus/Fascia_modelZoo/data/eeg_fpz_cz/',
                           """Directory where to load training data.""")

def _load_npz_file(self, npz_file):
    """Load data and labels from a npz file."""
    with np.load(npz_file) as f:
        data = f["x"]
        labels = f["y"]
        sampling_rate = f["fs"]
    return data, labels, sampling_rate


def main(argv=None):

    npz_f = FLAGS.file
    print("Loading {} ...".format(npz_f))
    data, labels, sampling_rate = self._load_npz_file(npz_f)
    fs = None

    if fs is None:
        fs = sampling_rate
    elif fs != sampling_rate:
        raise Exception("Found mismatch in sampling rate.")
    data = np.vstack(data)
    labels = np.hstack(labels)
    print(data, labels)
