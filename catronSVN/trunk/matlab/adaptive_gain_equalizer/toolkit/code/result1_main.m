%RESULT1_MAIN
% Produces figure which shows the difference in noise estimation between
% Method 1 and Method 3.
%
% Method 1: The method described by Westerlund et al.
% Method 3: The noise level is estimated with Method 1 but only updated
%           when the block difference is small. The same subband gain as in
%           Method 1 is used.

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2009-12-01
% Last modification - 2010-02-09

clear all, close all

% Start timer
tic;

DIR_HOME = pwd;
DIR_DATA = '.\signals\';
DIR_RES = '.\result\';

disp('_____________________________________________________________')
help result1_main


%time = toc;
disp(['-- Time: ',num2str(toc),' s. ','Loading audio signals'])
% s0 = [DIR_DATA,'s0_16kHz.wav'];
% s0 = [DIR_DATA,'s0multi_16kHz.wav'];
% s0 = [DIR_DATA,'s0_16kHz_no_silence.wav'];
s0 = [DIR_DATA,'s0_16kHz_nlively_disc_REVERB_WARM2.wav'];
% s0 = [DIR_DATA,'christian_speech3.wav'];
% noise = [DIR_DATA,'n_16kHz.wav'];
% noise = [DIR_DATA,'npink_16kHz.wav'];
noise = [DIR_DATA,'ventillation.wav'];
% noise = [DIR_DATA,'dator.wav'];
noise2 = [DIR_DATA,'silence_ventillation.wav'];
% noise2 = [DIR_DATA,'silence_npink_16kHz.wav'];


% Gain (in dB) applied to speech and noise signal
sgain = 0; %-9
ngain = 0; %0
n2gain = 6; %6

% Input data, combine speech and noise signal
[mic1, fsA, xsa, xna] = combine_signals(s0, noise, sgain, ngain, 40);
[mic2, fsB, xsb, xnb] = combine_signals(noise2, noise, n2gain, ngain, 40);
SIMU_LENGTH = length(mic1);

% Find active speech regions
[sInd, sAct] = active_speech(xsa, 0.001, fsA, 0.01);
% Calculate SNR
temp = xsa;
[junk1, junk2, junk3, iSNR, junk4] = speech_quality(xsa, temp, sAct, xna, temp, 0.0);
disp(['# Input SNR: ',num2str(iSNR)]);
% close all, plot(xs), hold on, plot(sAct,'r')


% --- Settings ---
% Subband filterbank settings
N = 256; % Number of prototype filter coefficients
M = 64; % Number of subbands
R = 32; % Decimation ratio
load filter64_256 % Loads p

% Subband length
Lsub = ceil(SIMU_LENGTH/R);
% time = [0:SIMU_LENGTH-1]./fs;
% time_sub = [0:Lsub-1]./(fs/R);
% window_size = 512;
% % Frequency vector
% freq = [0:0.5*window_size]*(0.5*fs);
% freq = freq./length(freq);

% \\ Method 1
% Settings for Westerlund et. al. method
alpha1 = 0.001; %0.0004
beta1 = 1.8e-5; %3e-5
pow1 = ones(M,1)*1;
GLdB = 12; % Maximum enhancement in dB
GL = 10^(GLdB/20);
% Change parameters since we use downsampling in our subband processing
% Nils: 8 kHz med 16 subband
% Vi: 16 kHz med 32 subband, decimering 32
% För varje subbandssample hos oss så borde det väl finnas 16st sample hos nils eftersom den inte är nedsamplad
alpha1 = 1 - (1-alpha1)^(0.5*R);
beta1 = (1+beta1)^(0.5*R) - 1;
% beta1 = 10.^(3./(20*500))-1

% \\ Method 2
% Settings for Borgh et. al. method
beta2 = 0.05; %1-hex2dec('0b00')/32767; %0.05, %beta1;
block_size = 8;%64; % Accumulation length
block_nr = 64;%8; % Nr of blocks in accumulation
comp_factor = 2; % Factor to compensate difference between min- and mean-value in noise level estimation
noise_thresh = [3,10,20]; % Minimum blockmax/blockmin ratio to detect speech
% 3,5,8   - if subbands not connected
% 3,10,20 - if subbands are connected
% \\ Method 3
% Increase of noise estimation during silence
% beta3 = 5e-3;
beta3 = (5e-3)/block_size;
% beta3 = beta1

% --- Calculations ---
% Split signals in subbands
disp(['-- Time: ',num2str(toc),' s. ','Subband splitting'])
X_vecA = SubAnal(mic1, N, M, R, p);
X_vecB = SubAnal(mic2, N, M, R, p);

% >> Subband processing
disp(['-- Time: ',num2str(toc),' s. ','Calculating averages'])
% Calculate short and long time averages of positive frequencies
SUB_POS = M/2+1; % Nr of subbands with positive frequencies
[A1, An1] = averages(X_vecA, alpha1, beta1, Lsub, SUB_POS); % Westerlund et. al
[A3, An31] = averages4(X_vecA, alpha1, beta3, Lsub, SUB_POS, block_nr, block_size, noise_thresh(1));
[A3, An32] = averages4(X_vecA, alpha1, beta3, Lsub, SUB_POS, block_nr, block_size, noise_thresh(2));
[A3, An33] = averages4(X_vecA, alpha1, beta3, Lsub, SUB_POS, block_nr, block_size, noise_thresh(3));
%[A3, An34] = averages4(X_vecA, alpha1, beta3, Lsub, SUB_POS, block_nr, block_size, noise_thresh(4));
load speech4
SA = maxi_save4;

