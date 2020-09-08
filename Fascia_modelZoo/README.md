This repository contains 2 types of files:
1. deepsleepnet replication from https://github.com/akaraspt/deepsleepnet
2. Naive LSTM from https://github.com/CVxTz/EEG_classification/

The dataset that is common to both is Sleep-EDF dataset.   
In order to download this dataset to your computer, use

`cd data`  
`chmod +x download_physionet.sh`  
`./download_physionet.sh`

To train a deepsleepnet model for a particular channel, where channel in [eeg_fpz_cz, eeg_pz_oz, eog, emg, resp-oro-nasal, temp]:
1. Navigate to the deepsleepnet directory
2. Run:
`CUDA_VISIBLE_DEVICES=$gpu python train.py --data_dir ~/Fascia_nucleus/Fascia_modelZoo/data/$channel --output_dir output/$channel --n_folds 20 --fold_idx 0 --pretrain_epochs 100 --finetune_epochs 200 --resume False`


