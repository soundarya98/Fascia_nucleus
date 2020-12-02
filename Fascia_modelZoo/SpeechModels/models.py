def get_LSTM_model():
    from tensorflow.keras.layers import Bidirectional, BatchNormalization, TimeDistributed, GlobalMaxPool1D
    from tensorflow.keras.layers import Dense, Dropout, Flatten, Conv1D, Input, MaxPooling1D
    from tensorflow.keras.models import Model
    from tensorflow.keras import backend as K
    from tensorflow.keras.layers import GRU, LSTM

    K.clear_session()

    # K._get_available_gpus()

    inputs = Input(shape=(3000, 6))
    x = BatchNormalization(axis=-1, momentum=0.99, epsilon=1e-3, center=True, scale=True)(inputs)

    # First Conv1D layer
    x = Conv1D(8, 13, padding='valid', activation='relu', strides=1)(x)
    x = MaxPooling1D(3)(x)
    x = Dropout(0.3)(x)

    # Second Conv1D layer
    x = Conv1D(16, 11, padding='valid', activation='relu', strides=1)(x)
    x = MaxPooling1D(3)(x)
    x = Dropout(0.3)(x)

    # Third Conv1D layer
    x = Conv1D(32, 9, padding='valid', activation='relu', strides=1)(x)
    x = MaxPooling1D(3)(x)
    x = Dropout(0.3)(x)

    x = BatchNormalization(axis=-1, momentum=0.99, epsilon=1e-3, center=True, scale=True)(x)

    x = Bidirectional(LSTM(128, return_sequences=True), merge_mode='sum')(x)
    x = Bidirectional(LSTM(128, return_sequences=True), merge_mode='sum')(x)
    x = Bidirectional(LSTM(128, return_sequences=False), merge_mode='sum')(x)

    x = BatchNormalization(axis=-1, momentum=0.99, epsilon=1e-3, center=True, scale=True)(x)

    # Flatten layer
    # x = Flatten()(x)

    # Dense Layer 1
    x = Dense(256, activation='relu')(x)
    outputs = Dense(5, activation="softmax")(x)

    model = Model(inputs, outputs)
    return model


def get_DeepSpeech_model():
    from tensorflow.keras.layers import Bidirectional, BatchNormalization, TimeDistributed, GlobalMaxPool1D
    from tensorflow.keras.layers import Dense, Dropout, Flatten, Conv1D, Input, MaxPooling1D
    from tensorflow.keras.models import Model
    from tensorflow.keras import backend as K
    from tensorflow.keras.layers import GRU, LSTM

    K.clear_session()

    # K._get_available_gpus()

    inputs = Input(shape=(3000, 6))
    x = BatchNormalization(axis=-1, momentum=0.99, epsilon=1e-3, center=True, scale=True)(inputs)

    # First Conv1D layer
    x = Conv1D(8, 13, padding='valid', activation='relu', strides=1)(x)
    x = MaxPooling1D(3)(x)
    x = Dropout(0.3)(x)

    # Second Conv1D layer
    x = Conv1D(16, 11, padding='valid', activation='relu', strides=1)(x)
    x = MaxPooling1D(3)(x)
    x = Dropout(0.3)(x)

    # Third Conv1D layer
    x = Conv1D(32, 9, padding='valid', activation='relu', strides=1)(x)
    x = MaxPooling1D(3)(x)
    x = Dropout(0.3)(x)

    x = BatchNormalization(axis=-1, momentum=0.99, epsilon=1e-3, center=True, scale=True)(x)

    x = Bidirectional(LSTM(128, return_sequences=True), merge_mode='sum')(x)
    x = Bidirectional(LSTM(128, return_sequences=True), merge_mode='sum')(x)
    x = Bidirectional(LSTM(128, return_sequences=True), merge_mode='sum')(x)
    x = Bidirectional(LSTM(128, return_sequences=True), merge_mode='sum')(x)
    x = Bidirectional(LSTM(128, return_sequences=False), merge_mode='sum')(x)

    x = BatchNormalization(axis=-1, momentum=0.99, epsilon=1e-3, center=True, scale=True)(x)

    # Flatten layer
    # x = Flatten()(x)

    # Dense Layer 1
    x = Dense(256, activation='relu')(x)
    outputs = Dense(5, activation="softmax")(x)

    model = Model(inputs, outputs)
    return model