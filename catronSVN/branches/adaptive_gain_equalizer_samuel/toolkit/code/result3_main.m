%RESULT3_MAIN
% Produces figure which shows the difference in 
%
% Method 1: The method described by Westerlund et al.
% Method 2: The noise level is estimated from the minimum of blocked data.
%           The same subband gain as in Method 1 is used.
% Method 3: The noise level is estimated with Method 1 but only updated
%           when the block difference is small. The same subband gain as in
%           Method 1 is used.
% Method 4: Same noise level estimation as in Method 3. The subband gain is
%           modified to give less speech distortion.

% Author - Markus Borgh
% E-mail - markus.borgh@limestechnology.com
% Website - www.limestechnology.com
% Copyright - Limes Technology AB
% Created - 2009-12-02
% Last modification - 2009-12-02

clear all, close all

% Start timer
tic;

DIR_HOME = pwd;
DIR_DATA = '.\signals\';
DIR_RES = '.\result\';

disp('_____________________________________________________________')
help result3_main


%time = toc;
disp(['-- Time: ',num2str(toc),' s. ','Loading audio signals'])
s0 = [DIR_DATA,'s0_16kHz.wav'];
% s0 = [DIR_DATA,'s0_16kHz_nlively_disc_REVERB_WARM2.wav'];
% s0 = [DIR_DATA,'christian_speech3.wav'];
% noise = [DIR_DATA,'n_16kHz.wav'];
% noise = [DIR_DATA,'npink_16kHz.wav'];
noise = [DIR_DATA,'ventillation.wav'];
% noise = [DIR_DATA,'dator.wav'];

% Gain (in dB) applied to speech and noise signal
sgain = 0; %0
ngain = -3; %-3

% Input data, combine speech and noise signal
[mic1, fs, xs, xn] = combine_signals(s0, noise, sgain, ngain, 40);
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
window_size = 512; %512
% Frequency vector
freq = [0:0.5*window_size]*(0.5*fs);
freq = freq./length(freq);
freq_log = log10(freq);

% \\ Method 1
% Settings for Westerlund et. al. method
alpha1 = 0.001; %0.0004
beta1 = 3e-5; %3e-5
pow1 = ones(M,1)*1;
GLdB = 24; %24,12 % Maximum enhancement in dB
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
%beta3 = 5e-3;
beta3 = (5e-3)/block_size;

% \\ Method 4
pow4 = ones(M,1)*1;
pow4(2) = 2; % 2 seems to work ok with s0 and ventillation - 3(9) dB
pow4(M/2+1:M) = pow4(M/2:-1:1);



% --- Calculations ---
% Split signals in subbands
disp(['-- Time: ',num2str(toc),' s. ','Subband splitting'])
X_vec = SubAnal(mic1, N, M, R, p);

% >> Subband processing
disp(['-- Time: ',num2str(toc),' s. ','Calculating averages'])
% Calculate short and long time averages of positive frequencies
SUB_POS = M/2+1; % Nr of subbands with positive frequencies
[A3, An3] = averages4(X_vec, alpha1, beta3, Lsub, SUB_POS, block_nr, block_size, noise_thresh); % Modified FENIX

% Use the fact that there is a Hermitian symmetry owing to real input signals
A3 = hermitian_symmetry(A3, Lsub, M);
An3 = hermitian_symmetry(An3, Lsub, M);

% Same level estimation method is used in multiple methods
A4 = A3; An4 = An3;


% Gain calculation
disp(['-- Time: ',num2str(toc),' s. ','Calculating and applying subband gain'])
G3 = gains(A3, An3, pow1, GL);
G4 = gains(A4, An4, pow4, GL); % Modify to reduce speech distortion


G3 = G3./GL;
G4 = G4./GL;

% Apply gain
Y3 = X_vec.*G3;
Y4 = X_vec.*G4;


% >> Put together subbands
disp(['-- Time: ',num2str(toc),' s. ','Subband combining'])
[e_out3, e_tdl] = SubSynt(Y3, N, SIMU_LENGTH-N, R, p);
[g_out3, g_tdl] = SubSynt(G3, N, SIMU_LENGTH-N, R, p);
[e_out4, e_tdl] = SubSynt(Y4, N, SIMU_LENGTH-N, R, p);
[g_out4, g_tdl] = SubSynt(G4, N, SIMU_LENGTH-N, R, p);

