Command to train:


Testing:

`CUDA_VISIBLE_DEVICES=0 python predict.py /home/fascia/Fascia_nucleus/Fascia_modelZoo/data/ eeg_fpz_cz SC401`

Testing Single Epoch:
`CUDA_VISIBLE_DEVICES=0 python predict_single_epoch.py /home/fascia/Fascia_nucleus/Fascia_modelZoo/SleepEDF_NPZ/30sec/epoch1.npz`


Training with SleepEDF Extended
CUDA_VISIBLE_DEVICES=0 python3.7 cnn_crf_model_psg_20_folds.py /home/fascia/Fascia_nucleus/Fascia_modelZoo/SleepEDF_NPZ/ 

Transformers:
CUDA_VISIBLE_DEVICES=0 python3.7 transformers.py /home/fascia/Fascia_nucleus/Fascia_modelZoo/data/ eeg_fpz_cz SC401

Preprocess:
python preprocess.py --output_dir /home/fascia/Fascia_nucleus/Fascia_modelZoo/SleepEDF_NPZ/