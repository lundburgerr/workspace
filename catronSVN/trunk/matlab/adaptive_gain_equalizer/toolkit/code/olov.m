%RESULT_MOS
% Produces wav files for MOS tests that compares Westerlund et al with our
% proposed method.
%

% Author - Markus Borgh
% E-mail - markus.borgh@limestechnology.com
% Website - www.limestechnology.com
% Copyright - Limes Technology AB
% Created - 2010-01-21
% Last modification - 2011-May-24

clear all, close all

% Start timer
tic;

DIR_HOME = pwd;
DIR_DATA = '.\signals\';
DIR_RES = '.\MOS\';

DISTORTION_CALC = 0;
APPEND_TO_TXT_FILE = 0;

disp('_____________________________________________________________')
help result_mos

% Which noise source should be used
nSource = {'car','comp','vent','white','cocktail','music'};
nMode = 1;

beta_rise_db_per_sec = 9

%time = toc;
disp(['-- Time: ',num2str(toc),' s. ','Loading audio signals'])
% s0 = [DIR_DATA,'s0_16kHz.wav'];
% s0 = [DIR_DATA,'s0mod_16kHz.wav'];
% s0 = [DIR_DATA,'s0mod2_16kHz.wav'];
% s0 = [DIR_DATA,'sMaleFemale_16kHz.wav'];
% s0 = [DIR_DATA,'p501_2female_2male.wav'];
s0 = [DIR_DATA,'p501_2female_2male_ver2.wav']; % 4s extra silence
% s0 = [DIR_DATA,'cocktail_chatter_wspeech.wav']; % Use no noise to this one...
% s0 = [DIR_DATA,'cocktail_chatter_wspeech_2.wav']; % Use no noise to this one...
% s0 = [DIR_DATA,'p501_2female_2male_REVERB_WARM2_ver2.wav'];
% s0 = [DIR_DATA,'s0_16kHz_nlively_disc_REVERB_WARM2.wav'];
% s0 = [DIR_DATA,'christian_speech3.wav'];
% s0 = [DIR_DATA,'noizeus16kHz_combined_1s_silence.wav'];
% s0 = [DIR_DATA,'rosetta_stoned_16khz.wav'];
% s0 = [DIR_DATA,'markus_christian_count.wav'];
% s0 = [DIR_DATA,'markus_christian_marcus_count_cut.wav'];
% s0 = [DIR_DATA,'telia_markus_fredric.wav'];

switch nMode
    case 1
        noise = [DIR_DATA,'car100kmh.wav'];
    case 2
        noise = [DIR_DATA,'dator.wav'];
    case 3
        noise = [DIR_DATA,'ventillation.wav'];
    case 4
        % noise = [DIR_DATA,'n_16kHz.wav'];
        noise = [DIR_DATA,'n_16kHz_short.wav'];
    case 5
        noise = [DIR_DATA,'cocktail_noise4_16khz.wav'];
    case 6
        noise = [DIR_DATA,'rosetta_stoned_16khz.wav'];
    otherwise
        % noise = [DIR_DATA,'npink_16kHz.wav'];
        % noise = [DIR_DATA,'cocktail_noise2.wav'];
        % noise = [DIR_DATA,'silence.wav'];
end

% Gain (in dB) applied to speech and noise signal
sgain = 6;
% sgain = 0;
switch nMode
    % This is adjusted to get 0 dB SNR for s0 = [DIR_DATA,'p501_2female_2male_ver2.wav'];
    case 1
        ngain = 7.75;
    case 2
        ngain = 5.69;
    case 3
        ngain = 5.11;
    case 4
        ngain = -60; % This is not 0 dB...
    case 5
        ngain = 17.74;
    case 6
        ngain = -10.3;
    otherwise
        ngain = 0;
end
% Adjust SNR
% ngain = ngain - 0; % 0 dB
% ngain = ngain - 6; % -6 dB
% ngain = ngain - 12;
% ngain = ngain - 18;
ngain = ngain - 24;


% Input data, combine speech and noise signal
totLengthTime = 75;
[mic1, fs, xs, xn] = combine_signals(s0, noise, sgain, ngain, totLengthTime);
% [mic1, fs, xs, xn] = combine_signals(s0, noise, sgain, ngain, 60+55);
SIMU_LENGTH = length(mic1);

