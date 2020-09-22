from models import get_model_cnn_crf
import numpy as np
from utils import gen, chunker, WINDOW_SIZE, rescale_array
from keras.callbacks import ModelCheckpoint, EarlyStopping, ReduceLROnPlateau
from sklearn.metrics import f1_score, accuracy_score, classification_report
from glob import glob
import os
from sklearn.model_selection import train_test_split
from tqdm import tqdm
# import matplotlib.pyplot as plt

import tensorflow as tf
# gpus = tf.config.experimental.list_physical_devices("GPU")
# tf.config.experimental.set_memory_growth(gpus[0], True)

tf.compat.v1.disable_eager_execution()


import sys

if(len(sys.argv)!=4):
    print("error")
    exit(1)

base_path = os.path.join(sys.argv[1], sys.argv[2])
print("BASE PATH", base_path)

files = sorted(glob(os.path.join(base_path, "*.npz")))

id = sys.argv[3]
# for id in ids
    # print(id)
test_ids = {id}

test = [x for x in files if x.split("/")[-1][:5] in test_ids]

test_dict = {k: np.load(k) for k in test}

model = get_model_cnn_crf(lr=0.0001)

channels = ['eeg_fpz_cz', 'eeg_pz_oz', 'emg', 'eog', 'resp_oro_nasal', 'temp']
overall_gt = {}
overall_preds = {}

for ch in channels:
    list_f1 = []
    list_acc = []
    preds = []
    gt = []

    file_path = "models/cnn_crf_model_" +ch+"_fold"+str(id)+".ckpt"
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
            # print(model.predict_proba(X))
            Y_pred = Y_pred.argmax(axis=-1).ravel().tolist()

            gt += Y.ravel().tolist()
            preds += Y_pred

            record_y_gt += Y.ravel().tolist()
            record_y_pred += Y_pred

    print(ch, "gtshape", len(gt))
    print(ch, "predsshape", len(preds))

    overall_gt[ch] = gt
    overall_preds[ch] = preds

    f1 = f1_score(gt, preds, average="macro")

    acc = accuracy_score(gt, preds)

    print(ch, "acc %s, f1 %s"%(acc, f1))

# for ch1 in channels:
#     for ch2 in channels:
#         print("overall gt: ", ch1, ch2, np.array_equal(overall_gt[ch1], overall_gt[ch2]))
mean_overall_preds = np.zeros(len(overall_preds[channels[0]]))
print(len(overall_preds[channels[0]]))
for i in range(len(overall_preds[channels[0]])):
    for ch in channels:
        mean_overall_preds[i]+=overall_preds[ch][i]

mean_overall_preds/=len(channels)

overall_f1 = f1_score(overall_gt[channels[0]], np.around(mean_overall_preds), average="macro")

overall_acc = accuracy_score(overall_gt[channels[0]], np.around(mean_overall_preds))

print("Overall Accuracy", "acc %s, f1 %s"%(overall_acc, overall_f1))

