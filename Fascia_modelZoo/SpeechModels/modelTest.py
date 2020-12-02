import numpy as np
from utils import gen, chunker, WINDOW_SIZE, rescale_array
from tensorflow.keras.callbacks import ModelCheckpoint, EarlyStopping, ReduceLROnPlateau
from sklearn.metrics import f1_score, accuracy_score, classification_report
from glob import glob
import os
from sklearn.model_selection import train_test_split
from tqdm import tqdm
import sys

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

x_test = []
y_test = []

for file in test:
    temp = np.load(file)
    x_test.append(np.array(temp["x"]))
    y_test.append(temp["y"])

x_test = np.vstack(x_test)
y_test = np.hstack(y_test)
x_test = np.array(x_test).reshape(-1, 3000, 1)
y_test = np.array(y_test)

from sklearn.preprocessing import LabelEncoder
import numpy as np
from keras.utils import np_utils

label_encoder = LabelEncoder()
classes = list(label_encoder.classes_)

label_encoder = LabelEncoder()
y_test = label_encoder.fit_transform(y_test)
classes = list(label_encoder.classes_)
y_test = np_utils.to_categorical(y_test, num_classes=len(labels))

file_path = "models/cnn_crf_model_" + ch + "_fold" + str(id) + ".ckpt"
model.load_weights(file_path)

for record in tqdm(x_test):
    all_rows = x_test[record]
    record_y_gt = []
    record_y_pred = []
    for batch_hyp in chunker(range(all_rows.shape[0])):


        X = all_rows[min(batch_hyp):max(batch_hyp)+1, ...]
        Y = y_test[record][min(batch_hyp):max(batch_hyp)+1]

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