close all;
clear all;

%% Choose signals to test

filePath = pwd;

postTrueVoiceName = 'post_trueVoice';
attenuatedNoiseName = 'attenuated_noise_8kHz';

postTrueVoice = load([filePath,'\LUF_post_TrueVoice\',postTrueVoiceName]);
[attenuatedNoise, fs] = wavread([filePath,'\LUF_attenuated_noise\',attenuatedNoiseName,'.wav']);

TrueVoice_length = length(postTrueVoice);
start_TrueVoice = floor(TrueVoice_length/2);
noise_length = length(attenuatedNoise);

test_length = min(TrueVoice_length,noise_length);

postTrueVoice = postTrueVoice(start_TrueVoice:start_TrueVoice+test_length-1,1);
attenuatedNoise = attenuatedNoise(1:test_length,1);

%% changed to floating point and write to .wav
postTrueVoice = postTrueVoice./32768;
namePostTrueVoice = [filePath,'\LUF_post_TrueVoice\post_TrueVoice.wav'];
wavwrite(postTrueVoice, fs, namePostTrueVoice);

%% Degrade signal to simulate use of Radio


%% Calculate SNR

SNR = calc_snr(postTrueVoice,attenuatedNoise,fs);

%display(['SNR PESQ file: ', num2str(SNR)]);
display(['SNR PESQ file: ', num2str(10*log10(SNR)),' dB']);
display(' ');

%% Add signals that are gonig to be tested

pesqTestSignal = attenuatedNoise + postTrueVoice;

% PESQ test signal
namePesqTest = [filePath,'\LUF_PESQ\PESQ_mic_8kHz.wav'];
wavwrite(pesqTestSignal, fs, namePesqTest);

