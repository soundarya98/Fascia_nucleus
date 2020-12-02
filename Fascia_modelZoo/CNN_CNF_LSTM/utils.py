import h5py
import numpy as np
import random
import os

WINDOW_SIZE = 100
base_path = '/home/fascia/Fascia_nucleus/Fascia_modelZoo/CNN_CNF_LSTM/'

def rescale_array(X):
    X = X / 20
    X = np.clip(X, -5, 5)
    return X


def aug_X(X):
    scale = 1 + np.random.uniform(-0.1, 0.1)
    offset = np.random.uniform(-0.1, 0.1)
    noise = np.random.normal(scale=0.05, size=X.shape)
    X = scale * X + offset + noise
    return X

def gen(dict_files, section, aug=False):
    while True:
        record_name = random.choice(list(dict_files))
        # batch_data = dict_files[record_name]
        all_rows = np.load(record_name)['arr_0']

        for i in range(10):
            start_index = random.choice(range(all_rows.shape[0]-WINDOW_SIZE))

            X = all_rows[start_index:start_index+WINDOW_SIZE, ...]
            Y = np.load(os.path.join(base_path, section, 'y', record_name.split("/")[-1]))['arr_0']
            Y = Y[start_index:start_index+WINDOW_SIZE]

            X = np.expand_dims(X, 0)
            Y = np.expand_dims(Y, -1)
            Y = np.expand_dims(Y, 0)

            if aug:
                X = aug_X(X)
            X = rescale_array(X)

            # print("X shape", X.shape)
            # print("Y shape", Y.shape)

            yield X, Y


def chunker(seq, size=WINDOW_SIZE):
    return (seq[pos:pos + size] for pos in range(0, len(seq), size))