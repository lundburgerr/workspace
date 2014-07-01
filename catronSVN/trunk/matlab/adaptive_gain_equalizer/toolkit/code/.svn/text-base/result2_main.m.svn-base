%RESULT2_MAIN
% Produces figure which show the difference of letting the noise damping
% increase during speechpauses, i.e. Method 5 vs Method 7.
%
% --- Information ---
% Method 1: The method described by Westerlund et al.
% Method 3: The noise level is estimated with Method 1 but only updated
%           when the block difference is small. The same subband gain as in
%           Method 1 is used.
% Method 4: Same noise level estimation as in Method 3. The subband gain is
%           modified to give less speech distortion.
% Method 5: Noise level is updated with Method 3 but with a different alpha.
%           Both subband and fullband gain is applied.
% Method 6: Rangachari et al.
% Method 7: Noise level is updated with Method 5. A fullband gain is added
%           so that the combined subband and fullband gain equals the
%           maximum total gain of method 5.

% Author - Markus Borgh
% E-mail - markus.borgh@limestechnology.com
% Website - www.limestechnology.com
% Copyright - Limes Technology AB
% Created - 2009-12-01
% Last modification - 2011-May-31

clear all, close all

% Start timer
tic;

DIR_HOME = pwd;
DIR_DATA = '.\signals\';
DIR_RES = '.\result\';

disp('_____________________________________________________________')
help result2_main


%time = toc;
disp(['-- Time: ',num2str(toc),' s. ','Loading audio signals'])
% s0 = [DIR_DATA,'s0_16kHz.wav'];
s0 = [DIR_DATA,'s0mod_16kHz.wav'];
% s0 = [DIR_DATA,'christian_speech3.wav'];
% noise = [DIR_DATA,'n_16kHz.wav'];
% noise = [DIR_DATA,'npink_16kHz.wav'];
noise = [DIR_DATA,'ventillation.wav'];
% noise = [DIR_DATA,'dator.wav'];
% noise = [DIR_DATA,'cocktail_noise2.wav'];

% Gain (in dB) applied to speech and noise signal
sgain = 0;
ngain = 0; %0

% Input data, combine speech and noise signal
[mic1, fs, xs, xn] = combine_signals(s0, noise, sgain, ngain, 40);
SIMU_LENGTH = length(mic1);


% Find active speech regions
[sInd, sAct] = active_speech(xs, 0.001, fs, 0.01);
% Calculate SNR
temp = xs;
[junk1, junk2, junk3, iSNR, junk4] = speech_quality(xs, temp, sAct, xn, temp, 0.0);
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

% \\ Method 2
% Settings for Borgh et. al. method
beta2 = 0.05; %1-hex2dec('0b00')/32767; %0.05, %beta1;
block_size = 8;%64; % Accumulation length
block_nr = 64;%8; % Nr of blocks in accumulation
comp_factor = 2; % Factor to compensate difference between min- and mean-value in noise level estimation
noise_thresh = 10;%9; % Minimum blockmax/blockmin ratio to detect speech

% \\ Method 3
% Increase of noise estimation during silence
% beta3 = 5e-3;
beta3 = (5e-3)/block_size;

% \\ Method 5
% Use different rise/fall parameter, 4 times faster
alpha5 = alpha1;%*4;
% Maximum subband enhancement in dB
GLsub_dB = 12;
GLsub_dB = min(GLsub_dB, GLdB); % Can't be more than maximal total gain
GLsub = 10^(GLsub_dB/20);
% Fullband gain settings
TMdB = 20*log10(2); %6 dB
Tspeech_dB = max(GLsub_dB-TMdB, 0); % Threshold for start noise damping -7.4, -4
Tfast_damping_dB = max(GLdB-GLsub_dB-6, 0);%2; % Fast adaption down to this level

% \\ Method 7
% Constant fullband gain
GL7full_dB = GLdB-GLsub_dB;
GL7full = 10^(GL7full_dB/20);

% --- Calculations ---
% Split signals in subbands
disp(['-- Time: ',num2str(toc),' s. ','Subband splitting'])
X_vec = SubAnal(mic1, N, M, R, p);

% >> Subband processing
disp(['-- Time: ',num2str(toc),' s. ','Calculating averages'])
% Calculate short and long time averages of positive frequencies
SUB_POS = M/2+1; % Nr of subbands with positive frequencies
[A5, An5] = averages4(X_vec, alpha5, beta3, Lsub, SUB_POS, block_nr, block_size, noise_thresh); % Modified FENIX
[AWest, AnWest] = averages(X_vec, alpha5, beta1, Lsub, SUB_POS); % Westerlund et. al


% Use the fact that there is a Hermitian symmetry owing to real input signals
A5 = hermitian_symmetry(A5, Lsub, M);
An5 = hermitian_symmetry(An5, Lsub, M);
AWest = hermitian_symmetry(AWest, Lsub, M);
AnWest = hermitian_symmetry(AnWest, Lsub, M);


% Gain calculation
disp(['-- Time: ',num2str(toc),' s. ','Calculating and applying subband gain'])
G5sub = gains(A5, An5, pow1, GLsub);
G5full = fullband_gains(G5sub, GLsub_dB, GLdB, Tfast_damping_dB, Tspeech_dB);
G5tot = G5sub.*G5full;
G7sub = G5sub;
G7full = ones(size(G5full)).*GL7full;
G7tot = G7sub.*G7full;
GWest = gains(AWest, AnWest, pow1, GLsub); % Borde det stå GLsub här?



% Make 0dB maximum
G5tot = G5tot./GL;
G7tot = G7tot./GL;
GWest = GWest./GLsub;



% --- Paper figures ---
disp(['-- Time: ',num2str(toc),' s. ','Creating result figures'])
% result_figure2(X_vec, G5tot, G7tot, ceil(fs/R), DIR_RES, TMdB, 'result2');
result_figure2(X_vec, G5tot, GWest, ceil(fs/R), DIR_RES, TMdB, 'result2');


