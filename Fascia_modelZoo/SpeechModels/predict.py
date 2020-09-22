from models import get_DeepSpeech_model
import numpy as np
from utils import gen, chunker, WINDOW_SIZE, rescale_array
from tensorflow.keras.callbacks import ModelCheckpoint, EarlyStopping, ReduceLROnPlateau
from sklearn.metrics import f1_score, accuracy_score, classification_report
from glob import glob
import os
from sklearn.model_selection import train_test_split
from tqdm import tqdm
import sys
from keras_contrib.layers import CRF

if (len(sys.argv) != 4):
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

labels = ['Wake', 'N1', 'N2', 'N3', 'REM']

test_ids = {id}

test = [x for x in files if x.split("/")[-1][:5] in test_ids]
channels = ['eeg_fpz_cz', 'eeg_pz_oz', 'emg', 'eog', 'resp_oro_nasal', 'temp']

x_test = []
y_test = []
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

from sklearn.preprocessing import LabelEncoder
import numpy as np
from keras.utils import np_utils

label_encoder = LabelEncoder()
y_test = label_encoder.fit_transform(y_test)
classes = list(label_encoder.classes_)
y_test = np_utils.to_categorical(y_test, num_classes=len(labels))

import keras
model = get_DeepSpeech_model()
model.load_weights("speech2text_model_PSG_DeepSpeech.ckpt")

# # load json and create model
# from keras.models import model_from_json
# json_file = open('speech2text_model.json', 'r')
# loaded_model_json = json_file.read()
# json_file.close()
# loaded_model = model_from_json(loaded_model_json)
#
# # load weights into new model
# loaded_model.load_weights("speech2text_model.h5")
# print("Loaded model from disk")
print(x_test.shape)

y_pred = model.predict(x_test)


print(np.argmax(y_pred, axis = 1))

f1 = f1_score(np.argmax(y_test, axis = 1), np.argmax(y_pred, axis = 1), average="macro")

acc = accuracy_score(np.argmax(y_test, axis = 1), np.argmax(y_pred, axis = 1))

print("acc %s, f1 %s"%(acc, f1))