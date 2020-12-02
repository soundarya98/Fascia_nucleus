from models import get_model_cnn_crf_psg
import numpy as np
from utils import gen, chunker, WINDOW_SIZE, rescale_array
from sklearn.metrics import f1_score, accuracy_score, classification_report
from glob import glob
import os
from tqdm import tqdm
import matplotlib.pyplot as plt

import tensorflow as tf
tf.compat.v1.disable_eager_execution()
import sys

test = [sys.argv[1]]

print("sys argv1", sys.argv[1])

test_dict={}
for k in test:
    print("k is", k)
    flag = True
    data = {}
    test_dict[k] = {}
    data = np.load(k)
    test_dict[k]["y"] = data["y"]
    test_dict[k]["x"] = data["x"]

model = get_model_cnn_crf_psg(lr=0.0001)

list_f1 = []
list_acc = []
preds = []
gt = []

file_path = "models/cnn_crf_model_psg"+'extended_fold_0_psg'+".ckpt"
model.load_weights(file_path)

indices = np.r_[0:6]

for record in tqdm(test_dict):
    X = np.transpose(np.transpose(test_dict[record]['x'])[indices][:][:])
    X = np.expand_dims(X, 0)
    X = np.expand_dims(X, 0)
    X = rescale_array(X)
    Y = test_dict[record]['y']
    record_y_gt = []
    record_y_pred = []
    # for batch_hyp in chunker(range(all_rows.shape[0])):
    #
    #
    #     X = all_rows[min(batch_hyp):max(batch_hyp)+1, ...]
    #     Y = test_dict[record]['y'][min(batch_hyp):max(batch_hyp)+1]
    #
    #     X = np.expand_dims(X, 0)
    #
    #     X = rescale_array(X)

    Y_pred = model.predict(X)
    Y_pred = Y_pred.argmax(axis=-1).ravel().tolist()

    gt += Y.ravel().tolist()
    preds += Y_pred

    record_y_gt += Y.ravel().tolist()
    record_y_pred += Y_pred

from sklearn.metrics import confusion_matrix
cm = confusion_matrix(gt, preds)
print(cm)

f1 = f1_score(gt, preds, average="macro")

acc = accuracy_score(gt, preds)

print("acc %s, f1 %s"%(acc, f1))