from models import get_model_cnn_crf, get_model_cnn_crf_psg
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

base_path = os.path.join(sys.argv[1], 'eeg_fpz_cz')
print("BASE PATH", base_path)

files = sorted(glob(os.path.join(base_path, "*.npz")))

id = sys.argv[3]
list_f1 = []
list_acc = []
preds = []
gt = []

# for id in ids
    # print(id)

#old
# test_ids = {id}
# train_ids = set([x.split("/")[-1][:5] for x in files]) - test_ids
#
# train_val, test = [x for x in files if x.split("/")[-1][:5] in train_ids],\
#                   [x for x in files if x.split("/")[-1][:5] in test_ids]
#
# train, val = train_test_split(train_val, test_size=0.1, random_state=1337)
#
#
#
# train_dict = {k: np.load(k) for k in train}
# test_dict = {k: np.load(k) for k in test}
# val_dict = {k: np.load(k) for k in val}

#oldend

#new
test_ids = {id}
train_ids = set([x.split("/")[-1][:5] for x in files]) - test_ids

train_val, test = [x for x in files if x.split("/")[-1][:5] in train_ids],\
                  [x for x in files if x.split("/")[-1][:5] in test_ids]

train, val = train_test_split(train_val, test_size=0.1, random_state=1337)
channels = ['eeg_fpz_cz', 'eeg_pz_oz', 'emg', 'eog', 'resp_oro_nasal', 'temp']

train_dict={}
for k in train:
    flag = True
    data = {}
    train_dict[k] = {}
    for ch in channels:
        data[ch] = np.load(os.path.join(sys.argv[1], ch, k.split("/")[-1]))
        train_dict[k]["y"] = data[ch]["y"]
        if(flag==True):
            train_dict[k]["x"] = data[ch]["x"]
            flag = False
        else:
            train_dict[k]["x"] = np.concatenate((train_dict[k]["x"], data[ch]["x"]), axis=2)

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

val_dict={}
for k in val:
    flag = True
    data = {}
    val_dict[k] = {}
    for ch in channels:
        data[ch] = np.load(os.path.join(sys.argv[1], ch, k.split("/")[-1]))
        val_dict[k]["y"] = data[ch]["y"]
        if(flag==True):
            val_dict[k]["x"] = data[ch]["x"]
            flag = False
        else:
            val_dict[k]["x"] = np.concatenate((val_dict[k]["x"], data[ch]["x"]), axis=2)


model = get_model_cnn_crf_psg(lr=0.0001)
#
file_path = "models/cnn_crf_model_psg_fold"+str(id)+".ckpt"

checkpoint = ModelCheckpoint(file_path, monitor='val_crf_viterbi_accuracy', verbose=1, save_best_only=True, mode='max')
early = EarlyStopping(monitor="val_crf_viterbi_accuracy", mode="max", patience=20, verbose=1)
redonplat = ReduceLROnPlateau(monitor="val_crf_viterbi_accuracy", mode="max", patience=5, verbose=2)
callbacks_list = [checkpoint, redonplat]  # early

model.fit_generator(gen(train_dict, aug=False), validation_data=gen(val_dict), epochs=40, verbose=2,
                    steps_per_epoch=1000, validation_steps=300, callbacks=callbacks_list)

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


f1 = f1_score(gt, preds, average="macro")

acc = accuracy_score(gt, preds)

print("acc %s, f1 %s"%(acc, f1))