% --- Settings ---
% Subband filterbank settings
N = 256; % Number of prototype filter coefficients
M = 64; % Number of subbands
R = 32; % Decimation ratio
load filter64_256 % Loads p

% Subband length
Lsub = ceil(SIMU_LENGTH/R);
time = [0:SIMU_LENGTH-1]./fs;
time_sub = [0:Lsub-1]./(fs/R);
window_size = 512;
% Frequency vector
freq = [0:0.5*window_size]*(0.5*fs);
freq = freq./length(freq);

% \\ Method 1
% Settings for Westerlund et. al. method
alpha1 = 0.001; %0.0004
beta1 = 1.8e-5; %3e-5
pow1 = ones(M,1)*1;
GLdB = 30; % Maximum enhancement in dB
GL = 10^(GLdB/20);
% Change parameters since we use downsampling in our subband processing
% Nils: 8 kHz med 16 subband
% Vi: 16 kHz med 32 subband, decimering 32
% För varje subbandssample hos oss så borde det väl finnas 16st sample hos nils eftersom den inte är nedsamplad
alpha1 = 1 - (1-alpha1)^(0.5*R);
beta1 = (1+beta1)^(0.5*R) - 1;
% Rise/s:   db((1+beta1)^(1*500))
RISE = [1:10]; % Rise in dB/s
betas = [10.^(RISE./(20*500))-1]'; % 500 samples is one second
%beta1 = 4.61e-4; % 2dB / s
beta1 = 10.^(beta_rise_db_per_sec./(20*500))-1; %betas(beta_rise_db_per_sec);

% \\ Method 2
% Settings for Borgh et. al. method
beta2 = 0.05; %1-hex2dec('0b00')/32767; %0.05, %beta1;
block_size = 8; % Accumulation length
block_nr = 64; % Nr of blocks in accumulation
noise_thresh = 10; % Minimum blockmax/blockmin ratio to detect speech

% \\ Method 3
% Increase of noise estimation during silence
beta3 = (5e-3)/block_size;
beta3 = beta1;

% \\ Method 4
% pow4 = ones(M,1)*1;
% pow4(2) = 2; % 2 seems to work ok with s0 and ventillation - 3(9) dB
% pow4(M/2+1:M) = pow4(M/2:-1:1);

% \\ Method 5
% Use different rise/fall parameter, 4 times faster
alpha5 = alpha1;%*4;
% Maximum subband enhancement in dB
GLsub_dB = 12;
GLsub_dB = min(GLsub_dB, GLdB); % Can't be more than maximal total gain
GLsub = 10^(GLsub_dB/20);


% --- Calculations ---
% Split signals in subbands
disp(['-- Time: ',num2str(toc),' s. ','Subband splitting'])
X_vec = SubAnal(mic1, N, M, R, p);

% >> Subband processing
disp(['-- Time: ',num2str(toc),' s. ','Calculating averages'])
% Calculate short and long time averages of positive frequencies
SUB_POS = M/2+1; % Nr of subbands with positive frequencies
[A5, An5] = averages4(X_vec, alpha5, beta3, Lsub, SUB_POS, block_nr, block_size, noise_thresh); % Modified FENIX



% Use the fact that there is a Hermitian symmetry owing to real input signals
A5 = hermitian_symmetry(A5, Lsub, M);
An5 = hermitian_symmetry(An5, Lsub, M);


% Gain calculation
disp(['-- Time: ',num2str(toc),' s. ','Calculating and applying subband gain'])
% G5sub = gains(A5, An5, pow4, GLsub);   % <--------   IMPORTANT, CHANGED FROM pow1
G5sub = gains(A5, An5, pow1, GLsub);   % The effect of varying pow is not included in paper
G5tot = G5sub;


% Make 0dB maximum
G5tot = G5tot./GLsub;


% Apply gain
Y5 = X_vec.*G5tot;


