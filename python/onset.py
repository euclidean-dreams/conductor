import numpy
from madmom.audio import Spectrogram
from madmom.audio.signal import FramedSignal, Signal
from madmom.audio.stft import STFT
from madmom.features.onsets import spectral_flux, OnsetPeakPickingProcessor


def detect_specflux_onset(samples):
    frame = numpy.asarray(samples)
    signal = Signal(frame, sample_rate=44100)
    frames = FramedSignal(signal, frame_size=512, hop_size=128)
    short_time_fourier_transform = STFT(frames)
    spectrogram = Spectrogram(short_time_fourier_transform)
    onset_processor_output = spectral_flux(spectrogram)
    peak_picker = OnsetPeakPickingProcessor(onset_processor_output)
    result = peak_picker(onset_processor_output).tolist()
    print(result)
    return result