[B1, Bn1] = averages(X_vecB, alpha1, beta1, Lsub, SUB_POS); % Westerlund et. al
[B3, Bn31] = averages4(X_vecB, alpha1, beta3, Lsub, SUB_POS, block_nr, block_size, noise_thresh(1));
[B3, Bn32] = averages4(X_vecB, alpha1, beta3, Lsub, SUB_POS, block_nr, block_size, noise_thresh(2));
[B3, Bn33] = averages4(X_vecB, alpha1, beta3, Lsub, SUB_POS, block_nr, block_size, noise_thresh(3));
%[B3, Bn34] = averages4(X_vec, alpha1, beta3, Lsub, SUB_POS, block_nr, block_size, noise_thresh(4));
load speech4
SB = maxi_save4;


% Use the fact that there is a Hermitian symmetry owing to real input signals
A1 = hermitian_symmetry(A1, Lsub, M);
An1 = hermitian_symmetry(An1, Lsub, M);
A3 = hermitian_symmetry(A3, Lsub, M);
An31 = hermitian_symmetry(An31, Lsub, M);
An32 = hermitian_symmetry(An32, Lsub, M);
An33 = hermitian_symmetry(An33, Lsub, M);
%An34 = hermitian_symmetry(An34, Lsub, M);
SA = hermitian_symmetry(SA, Lsub, M);
B1 = hermitian_symmetry(B1, Lsub, M);
Bn1 = hermitian_symmetry(Bn1, Lsub, M);
B3 = hermitian_symmetry(B3, Lsub, M);
Bn31 = hermitian_symmetry(Bn31, Lsub, M);
Bn32 = hermitian_symmetry(Bn32, Lsub, M);
Bn33 = hermitian_symmetry(Bn33, Lsub, M);
%An34 = hermitian_symmetry(Bn34, Lsub, M);
SB = hermitian_symmetry(SB, Lsub, M);

% Combine to 3d matrix with different noise_thresh for each "plane"
An3t(:,:,1) = An31;
An3t(:,:,2) = An32;
An3t(:,:,3) = An33;
%An3t(:,:,4) = An34;
Bn3t(:,:,1) = Bn31;
Bn3t(:,:,2) = Bn32;
Bn3t(:,:,3) = Bn33;
%Bn3t(:,:,4) = Bn34;


% --- Gain calculations ---
disp(['-- Time: ',num2str(toc),' s. ','Calculating subband gain'])
GA1 = gains(A1, An1, pow1, GL);
GA32 = gains(A3, An32, pow1, GL);
GB1 = gains(B1, Bn1, pow1, GL);
GB32 = gains(B3, Bn32, pow1, GL);
GA1 = GA1/GL;
GA32 = GA32/GL;
GB1 = GB1/GL;
GB32 = GB32/GL;



% --- Paper figures ---
disp(['-- Time: ',num2str(toc),' s. ','Creating result figures'])
result_figure1(A1, An1, An3t, ceil(fsA/R), 20.5, 39.8, DIR_RES, 'result1a', noise_thresh, GA1, GA32, 2, SA, mic1); %4
result_figure1(B1, Bn1, Bn3t, ceil(fsB/R), 15, 25, DIR_RES, 'result1b', noise_thresh, GB1, GB32, 2, SB, mic2);


% --- .wav creation ---
disp(['-- Time: ',num2str(toc),' s. ','Writing .wav'])
disp(['   ++ Time: ',num2str(toc),' s. ','Applying gain'])
% Apply gain
YA1 = X_vecA.*GA1;
YA32 = X_vecA.*GA32;
disp(['   ++ Time: ',num2str(toc),' s. ','Subband combining'])
[e_outA1, e_tdl] = SubSynt(YA1, N, SIMU_LENGTH-N, R, p);
[e_outA32, e_tdl] = SubSynt(YA32, N, SIMU_LENGTH-N, R, p);
disp(['   ++ Time: ',num2str(toc),' s. ','Post processing'])
e_outA1 = fill_up_with_const(e_outA1, SIMU_LENGTH, 0);
e_outA32 = fill_up_with_const(e_outA32, SIMU_LENGTH, 0);
disp(['   ++ Time: ',num2str(toc),' s. ','Writing'])
sig = [xsa, mic1, e_outA1', e_outA32'];
result_wav(sig, fsA, 20.5, 39.8, 1, [DIR_RES,'FIG1A__s_sn_m1_m3.wav']);



disp(['-- Time: ',num2str(toc),' s. ','Checking betas'])
% N = 500;
% % beta1 = 0.00028; beta3 = 0.005; block_nr = 8;
% t = [0:1:N]./500; XX = 20*log10([(1+beta1).^(500*t); (1+beta3/block_nr).^(500*t)] );
% figure, plot(t,XX(1,:)), hold on, plot(t,XX(2,:),'r')
% legend(['\beta_1 = ',num2str(beta1)],['\beta_3 = ',num2str(beta3/block_nr)]), ylabel('Amplitude [dB]'), xlabel('Time [s]')

disp(['-- Time: ',num2str(toc),' s. ','Finished'])