% >> Put together subbands
disp(['-- Time: ',num2str(toc),' s. ','Subband combining'])
[e_out5, e_tdl] = SubSynt(Y5, N, SIMU_LENGTH-N, R, p);
[g_out5, g_tdl] = SubSynt(G5tot, N, SIMU_LENGTH-N, R, p);


% --- Post processing ---
disp(['-- Time: ',num2str(toc),' s. ','Post processing'])
% Fill up with zeros to get same length as other vectors
e_out5 = fill_up_with_const(e_out5, SIMU_LENGTH, 0);
g_out5 = fill_up_with_const(g_out5, SIMU_LENGTH, 0);




% ==== SANDBOX ====
xsT = xs.'; % Transpose
xnT = xn.';
% Find active speech regions
[sInd, sAct] = active_speech(xs, 0.001, fs, 0.01);


if(DISTORTION_CALC)
    % Speech signal only
    S_vec = SubAnal(xs, N, M, R, p);
    SF5 = S_vec.*G5tot;
    [sf5, e_tdl] = SubSynt(SF5, N, SIMU_LENGTH-N, R, p);
    sf5 = fill_up_with_const(sf5, SIMU_LENGTH, 0);

    % Noise signal only
    N_vec = SubAnal(xn, N, M, R, p);
    NF5 = N_vec.*G5tot;
    [nf5, e_tdl] = SubSynt(NF5, N, SIMU_LENGTH-N, R, p);
    nf5 = fill_up_with_const(nf5, SIMU_LENGTH, 0);

    % Speech distortion measure
    % reg = 1e-10;
    % v5Full = (sf5-xsT).^2./(xsT.^2 + reg);
    % vWestFull = (sfWest-xsT).^2./(xsT.^2 + reg);
    [sdi5, sr5, nr5, iSNR5, oSNR5] = speech_quality(xs, sf5, sAct, xn, nf5, 0.3);

%     % Speech reduction factor
%     % Trace(Rxs)
%     % Rxs = xcorr(xs,xs,0); % =sum(xs.^2)
%     % Rsf5 = xcorr(sf5,sf5,0);
%     % RsfWest = xcorr(sfWest,sfWest,0);
%     % chi5 = Rxs/Rsf5;
%     % chiWest = Rxs/RsfWest;
% 
%     BS = 1000; % Block size
%     chis5 = zeros(size(xsT));
%     chisWest = zeros(size(xsT));
%     chin5 = zeros(size(xsT));
%     chinWest = zeros(size(xsT));
%     v5 = zeros(size(xsT));
%     vWest = zeros(size(xsT));
% 
%     warning off all
%     for(i=(BS+1):BS/100:SIMU_LENGTH)
%         % Speech reduction
%         xTemp = xs(i-BS:i);
%         Rxs = sum(xTemp.^2); % = xcorr(xTemp, xTemp, 0);
%         xTemp = sf5(i-BS:i);
%         Rsf5 = sum(xTemp.^2);
%         xTemp = sfWest(i-BS:i);
%         RsfWest = sum(xTemp.^2);
% 
%         chis5(i-BS:i) = Rxs/Rsf5;
%         chisWest(i-BS:i) = Rxs/RsfWest;
% 
%         % Noise reduction
%         xTemp = xn(i-BS:i);
%         Rxn = sum(xTemp.^2); % = xcorr(xTemp, xTemp, 0);
%         xTemp = nf5(i-BS:i);
%         Rnf5 = sum(xTemp.^2);
%         xTemp = nfWest(i-BS:i);
%         RnfWest = sum(xTemp.^2);
% 
%         chin5(i-BS:i) = Rxn/Rnf5;
%         chinWest(i-BS:i) = Rxn/RnfWest;
% 
%         % Speech distortion
%         xTemp = xsT(i-BS:i);
%         regV = 1e-10;
%         sTemp = sf5(i-BS:i);
%         v5(i-BS:i) = mean((sTemp-xTemp).^2)./(mean(xTemp.^2) + regV);
%         sTemp = sfWest(i-BS:i);
%         vWest(i-BS:i) = mean((sTemp-xTemp).^2)./(mean(xTemp.^2) + regV);
%     end
%     warning on all
% 
%     gamma_v = 0.999;
%     vWestAve = smoothing(vWest, gamma_v, gamma_v);
%     v5Ave = smoothing(v5, gamma_v, gamma_v);
%     % close all
%     figure
%     fhv(1) = subplot(3,1,1);
%     plot(vWestAve,'k')
%     hold on
%     plot(v5Ave,'r')
%     %plot(sAct*10000,'b')
%     legend('SBA','Proposed')
%     ylabel('[dB]')
%     title('Speech distortion')
%     fhv(2) = subplot(3,1,2);
%     plot(vWestAve-v5Ave)
%     title('Difference: SBA-Proposed')
%     xlabel('Samples')
%     fhv(3) = subplot(3,1,3);
%     plot(xs)
%     hold on
%     plot(sAct,'r')
%     title('Speech signal')
%     xlabel('Samples')
%     ylabel('Amplitude')
%     % Link axes to zoom in both plots simultaneously
%     linkaxes(fhv,'x'); % Only in x-direction
% 
%     figure
%     fh(1) = subplot(4,1,1);
%     plot(xs)
%     hold on
%     plot(sf5,'r')
%     plot(sfWest,'--k')
% 
%     fh(2) = subplot(4,1,2);
%     plot(10*log10(chisWest).*sAct,'k')
%     hold on
%     plot(10*log10(chis5).*sAct,'r')
%     title('Speech reduction factor')
%     legend('SBA','Proposed')
%     ylabel('[dB]')
% 
%     fh(3) = subplot(4,1,3);
%     plot(10*log10(chinWest).*sAct,'k')
%     hold on
%     plot(10*log10(chin5).*sAct,'r')
%     title('Noise reduction factor')
%     legend('SBA','Proposed')
%     ylabel('[dB]')
% 
%     fh(4) = subplot(4,1,4);
%     plot(10*log10(chinWest./chisWest).*sAct,'k')
%     hold on
%     plot(10*log10(chin5./chis5).*sAct,'r')
%     title('SNR gain')
%     legend('SBA','Proposed')
%     ylabel('[dB]')
% 
%     % Link axes to zoom in both plots simultaneously
%     linkaxes(fh,'x'); % Only in x-direction
    
    %disp(['Speech-Distortion Index: SBA = ',num2str(db(sum(vWest))),' dB. Proposed = ',num2str(db(sum(v5))),' dB'] )
