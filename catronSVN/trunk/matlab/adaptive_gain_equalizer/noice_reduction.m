function [Res, ResS, ResN, ResG] = noice_reduction(S, gainS, No, gainN, fs, useDelay, calcTheoretical)

addPath ./toolkit/code;
addPath ./filterbank1;

S = S*10^(gainS/20);
No = No*10^(gainN/20);

SIMU_LENGTH = min(length(S),length(No));

H = S(1:SIMU_LENGTH)+No(1:SIMU_LENGTH);

H = H./max(abs(H));

% --- Settings ---

beta_rise_db_per_sec = 9;

% Subband filterbank settings
M = 32; % Number of subbands (orig. 64)
N = 256; % Number of prototype filter coefficients (orig. 256)
R = M/2; % Decimation ratio

p = iterlsdesign(N, M, R);
freqz(p, 1, 4096, 16000);
%load filter64_256.mat % Loads p

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
alpha1 = 0.003; %0.001
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
X_vec = SubAnal(H, N, M, R, p);
N_vec = SubAnal(No, N, M, R, p);
S_vec = SubAnal(S, N, M, R, p);

if calcTheoretical
    X_vec = SubAnal(S, N, M, R, p);
end

% >> Subband processing
% Calculate short and long time averages of positive frequencies
SUB_POS = M/2+1; % Nr of subbands with positive frequencies
[A5, An5] = averages4(X_vec, alpha5, beta3, Lsub, SUB_POS, block_nr, block_size, noise_thresh); % Modified FENIX

% plotsub=3;
% figure
% %plot(abs(X_vec(plotsub,:)))
% plot(smooth(abs(X_vec(plotsub,:)),30))
% hold on
% plot(A5(plotsub,:),'r')
% plot(An5(plotsub,:),'m')
% legend('x','short time','long time')
% 

%timedelay 
if useDelay
    delay_steps = round(log(2)/alpha5);
    A5 = [A5, An5(:,end-delay_steps+1:end)];
    A5 = A5(:,delay_steps+1:end);
end

% plotsub=3;
% figure
% %plot(abs(X_vec(plotsub,:)))
% plot(smooth(abs(X_vec(plotsub,:)),30))
% hold on
% plot(A5(plotsub,:),'r')
% plot(An5(plotsub,:),'m')
% legend('x','short time','long time')

% Use the fact that there is a Hermitian symmetry owing to real input signals
A5 = hermitian_symmetry(A5, Lsub, M);
An5 = hermitian_symmetry(An5, Lsub, M);


% Gain calculation
% G5sub = gains(A5, An5, pow4, GLsub);   % <--------   IMPORTANT, CHANGED FROM pow1
G5sub = gains(A5, An5, pow1, GLsub);   % The effect of varying pow is not included in paper
G5tot = G5sub;


% Make 0dB maximum
G5tot = G5tot./GLsub;


% Apply gain
Y5 = X_vec.*G5tot;
YN = N_vec.*G5tot;
YS = S_vec.*G5tot;


% >> Put together subbands
[e_out5, e_tdl] = SubSynt(Y5, N, SIMU_LENGTH-N, R, p);
[n_out5, e_tdl] = SubSynt(YN, N, SIMU_LENGTH-N, R, p);
[s_out5, e_tdl] = SubSynt(YS, N, SIMU_LENGTH-N, R, p);
[g_out5, g_tdl] = SubSynt(G5tot, N, SIMU_LENGTH-N, R, p);


% --- Post processing ---
% Fill up with zeros to get same length as other vectors
e_out5 = fill_up_with_const(e_out5, SIMU_LENGTH, 0);
n_out5 = fill_up_with_const(n_out5, SIMU_LENGTH, 0);
s_out5 = fill_up_with_const(s_out5, SIMU_LENGTH, 0);
g_out5 = fill_up_with_const(g_out5, SIMU_LENGTH, 0);

Res = e_out5;
ResS = s_out5;
ResN = n_out5;

ResG = g_out5;
