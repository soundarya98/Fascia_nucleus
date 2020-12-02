from models import get_model_cnn_crf_psg
import numpy as np
from utils import gen, chunker, WINDOW_SIZE, rescale_array
from keras.callbacks import ModelCheckpoint, EarlyStopping, ReduceLROnPlateau
from sklearn.metrics import f1_score, accuracy_score, classification_report
from glob import glob
import os
from sklearn.model_selection import train_test_split
from tqdm import tqdm
import matplotlib.pyplot as plt

import tensorflow as tf
# gpus = tf.config.experimental.list_physical_devices("GPU")
# tf.config.experimental.set_memory_growth(gpus[0], True)

tf.compat.v1.disable_eager_execution()
channels = ['eeg_fpz_cz', 'eeg_pz_oz', 'emg', 'eog', 'resp_oro_nasal', 'temp']

import sys

if(len(sys.argv)!=4):
    print("error")
    exit(1)

base_path = os.path.join(sys.argv[1], sys.argv[2])
print("BASE PATH", base_path)

files = sorted(glob(os.path.join(base_path, "*.npz")))

id = sys.argv[3]

test_ids = {id}

test = [x for x in files if x.split("/")[-1][:5] in test_ids]

test_dict={}
for k in test:
    flag = True
    data = {}
    test_dict[k] = {}
    for ch in channels:
        data[ch] = np.load(os.path.join(sys.argv[1], ch, k.split("/")[-1]))
        test_dict[k]["y"] = data[ch]["y"]
        if(flag==True):
            test_dict[k]["x"] = data[ch]["x"]
            flag = False
        else:
            test_dict[k]["x"] = np.concatenate((test_dict[k]["x"], data[ch]["x"]), axis=2)


model = get_model_cnn_crf_psg(lr=0.0001)

# for ch in channels:
list_f1 = []
list_acc = []
preds = []
gt = []

file_path = "models/test.ckpt"
model.load_weights(file_path)

for record in tqdm(test_dict):
    all_rows = test_dict[record]['x']
    record_y_gt = []
    record_y_pred = []
    for batch_hyp in chunker(range(all_rows.shape[0])):

        X = all_rows[min(batch_hyp):max(batch_hyp)+1, ...]
        Y = test_dict[record]['y'][min(batch_hyp):max(batch_hyp)+1]

        X = np.expand_dims(X, 0)

        X = rescale_array(X)

        Y_pred = model.predict(X)
        Y_pred = Y_pred.argmax(axis=-1).ravel().tolist()

        gt += Y.ravel().tolist()
        preds += Y_pred

        record_y_gt += Y.ravel().tolist()
        record_y_pred += Y_pred

from sklearn.metrics import confusion_matrix
import seaborn as sns
import pandas as pd
def print_confusion_matrix(confusion_matrix, class_names, figsize=(10, 7), fontsize=14):
    """Prints a confusion matrix, as returned by sklearn.metrics.confusion_matrix, as a heatmap.

    Arguments
    ---------
    confusion_matrix: numpy.ndarray
        The numpy.ndarray object returned from a call to sklearn.metrics.confusion_matrix.
        Similarly constructed ndarrays can also be used.
    class_names: list
        An ordered list of class names, in the order they index the given confusion matrix.
    figsize: tuple
        A 2-long tuple, the first value determining the horizontal size of the ouputted figure,
        the second determining the vertical size. Defaults to (10,7).
    fontsize: int
        Font size for axes labels. Defaults to 14.

    Returns
    -------
    matplotlib.figure.Figure
        The resulting confusion matrix figure
    """
    df_cm = pd.DataFrame(
        confusion_matrix, index=class_names, columns=class_names,
    )
    fig = plt.figure(figsize=figsize)
    try:
        heatmap = sns.heatmap(df_cm, annot=True, fmt="d")
    except ValueError:
        raise ValueError("Confusion matrix values must be integers.")
    heatmap.yaxis.set_ticklabels(heatmap.yaxis.get_ticklabels(), rotation=0, ha='right', fontsize=fontsize)
    heatmap.xaxis.set_ticklabels(heatmap.xaxis.get_ticklabels(), rotation=45, ha='right', fontsize=fontsize)
    plt.ylabel('True label')
    plt.xlabel('Predicted label')
    return fig

print("here1")
cm = confusion_matrix(gt, preds)
print_confusion_matrix(cm, class_names=['Wake', 'N1', 'N2', 'N3', 'REM'])
print("here2")

f1 = f1_score(gt, preds, average="macro")

acc = accuracy_score(gt, preds)

print("acc %s, f1 %s"%(acc, f1))



