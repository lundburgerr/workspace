% Calculate SNR from LUF recordings for different airplane scenarios.
% Use speech when there is no aircraft sound in the background as speech
% referens. The compare at differnt cases of noise in the recordings.

filePath = pwd;

speechName = 'speech_ronneby';                      %speech where no bakground noise from airplane is present
noiseAfterStartName = 'aircraft_noise_after_start'; %Noise after start up of airplane
noiseTankning = 'noise_tankning';                     %

[speech, fs] = wavread([filePath,'\LUF_ronneby_recorded_scenarios\',speechName,'.wav']);
[noiseStart, fs] = wavread([filePath,'\LUF_ronneby_recorded_scenarios\',noiseAfterStartName,'.wav']);
[noiseTankning, fs] = wavread([filePath,'\LUF_ronneby_recorded_scenarios\',noiseTankning,'.wav']);

noiseStart_length = length(noiseStart);
noiseTankning_length = length(noiseTankning);
speech_length = length(speech);

test_length = min([noiseStart_length,noiseTankning_length,speech_length]);
speech = speech(1:test_length,1);
noiseStart = noiseStart(1:test_length,1);
noiseTankning = noiseTankning(1:test_length,1);

SNR_start = calc_snr(speech,noiseStart,fs);
SNR_tankning = calc_snr(speech,noiseTankning,fs);

display(['SNR start: ', num2str(10*log10(SNR_start)),' dB']);
display(['SNR tankning:     ', num2str(10*log10(SNR_tankning)),' dB']);
display(' ');

% figure;
% plot(speech+noiseStart);
% 
% figure;
% plot(speech+noiseTankning);