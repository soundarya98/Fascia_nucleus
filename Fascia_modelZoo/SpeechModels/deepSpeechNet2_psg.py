labels = ['Wake', 'N1', 'N2', 'N3', 'REM']
import models
from tensorflow.keras.callbacks import EarlyStopping, ModelCheckpoint

print("here")
model = models.get_DeepSpeech_model()
model.summary()

model.compile(loss='categorical_crossentropy', optimizer='nadam', metrics=['accuracy'])
early_stop = EarlyStopping(monitor='val_loss', mode='min',
                           verbose=1, patience=10, min_delta=0.0001)

checkpoint = ModelCheckpoint('speech2text_model_PSG_DeepSpeech.ckpt', monitor='val_acc',
                             verbose=1, save_best_only=True, mode='max')

import numpy as np
from tensorflow.keras.callbacks import ModelCheckpoint, EarlyStopping, ReduceLROnPlateau
from sklearn.metrics import f1_score, accuracy_score, classification_report
from glob import glob
import os
from sklearn.model_selection import train_test_split
from tqdm import tqdm

import tensorflow as tf

# tf.compat.v1.disable_eager_execution()


import sys

if(len(sys.argv)!=4):
    print("error")
    exit(1)

base_path = os.path.join(sys.argv[1], sys.argv[2])
print("BASE PATH", base_path)

files = sorted(glob(os.path.join(base_path, "*.npz")))

id = sys.argv[3]
list_f1 = []
list_acc = []
preds = []
gt = []


test_ids = {id}
train_ids = set([x.split("/")[-1][:5] for x in files]) - test_ids

train_val, test = [x for x in files if x.split("/")[-1][:5] in train_ids],\
                  [x for x in files if x.split("/")[-1][:5] in test_ids]

train, val = train_test_split(train_val, test_size=0.1, random_state=1337)
channels = ['eeg_fpz_cz', 'eeg_pz_oz', 'emg', 'eog', 'resp_oro_nasal', 'temp']

x_train = []
y_train = []

x_val = []
y_val = []

x_test = []
y_test = []

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

# print(train_dict[train[0]]["x"].shape) #1002 3000 6

for file in train_dict.keys():
    x_train.append(np.array(train_dict[file]["x"]))
    y_train.append(train_dict[file]["y"])

x_train = np.vstack(x_train)
y_train = np.hstack(y_train)
x_train = np.array(x_train)
y_train = np.array(y_train)

# print(x_train.shape)

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

# print(test_dict[test[0]]["x"].shape) #1002 3000 6

for file in test_dict.keys():
    x_test.append(np.array(test_dict[file]["x"]))
    y_test.append(test_dict[file]["y"])

x_test = np.vstack(x_test)
y_test = np.hstack(y_test)
x_test = np.array(x_test)
y_test = np.array(y_test)

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

# print(val_dict[val[0]]["x"].shape) #1002 3000 6

for file in val_dict.keys():
    x_val.append(np.array(val_dict[file]["x"]))
    y_val.append(val_dict[file]["y"])

x_val = np.vstack(x_val)
y_val = np.hstack(y_val)
x_val = np.array(x_val)
y_val = np.array(y_val)

from sklearn.preprocessing import LabelEncoder
import numpy as np
from keras.utils import np_utils

label_encoder = LabelEncoder()
y_train = label_encoder.fit_transform(y_train)
classes = list(label_encoder.classes_)
y_train = np_utils.to_categorical(y_train, num_classes=len(labels))

label_encoder = LabelEncoder()
y_val = label_encoder.fit_transform(y_val)
classes = list(label_encoder.classes_)
y_val = np_utils.to_categorical(y_val, num_classes=len(labels))

label_encoder = LabelEncoder()
y_test = label_encoder.fit_transform(y_test)
classes = list(label_encoder.classes_)
y_test = np_utils.to_categorical(y_test, num_classes=len(labels))

hist = model.fit(
    x=x_train,
    y=y_train,
    epochs=100,
    callbacks=[early_stop, checkpoint],
    batch_size=64,
    validation_data=(x_val,y_val)
)
model.save("my_model_PSG_DeepSpeech")
model.save('model_speech2text_PSG_DeepSpeech.h5')
#
# # serialize model to JSON
# import simplejson
# model_json = model.to_json()
# with open("speech2text_model.json", "w") as json_file:
#     json_file.write(simplejson.dumps(simplejson.loads(model_json), indent=4))
#
# # serialize weights to HDF5
# model.save_weights("speech2text_model.h5")
# print("Saved model to disk")