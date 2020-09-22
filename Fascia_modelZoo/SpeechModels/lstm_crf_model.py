labels = ['Wake', 'N1', 'N2', 'N3', 'REM']
import models
from keras_contrib.layers import CRF

from tensorflow.keras.callbacks import EarlyStopping, ModelCheckpoint

print("here")
model = models.get_crf_model()
model.summary()

model.compile(loss='categorical_crossentropy', optimizer='nadam', metrics=['accuracy'])
early_stop = EarlyStopping(monitor='val_loss', mode='min',
                           verbose=1, patience=10, min_delta=0.0001)

checkpoint = ModelCheckpoint('speech2text_model_lstm_crf.ckpt', monitor='val_accuracy',
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

# for id in ids
    # print(id)
test_ids = {id}
train_ids = set([x.split("/")[-1][:5] for x in files]) - test_ids

train_val, test = [x for x in files if x.split("/")[-1][:5] in train_ids],\
                  [x for x in files if x.split("/")[-1][:5] in test_ids]

train, val = train_test_split(train_val, test_size=0.1, random_state=1337)

# train_dict = {k: np.load(k) for k in train}
# test_dict = {k: np.load(k) for k in test}
# val_dict = {k: np.load(k) for k in val}
x_train = []
y_train = []

x_val = []
y_val = []

x_test = []
y_test = []

for file in train:
    temp = np.load(file)
    x_train.append(np.array(temp["x"]))
    y_train.append(temp["y"])

x_train = np.vstack(x_train)
y_train = np.hstack(y_train)
x_train = np.array(x_train).reshape(-1,3000,1)
y_train = np.array(y_train)

for file in val:
    temp = np.load(file)
    x_val.append(np.array(temp["x"]))
    y_val.append(temp["y"])

x_val = np.vstack(x_val)
y_val = np.hstack(y_val)
x_val = np.array(x_val).reshape(-1,3000,1)
y_val = np.array(y_val)

for file in test:
    temp = np.load(file)
    x_test.append(np.array(temp["x"]))
    y_test.append(temp["y"])

x_test = np.vstack(x_test)
y_test = np.hstack(y_test)
x_test = np.array(x_test).reshape(-1,3000,1)
y_test = np.array(y_test)


from sklearn.preprocessing import LabelEncoder
import numpy as np
from keras.utils import np_utils

# label_encoder = LabelEncoder()
# y_train = label_encoder.fit_transform(y_train)
# classes = list(label_encoder.classes_)
# y_train = np_utils.to_categorical(y_train, num_classes=len(labels))
#
# label_encoder = LabelEncoder()
# y_val = label_encoder.fit_transform(y_val)
# classes = list(label_encoder.classes_)
# y_val = np_utils.to_categorical(y_val, num_classes=len(labels))
#
# label_encoder = LabelEncoder()
# y_test = label_encoder.fit_transform(y_test)
# classes = list(label_encoder.classes_)
# y_test = np_utils.to_categorical(y_test, num_classes=len(labels))
# y_train = np.expand_dims(y_train,2)
hist = model.fit(
    x=np.expand_dims(x_train, axis = 0),
    y=np.expand_dims(y_train, axis = 0),
    epochs=100,
    callbacks=[early_stop, checkpoint],
    batch_size=32,
    validation_data=(np.expand_dims(x_train, axis = 0),np.expand_dims(y_val, axis = 0))
)
model.save("my_model_LSTM_CRF")
model.save('model_speech2text_LSTM_CRF.h5')

# # serialize model to JSON
# import simplejson
# model_json = model.to_json()
# with open("speech2text_model.json", "w") as json_file:
#     json_file.write(simplejson.dumps(simplejson.loads(model_json), indent=4))
#
# # serialize weights to HDF5
# model.save_weights("speech2text_model.h5")
# print("Saved model to disk")