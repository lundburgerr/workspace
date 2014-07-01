clear
close all

% Load files
Na = wavread('2013-03-27 - Airplane_Noise97dBA_at_50cm.wav');
Na2 = wavread('Airplane_Noise2.wav');
Nw = wavread('2013-03-27 - White_Noise96,5dBA_at_50cm.wav');
Sn = wavread('2013-03-27 - Normal_Speech.wav');
Sl = wavread('2013-03-27 - Loud_Speech.wav');
fs = 48000;
%

% time settings
silenceLength = 2*fs; %length of silence that will be added so sound
audioLength = 7*fs; %length of creted signals
%

% noice reduction settings
useDelay = false;
%

% add zeros (silence) in the begining of the sounds
Sn = [zeros(silenceLength,1);Sn];
Sl = [zeros(silenceLength,1);Sl];
%

% fix soundlevels
targetSNR = calculate_snr(Sl, Na(1:audioLength), fs);

noiceSNR = calculate_snr(Sl, Na2(1:audioLength), fs);
diffSNR = noiceSNR-targetSNR;
Na2 = Na2*10^(diffSNR/20);

%checkSNR = calculate_snr(Sl, Na2(1:audioLength), fs)-targetSNR
%

% choose noice sound
NoiceSound = Na;

dBGain = 30;
dBMultiplayer = 10^(dBGain/20);

Sound = Sl(1:audioLength);
Noise = dBMultiplayer*NoiceSound(1:audioLength);

algoritmFs = 16000;
Sound = resample(Sound,algoritmFs,fs);
Noise = resample(Noise,algoritmFs,fs);

% Create filtered sounds
% h=firls(255,[0 .05 .05 .25 .25 1],[0 0 1 1 0 0]);
h=firls(512,[0 300 300 3500 3500 (0.5*algoritmFs)]./(0.5*algoritmFs),[0 0 1 1 0 0]);
%h=firls(255,[0 .25 .25 1],[1 1 0 0]);
FilteredSound = filter(h, 1 ,Sound);
FilteredNoise = filter(h, 1 ,Noise);
% fvtool(h);
%

%%%%%%%%%%%%%
% Unprocessed sound and noice
unprocessedX = Sound+Noise;
unprocessedX = unprocessedX./max(abs(unprocessedX));
Unprocessed_SNR = calculate_snr(Sound, Noise, fs)
%soundsc(unprocessedX,algoritmFs);
%%%%%%%%%%%%%

%%%%%%%%%%%%%
% Processed sound and noice
[processedX newS newN] = noice_reduction(Sound,0,Noise,0,algoritmFs,useDelay,false);
Processed_SNR = calculate_snr(newS, newN, algoritmFs)
processedX = processedX./max(abs(processedX));
%soundsc(processedX,algoritmFs);
%%%%%%%%%%%%%

%%%%%%%%%%%%%
% Filtered sound and noice
unprocessedFilteredX = FilteredSound+FilteredNoise;
unprocessedFilteredX = unprocessedFilteredX./max(abs(unprocessedFilteredX));
UnprocessedFiltered_SNR = calculate_snr(FilteredSound, FilteredNoise, fs)
%soundsc(unprocessedFilteredX,algoritmFs);
%%%%%%%%%%%%%

%%%%%%%%%%%%%
% Filtered and Processed sound and noice
[processedFilteredX newS newN, newG] = noice_reduction(FilteredSound,0,FilteredNoise,0,algoritmFs,useDelay,false);
ProcessedFiltered_SNR = calculate_snr(newS, newN, algoritmFs)
processedFilteredX = processedFilteredX./max(abs(processedFilteredX));
%soundsc(processedFilteredX,algoritmFs);
%%%%%%%%%%%%%

%%%%%%%%%%%%%
% Theroretical max sound and noice
%[theroreticalX newS newN, newG] = noice_reduction(FilteredSound,0,FilteredNoise,0,algoritmFs,useDelay,true);
%theroretical_SNR = calculate_snr(newS, newN, algoritmFs)
%processedFilteredX = processedFilteredX./max(abs(processedFilteredX));
%soundsc(processedFilteredX,algoritmFs);
%%%%%%%%%%%%%

figure
plot(unprocessedX)
hold on
plot(processedX,'r')
plot(processedFilteredX,'g')

figure
subplot(3,1,1)
specgram(unprocessedX)
subplot(3,1,2)
specgram(processedX)
subplot(3,1,3)
specgram(processedFilteredX)


figure
pwelch(Noise,[],[],1024,fs)

%wavwrite(processedFilteredX./max(abs(processedFilteredX))*0.9, algoritmFs, 'speechnoise_delay');

% 
% wavwrite(Sound./max(abs(Sound))*0.9, algoritmFs, 'wav/speech');
% wavwrite(unprocessedX./max(abs(unprocessedX))*0.9, algoritmFs, 'wav/speechnoise');
% wavwrite(processedX./max(abs(processedX))*0.9, algoritmFs, 'wav/speechnoise_proc');
% wavwrite(unprocessedFilteredX./max(abs(unprocessedFilteredX))*0.9, algoritmFs, 'wav/speechnoise_filter');
% wavwrite(processedFilteredX./max(abs(processedFilteredX))*0.9, algoritmFs, 'wav/speechnoise_filter_proc');


% figure
% subplot(2,1,1)
% specgram(Sound+Noise)
% subplot(2,1,2)
% specgram(unprocessedX)

% res = measure_rhyme_test();
% if res==4.5
%     OK!!
% end


arrange_figures(gcf, 3)
