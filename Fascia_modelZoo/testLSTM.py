from models import get_model_lstm
import numpy as np
from utils import gen, chunker, WINDOW_SIZE, rescale_array
from keras.callbacks import ModelCheckpoint, EarlyStopping, ReduceLROnPlateau
from sklearn.metrics import f1_score, accuracy_score, classification_report
from glob import glob
import os
from sklearn.model_selection import train_test_split
from tqdm import tqdm
import tensorflow

strategy = tensorflow.distribute.MirroredStrategy()
print('Number of devices: {}'.format(strategy.num_replicas_in_sync))

# Open a strategy scope.
with strategy.scope():
    base_path = "/home/fascia/Fascia_nucleus/Fascia_modelZoo/data/eeg_fpz_cz"

    files = sorted(glob(os.path.join(base_path, "*.npz")))

    ids = sorted(list(set([x.split("/")[-1][:5] for x in files])))
    #split by test subject
    train_ids, test_ids = train_test_split(ids, test_size=0.15, random_state=1338)

    train_val, test = [x for x in files if x.split("/")[-1][:5] in train_ids],\
                      [x for x in files if x.split("/")[-1][:5] in test_ids]

    train, val = train_test_split(train_val, test_size=0.1, random_state=1337)

    train_dict = {k: np.load(k) for k in train}
    test_dict = {k: np.load(k) for k in test}
    val_dict = {k: np.load(k) for k in val}

    file_path = "lstm_model.h5"
    model = get_model_lstm()
model.load_weights(file_path)


preds = []
gt = []

for record in tqdm(test_dict):
    all_rows = test_dict[record]['x']
    for batch_hyp in chunker(range(all_rows.shape[0])):


        X = all_rows[min(batch_hyp):max(batch_hyp)+1, ...]
        Y = test_dict[record]['y'][min(batch_hyp):max(batch_hyp)+1]

        X = np.expand_dims(X, 0)

        X = rescale_array(X)

        Y_pred = model.predict(X)
        Y_pred = Y_pred.argmax(axis=-1).ravel().tolist()

        gt += Y.ravel().tolist()
        preds += Y_pred



f1 = f1_score(gt, preds, average="macro")

print("Seq Test f1 score : %s "% f1)

acc = accuracy_score(gt, preds)

print("Seq Test accuracy score : %s "% acc)

print(classification_report(gt, preds))