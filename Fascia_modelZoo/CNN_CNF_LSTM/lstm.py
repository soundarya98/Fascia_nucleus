from models import get_model_cnn_crf, get_model_cnn_crf_psg, get_model_spindlesslowwaves, get_model_lstm
import numpy as np
from utils import gen, chunker, WINDOW_SIZE, rescale_array
from keras.callbacks import ModelCheckpoint, EarlyStopping, ReduceLROnPlateau
from sklearn.metrics import f1_score, accuracy_score, classification_report
from glob import glob
import os
import tensorflow as tf
sf = 100
fpzcz_index = np.r_[0]
pzoz_index = np.r_[1]
tf.compat.v1.disable_eager_execution()

# base_path = '/home/fascia/Fascia_nucleus/Fascia_modelZoo/SleepEDF_NPZ/'
base_path = '/home/fascia/Fascia_nucleus/Fascia_modelZoo/CNN_CNF_LSTM/'
# files = sorted(glob(os.path.join(base_path, "*.npz")))
train_files = sorted(glob(os.path.join(base_path, 'train', "*.npz")))
val_files = sorted(glob(os.path.join(base_path, 'val', "*.npz")))
test_files = sorted(glob(os.path.join(base_path, 'test', "*.npz")))

# id = ['SC409', 'SC419', 'SC428', 'SC440', 'SC449', 'SC459', 'SC471', 'SC482']
list_f1 = []
list_acc = []
preds = []
gt = []
indices = np.r_[0:6]

print('here!')
model = get_model_lstm()


file_path = "models/lstm.ckpt"

checkpoint = ModelCheckpoint(file_path, monitor='val_acc', verbose=1, save_best_only=True, mode='max')
early = EarlyStopping(monitor="val_acc", mode="max", patience=100, verbose=1)
redonplat = ReduceLROnPlateau(monitor="val_acc", mode="max", patience=5, verbose=2)
callbacks_list = [checkpoint, early, redonplat]  # early

model.fit_generator(gen(train_files, 'train', aug=False), validation_data=gen(val_files, 'val'), epochs=45, verbose=2,
                    steps_per_epoch=1000, validation_steps=300, callbacks=callbacks_list)

print('DONE')
# model.load_weights(file_path)
#
# for record in tqdm(test):
#     all_rows = test_dict[record]['x']
#     record_y_gt = []
#     record_y_pred = []
#     for batch_hyp in chunker(range(all_rows.shape[0])):
#
#         X = all_rows[min(batch_hyp):max(batch_hyp)+1, ...]
#         Y = test_dict[record]['y'][min(batch_hyp):max(batch_hyp)+1]
#
#         X = np.expand_dims(X, 0)
#
#         X = rescale_array(X)
#
#         Y_pred = model.predict(X)
#         Y_pred = Y_pred.argmax(axis=-1).ravel().tolist()
#
#         gt += Y.ravel().tolist()
#         preds += Y_pred
#
#         record_y_gt += Y.ravel().tolist()
#         record_y_pred += Y_pred
#
#
# f1 = f1_score(gt, preds, average="macro")
#
# acc = accuracy_score(gt, preds)
#
# print("acc %s, f1 %s"%(acc, f1))
