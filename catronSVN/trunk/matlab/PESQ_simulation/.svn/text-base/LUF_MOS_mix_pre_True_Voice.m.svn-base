close all;
clear all;

%% Choose signals to test

filePath = pwd;

% noiseName = '20131206_LUF_vittbrus_91,2dB_1,5m_ch1_talmic_ch2_brusmic';
% speechName = '20131206_LUF_tal_71,3dB_1,5m_ch1_talmic_ch2_brusmic';

%noiseName = 'urklippt_brus_50HzFiltered';
%noiseName = 'urklippt_brus_50HzFiltered_amp24dBdigitalt';
noiseName = 'urklippt_startMotor_ronneby_50HzFiltered';
speechName = 'urklippt_tal_filt50Hz'; %'urklippt_tal_tjej';

[noise_stereo, fs] = wavread([filePath,'\',noiseName,'.wav']);
[speech_stereo, fs] = wavread([filePath,'\',speechName,'.wav']);

noise_length = length(noise_stereo);
speech_length = length(speech_stereo);

test_length = min(noise_length,speech_length);

noise_headset = noise_stereo(1:test_length,1)*0.2;
noise_ref_mic = noise_stereo(1:test_length,2)*0.2;

speech_headset = speech_stereo(1:test_length,1);
speech_ref_mic = speech_stereo(1:test_length,2);

%% Save ref signal for PESQ
namePesqRef = [filePath,'\LUF_PESQ\PESQ_ref_8kHz.wav'];
wavwrite(speech_headset, fs, namePesqRef);

tempName = [filePath,'\LUF_PESQ\temp.wav'];
wavwrite(noise_headset, fs, tempName);

%% Calulate SNR
SNR = calc_snr(speech_headset,noise_headset,fs);
SNR_ref = calc_snr(speech_ref_mic,noise_ref_mic,fs);

% display(['SNR headset mic: ', num2str(SNR)]);
% display(['SNR ref mic:     ', num2str(SNR_ref)]);
% display(' ');
display(['SNR headset mic: ', num2str(10*log10(SNR)),' dB']);
display(['SNR ref mic:     ', num2str(10*log10(SNR_ref)),' dB']);
display(' ');

%% Add signals that are gonig to be used by True Voice

speech_and_noise_headset = noise_headset + speech_headset;
speech_and_noise_ref = noise_ref_mic + speech_ref_mic;

figure;
plot(speech_and_noise_headset);

% Save Headset mic signal
% As .wav file
nameHeadsetMicToTrueVoice = [filePath,'\LUF_TrueVoice\speech_and_noise_headset_8kHz.wav'];
wavwrite(speech_and_noise_headset, fs, nameHeadsetMicToTrueVoice);
% As .inc file 16-bit
inc_file_name_headset = 'speech_and_noise_headset_8kHz.inc';
write_to_inc(speech_and_noise_headset*32768, pwd, inc_file_name_headset);

% Save Noise reference signal
% As .wav file
nameRefMicToTrueVoice = [filePath,'\LUF_TrueVoice\speech_and_noise_ref_8kHz.wav'];
wavwrite(speech_and_noise_ref, fs, nameRefMicToTrueVoice);
% As .inc file 16-bit
inc_file_name_ref = 'speech_and_noise_ref_8kHz.inc';
write_to_inc(speech_and_noise_ref*32768, pwd, inc_file_name_ref);

%% Attenuate noise signal to mimic the effect the earcups have
% Different attenuation in different frequencies:
% H (dB)   33
% M (dB)   31
% L (dB)   24
% SNR (dB) 33
%
% What does H, M, L mean?

% Attenuate signal with 30 dB for all frequencies
attenuated_noise = noise_ref_mic*0.031;

P_x_noise = mean(noise_ref_mic.^2);
P_x_attenuatedNoise = mean((attenuated_noise).^2);

damping = 10*log10(P_x_attenuatedNoise/P_x_noise); % attenuation in dB

display(['Noise attenuated with ', num2str(damping),' dB']);
display(' ');

% Save attenuated noise signal
nameAttenuatedNoise = [filePath,'\LUF_attenuated_noise\attenuated_noise_8kHz.wav'];
wavwrite(attenuated_noise, fs, nameAttenuatedNoise);

%% save ref file + attenuated noise
pesqRefNoiseSignal = attenuated_noise + speech_headset;

% PESQ test signal
namePesqRefNoiseSignal = [filePath,'\LUF_PESQ\PESQ_ref_noise_8kHz.wav'];
wavwrite(pesqRefNoiseSignal, fs, namePesqRefNoiseSignal);