% --- Post processing ---
disp(['-- Time: ',num2str(toc),' s. ','Post processing'])
% Fill up with zeros to get same length as other vectors
e_out3 = fill_up_with_const(e_out3, SIMU_LENGTH, 0);
g_out3 = fill_up_with_const(g_out3, SIMU_LENGTH, 0);
e_out4 = fill_up_with_const(e_out4, SIMU_LENGTH, 0);
g_out4 = fill_up_with_const(g_out4, SIMU_LENGTH, 0);

% Fullband smoothing
gamma = 0.9994;
mic1_ave = smoothing(mic1, gamma, gamma);
e_out3_ave = smoothing(e_out3, gamma, gamma);
g_out3_ave = smoothing(g_out3, gamma, gamma);
e_out4_ave = smoothing(e_out4, gamma, gamma);
g_out4_ave = smoothing(g_out4, gamma, gamma);

% Frequency spectra
wind = window(@kaiser, window_size);
start = floor(length(e_out3)*0.5);
% start = 1;
Pxs = pwelch(xs(start:end), wind);
Pxn = pwelch(xn(start:end), wind);
Pe3 = pwelch(e_out3(start:end), wind);
Pe4 = pwelch(e_out4(start:end), wind);
% In decibel
Pxs_dB = 10*log10(Pxs);
Pxn_dB = 10*log10(Pxn);
Pe3_dB = 10*log10(Pe3);
Pe4_dB = 10*log10(Pe4);

% Square error
PD3xs_dB = (Pe3_dB-Pxs_dB).^1;
PD4xs_dB = (Pe4_dB-Pxs_dB).^1;

disp(['-- Time: ',num2str(toc),' s. ','Finished'])


% --- Plot ---
linewidth = 2;


h1 = figure;
ax1 = gca;
semilogx(freq,Pxs_dB,'k')
hold on
semilogx(freq,Pe3_dB,'k--')
semilogx(freq,Pe4_dB,'k-.')

h2 = figure;
ax2 = gca;
semilogx(freq,PD3xs_dB,'k--')
% plot(freq,PD3xs_dB,'k')
hold on
semilogx(freq,PD4xs_dB,'k')
% plot(freq,PD4xs_dB,'k')


% X-axis lower boundary
x_axis_low = 100;
% --- Create figure 3 ---
h3 = figure;
subplot(2,1,1)
ax3a = gca;
semilogx(freq,Pxs_dB,'k','LineWidth', linewidth)
hold on
semilogx(freq,Pxn_dB,'k--','LineWidth', linewidth)
% Change axis
v = axis;
v(1) = x_axis_low;
axis(v);
axis('auto y');

subplot(2,1,2)
ax3b = gca;
semilogx(freq,PD3xs_dB,'k--','LineWidth', linewidth)
hold on
semilogx(freq,PD4xs_dB,'k','LineWidth', linewidth)
% Change axis
v = axis;
v(1) = x_axis_low;
axis(v);
axis('auto y');


% --- Make it pretty
fontsize = 16; %16
fontleg = 12; % Fontsize in legends
% set(ax1, 'fontsize', fontsize);
% % Axis
% xlabel(ax1,'Frequency [Hz]')
% ylabel(ax1,'H(f) [dB]')
% 
% set(ax2, 'fontsize', fontsize);
% % Axis
% xlabel(ax2,'Frequency [Hz]')
% ylabel(ax2,'H_{diff}(f) [dB]')
% % title(ax2,'Difference between true speech and output spectra')



set(ax3a, 'fontsize', fontsize);
xlabel(ax3a,'Frequency [Hz]')
ylabel(ax3a,'Power [dB]')
title(ax3a,'(a)')
% title(ax3a,'Frequency spectra')
h = legend(ax3a,'s(n)','v(n)');
set(h,'fontsize',fontleg, 'location','SouthEast')

set(ax3b, 'fontsize', fontsize);
xlabel(ax3b,'Frequency [Hz]')
ylabel(ax3b,'Power [dB]')
title(ax3b,'(b)')
% title(ax3b,'Difference between true speech and output spectra')
h = legend(ax3b,'p_1 = 1','p_1 = 2');
set(h,'fontsize',fontleg, 'location','SouthEast')

% --- Results
% Save picture
cd(DIR_RES);
saveas(h3,'result3','eps')
cd(DIR_HOME);

%close(h);
