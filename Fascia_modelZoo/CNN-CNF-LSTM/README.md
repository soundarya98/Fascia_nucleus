Command to train:

Run:  
`CUDA_VISIBLE_DEVICES=$gpu python cnn_crf_model_20_folds.py /home/fascia/Fascia_nucleus/Fascia_modelZoo/data/ $channel $file`

Or
`CUDA_VISIBLE_DEVICES=$gpu python cnn_crf_model_20_folds.py /home/fascia/Fascia_nucleus/Fascia_modelZoo/data/ eeg_fpz_cz SC401`


Testing:

`CUDA_VISIBLE_DEVICES=0 python testTrainedModels.py /home/fascia/Fascia_nucleus/Fascia_modelZoo/data/ eeg_fpz_cz SC401`

PSG:
CUDA_VISIBLE_DEVICES=0 python cnn_crf_model_psg_20_folds.py /home/fascia/Fascia_nucleus/Fascia_modelZoo/data/ eeg_fpz_cz SC401

Transformers:
CUDA_VISIBLE_DEVICES=0 python transformers.py /home/fascia/Fascia_nucleus/Fascia_modelZoo/data/ eeg_fpz_cz SC401