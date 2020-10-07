import numpy as np
def _load_npz_file_single_epoch(channel, npz_file):
    """Load data and labels from a npz file."""
    with np.load(npz_file) as f:
        # print(f)
        data = f[channel]
        labels = f["labels"]
    return data, labels

data, labels = _load_npz_file_single_epoch('emg', '/home/fascia/Fascia_nucleus/Fascia_modelZoo/data/30sec/emg/SC4001E0.npz')
print(data.dtype)