%     disp(['Speech-Distortion Index: SBA = ',num2str(10*log10(sdiWest)),' dB. Proposed = ',num2str(10*log10(sdi5)),' dB'] )
%     disp(['Speech-Reduction Factor: SBA = ',num2str(10*log10(srWest)),' dB. Proposed = ',num2str(10*log10(sr5)),' dB'] )
%     disp(['Noise-Reduction Factor: SBA = ',num2str(10*log10(nrWest)),' dB. Proposed = ',num2str(10*log10(nr5)),' dB'] )
%     disp(['Input SNR: SBA = ',num2str(10*log10(iSNRWest)),' dB. Proposed = ',num2str(10*log10(iSNR5)),' dB'] )
%     disp(['Output SNR: SBA = ',num2str(10*log10(oSNRWest)),' dB. Proposed = ',num2str(10*log10(oSNR5)),' dB'] )
end % if(DISTORTION_CALC)


% SNR measure
varXn = var(xn);
SNR = 10*log10(var(xs)/varXn);
% SNR = 10*log10(var(mic1)/varXn - 1); % This yields same result as above
% SNRour = 10*log10(var(e_out5)/varXn - 1)
% SNRwest = 10*log10(var(e_outWest)/varXn -1)
% SNR in active areas
xsAct = xs.*sAct;
xnAct = xn.*sAct;
SNRact = 10*log10(var(xsAct)/var(xnAct));
SNRactAlt = 10*log10( sum(xsAct.^2) / sum(xnAct.^2));
% disp(['SNR (all data, var) is ',num2str(SNR),' dB']);
% disp(['SNR (active speech, var) is ',num2str(SNRact),' dB']);
disp(['SNR (active speech, sum.^2) is ',num2str(SNRactAlt),' dB']);

plot(mic1)
hold on
plot(e_out5,'r')
legend('Uunprocessed','Processed')


