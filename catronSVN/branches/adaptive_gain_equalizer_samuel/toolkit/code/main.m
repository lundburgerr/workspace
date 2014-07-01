% --- Information ---
% Method 1: The method described by Westerlund et al.
% Method 2: The noise level is estimated from the minimum of blocked data.
%           The same subband gain as in Method 1 is used.
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
% Method 8: Subband gain is calculated with Method 4. The subband gain is
%           increased to maximal gain if the gain rises above a certain
%           threshold after a period of silence.

% Author - Markus Borgh
% E-mail - markus.borgh@limestechnology.com
% Website - www.limestechnology.com
% Copyright - Limes Technology AB
% Created - 2009-10-26
% Last modification - 2009-12-01

clear all, close all

% Start timer
tic;

DIR_HOME = pwd;
DIR_DATA = '.\signals\';
DIR_RES = '.\result\';

disp('_____________________________________________________________')
help main


%time = toc;
disp(['-- Time: ',num2str(toc),' s. ','Loading audio signals'])
% s0 = [DIR_DATA,'s0_16kHz.wav'];
% s0 = [DIR_DATA,'s0mod_16kHz.wav'];
s0 = [DIR_DATA,'s0multi_16kHz.wav'];
% s0 = [DIR_DATA,'christian_speech3.wav'];
% noise = [DIR_DATA,'n_16kHz.wav'];
% noise = [DIR_DATA,'npink_16kHz.wav'];
noise = [DIR_DATA,'ventillation.wav'];
% noise = [DIR_DATA,'dator.wav'];
% noise = [DIR_DATA,'cocktail_noise2.wav'];

% Gain (in dB) applied to speech and noise signal
sgain = -9;%0;
ngain = 0; %-3

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
window_size = 512;
% Frequency vector
freq = [0:0.5*window_size]*(0.5*fs);
freq = freq./length(freq);

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

% \\ Method 2
% Settings for Borgh et. al. method
block_size = 64;
beta2 = 0.05; %1-hex2dec('0b00')/32767; %0.05, %beta1;
block_size = 64; % Accumulation length
block_nr = 8; % Nr of blocks in accumulation
comp_factor = 2; % Factor to compensate difference between min- and mean-value in noise level estimation
noise_thresh = 10; % Minimum blockmax/blockmin ratio to detect speech

% \\ Method 3
% Increase of noise estimation during silence
beta3 = 5e-3;

% \\ Method 4
pow4 = ones(M,1)*1;
pow4(2) = 2; % 2 seems to work ok with s0 and ventillation - 3(9) dB
pow4(M/2+1:M) = pow4(M/2:-1:1);

% \\ Method 5
% Use different rise/fall parameter, 4 times faster
alpha5 = alpha1*4;
% Maximum subband enhancement in dB
GLsub_dB = 12;
GLsub_dB = min(GLsub_dB, GLdB); % Can't be more than maximal total gain
GLsub = 10^(GLsub_dB/20);
% Fullband gain settings
TMdB = 6; % 7.4
Tspeech_dB = max(GLsub_dB-TMdB, 0); % Threshold for start noise damping -7.4, -4
Tfast_damping_dB = max(GLdB-GLsub_dB-6, 0);%2; % Fast adaption down to this level

% \\ Method 7
% Constant fullband gain
GL7full_dB = GLdB-GLsub_dB;
GL7full = 10^(GL7full_dB/20);

% \\ Method 8
% Gain threshold
GT8dB = 6;
% Required silent time [seconds]
t8 = 0.1;


% --- Calculations ---
% Split signals in subbands
disp(['-- Time: ',num2str(toc),' s. ','Subband splitting'])
X_vec = SubAnal(mic1, N, M, R, p);

% >> Subband processing
disp(['-- Time: ',num2str(toc),' s. ','Calculating averages'])
% Calculate short and long time averages of positive frequencies
SUB_POS = M/2+1; % Nr of subbands with positive frequencies
[A1, An1] = averages(X_vec, alpha1, beta1, Lsub, SUB_POS); % Westerlund et. al
[A2, An2] = averages2(X_vec, alpha1, beta2, Lsub, SUB_POS, block_size, block_nr, comp_factor, noise_thresh); % FENIX
[A3, An3] = averages4(X_vec, alpha1, beta3, Lsub, SUB_POS, block_size, block_nr, noise_thresh); % Modified FENIX
[A5, An5] = averages4(X_vec, alpha5, beta3, Lsub, SUB_POS, block_size, block_nr, noise_thresh); % Modified FENIX
[A6, An6] = averages3(X_vec, alpha1, beta1, Lsub, SUB_POS); % Rangachari

% Use the fact that there is a Hermitian symmetry owing to real input signals
A1 = hermitian_symmetry(A1, Lsub, M);
An1 = hermitian_symmetry(An1, Lsub, M);
A2 = hermitian_symmetry(A2, Lsub, M);
An2 = hermitian_symmetry(An2, Lsub, M);
A3 = hermitian_symmetry(A3, Lsub, M);
An3 = hermitian_symmetry(An3, Lsub, M);
A5 = hermitian_symmetry(A5, Lsub, M);
An5 = hermitian_symmetry(An5, Lsub, M);
A6 = hermitian_symmetry(A6, Lsub, M);
An6 = hermitian_symmetry(An6, Lsub, M);

% Same level estimation method is used in multiple methods
A4 = A3; An4 = An3;
% A5 = A3; An5 = An3;
% A7 = A3; An7 = An3;


% Gain calculation
disp(['-- Time: ',num2str(toc),' s. ','Calculating and applying subband gain'])
G1 = gains(A1, An1, pow1, GL); % Westerlund
G2 = gains(A2, An2, pow1, GL); % FENIX subband method
G3 = gains(A3, An3, pow1, GL);
G4 = gains(A4, An4, pow4, GL); % Modify to reduce speech distortion
G5sub = gains(A5, An5, pow1, GLsub);
G5full = fullband_gains(G5sub, GLsub_dB, GLdB, Tfast_damping_dB, Tspeech_dB);
G5tot = G5sub.*G5full;
G6 = gains(A6, An6, pow1, GL); % Rangachari
G7sub = G5sub;
G7full = ones(size(G5full)).*GL7full;
G7tot = G7sub.*G7full;
G8 = G4;
% This is not used at the moment since it takes very long time right now
% G8 = gains_vad(G8, GLdB, GT8dB, floor(t8*fs));


% Apply gain
Y1 = X_vec.*G1;
Y2 = X_vec.*G2;
Y3 = X_vec.*G3;
Y4 = X_vec.*G4;
Y5 = X_vec.*G5tot;
Y6 = X_vec.*G6;
Y7 = X_vec.*G7tot;
Y8 = X_vec.*G8;


% >> Put together subbands
disp(['-- Time: ',num2str(toc),' s. ','Subband combining'])
[e_out1, e_tdl] = SubSynt(Y1, N, SIMU_LENGTH-N, R, p);
[g_out1, g_tdl] = SubSynt(G1, N, SIMU_LENGTH-N, R, p);
[e_out2, e_tdl] = SubSynt(Y2, N, SIMU_LENGTH-N, R, p);
[g_out2, g_tdl] = SubSynt(G2, N, SIMU_LENGTH-N, R, p);
[e_out3, e_tdl] = SubSynt(Y3, N, SIMU_LENGTH-N, R, p);
[g_out3, g_tdl] = SubSynt(G3, N, SIMU_LENGTH-N, R, p);
[e_out4, e_tdl] = SubSynt(Y4, N, SIMU_LENGTH-N, R, p);
[g_out4, g_tdl] = SubSynt(G4, N, SIMU_LENGTH-N, R, p);
[e_out5, e_tdl] = SubSynt(Y5, N, SIMU_LENGTH-N, R, p);
[g_out5, g_tdl] = SubSynt(G5tot, N, SIMU_LENGTH-N, R, p);
[e_out6, e_tdl] = SubSynt(Y6, N, SIMU_LENGTH-N, R, p);
[g_out6, g_tdl] = SubSynt(G6, N, SIMU_LENGTH-N, R, p);
[e_out7, e_tdl] = SubSynt(Y7, N, SIMU_LENGTH-N, R, p);
[g_out7, g_tdl] = SubSynt(G7tot, N, SIMU_LENGTH-N, R, p);
[e_out8, e_tdl] = SubSynt(Y8, N, SIMU_LENGTH-N, R, p);
[g_out8, g_tdl] = SubSynt(G8, N, SIMU_LENGTH-N, R, p);


% --- Post processing ---
disp(['-- Time: ',num2str(toc),' s. ','Post processing'])
% Fill up with zeros to get same length as other vectors
e_out1 = fill_up_with_const(e_out1, SIMU_LENGTH, 0);
g_out1 = fill_up_with_const(g_out1, SIMU_LENGTH, 0);
e_out2 = fill_up_with_const(e_out2, SIMU_LENGTH, 0);
g_out2 = fill_up_with_const(g_out2, SIMU_LENGTH, 0);
e_out3 = fill_up_with_const(e_out3, SIMU_LENGTH, 0);
g_out3 = fill_up_with_const(g_out3, SIMU_LENGTH, 0);
e_out4 = fill_up_with_const(e_out4, SIMU_LENGTH, 0);
g_out4 = fill_up_with_const(g_out4, SIMU_LENGTH, 0);
e_out5 = fill_up_with_const(e_out5, SIMU_LENGTH, 0);
g_out5 = fill_up_with_const(g_out5, SIMU_LENGTH, 0);
e_out6 = fill_up_with_const(e_out6, SIMU_LENGTH, 0);
g_out6 = fill_up_with_const(g_out6, SIMU_LENGTH, 0);
e_out7 = fill_up_with_const(e_out7, SIMU_LENGTH, 0);
g_out7 = fill_up_with_const(g_out7, SIMU_LENGTH, 0);
e_out8 = fill_up_with_const(e_out8, SIMU_LENGTH, 0);
g_out8 = fill_up_with_const(g_out8, SIMU_LENGTH, 0);

% Fullband smoothing
gamma = 0.9994;
mic1_ave = smoothing(mic1, gamma, gamma);
e_out1_ave = smoothing(e_out1, gamma, gamma);
g_out1_ave = smoothing(g_out1, gamma, gamma);
e_out2_ave = smoothing(e_out2, gamma, gamma);
g_out2_ave = smoothing(g_out2, gamma, gamma);
e_out3_ave = smoothing(e_out3, gamma, gamma);
g_out3_ave = smoothing(g_out3, gamma, gamma);
e_out4_ave = smoothing(e_out4, gamma, gamma);
g_out4_ave = smoothing(g_out4, gamma, gamma);
e_out5_ave = smoothing(e_out5, gamma, gamma);
g_out5_ave = smoothing(g_out5, gamma, gamma);
e_out6_ave = smoothing(e_out6, gamma, gamma);
g_out6_ave = smoothing(g_out6, gamma, gamma);
e_out7_ave = smoothing(e_out7, gamma, gamma);
g_out7_ave = smoothing(g_out7, gamma, gamma);
e_out8_ave = smoothing(e_out8, gamma, gamma);
g_out8_ave = smoothing(g_out8, gamma, gamma);


% Frequency spectra
wind = window(@kaiser, window_size);
start = floor(length(e_out1)*0.5);
% start = 1;
% Px = pwelch(mic1, wind);
Pxs = pwelch(xs(start:end), wind);
Pxn = pwelch(xn(start:end), wind);
Pe1 = pwelch(e_out1(start:end), wind);
Pe2 = pwelch(e_out2(start:end), wind);
Pe3 = pwelch(e_out3(start:end), wind);
Pe4 = pwelch(e_out4(start:end), wind);
Pe5 = pwelch(e_out5(start:end), wind);
% Pe6 = pwelch(e_out6(start:end), wind);
Pe7 = pwelch(e_out7(start:end), wind);
Pe8 = pwelch(e_out8(start:end), wind);
% In decibel
Pxs_dB = 10*log10(Pxs);
Pxn_dB = 10*log10(Pxn);
Pe1_dB = 10*log10(Pe1);
Pe2_dB = 10*log10(Pe2);
Pe3_dB = 10*log10(Pe3);
Pe4_dB = 10*log10(Pe4);
Pe5_dB = 10*log10(Pe5);
% Pe6_dB = 10*log10(Pe6);
Pe7_dB = 10*log10(Pe7);
Pe8_dB = 10*log10(Pe8);

% [spect3, fspect, tspect] = specgram(e_out3(start:end),window_size,fs,kaiser(500,5),475);
% [spect4, fspect, tspect] = specgram(e_out4(start:end),window_size,fs,kaiser(500,5),475);

% Square error
PD3xs_dB = (Pe3_dB-(Pxs_dB+GLdB)).^1;
PD4xs_dB = (Pe4_dB-(Pxs_dB+GLdB)).^1;
PD5xs_dB = (Pe5_dB-(Pxs_dB+GLdB)).^1;
PD7xs_dB = (Pe7_dB-(Pxs_dB+GLdB)).^1;

%[mean(PD3xs_dB),mean(PD4xs_dB),mean(PD5xs_dB); std(PD3xs_dB),std(PD4xs_dB),std(PD5xs_dB)]

% -- Post-processing
% G1 = G1 + eps;
% G2 = G2 + eps;
% G2tot = G2 + eps;
% A2 = A2 + eps;
% An2 = An2 + eps;
% An1 = An1 + eps;


% --- Plotting ---
disp(['-- Time: ',num2str(toc),' s. ','Plotting'])

figure
start = floor(Lsub/2);
plot([1:Lsub-start],ones(Lsub-start,1).*Tspeech_dB,'k')
hold on
plot(20*log10(mean(G5sub(1:32,start:end))'))
title('Subband gains (Method 5)'), xlabel('n'), ylabel('G_{sub}(n) [dB]')
legend('Speech detection threshold')
axis([0, Lsub-start, 0, 1.1*GLsub_dB])

figure
plot(time,20*log10(e_out3_ave),'r')
hold on
plot(time,20*log10(e_out4_ave),'k')
plot(time,20*log10(mic1_ave))
plot(time,20*log10(e_out5_ave),'g')
plot(time,20*log10(e_out7_ave),'c')
legend('Method 3', 'Method 4', 'Input', 'Method 5', 'Method 7')
title('Smoothed signal amplitudes'), xlabel('Time [s]'), ylabel('Amplitude [dB]')

figure
plot(time,20*log10(e_out4_ave),'k')
hold on
plot(time,20*log10(mic1_ave))
plot(time,20*log10(e_out8_ave),'r')
legend('Method 4', 'Input', 'Method 8')
title('Smoothed signal amplitudes'), xlabel('Time [s]'), ylabel('Amplitude [dB]')

figure
plot(time,20*log10(g_out3_ave),'r')
hold on
plot(time,20*log10(g_out4_ave),'k')
plot(time,20*log10(g_out8_ave),'b')
% plot(time,20*log10(g_out5_ave),'g')
% plot(time,20*log10(mic1_ave))
% legend('Method 3', 'Method 4', 'Method 5', 'Input')
legend('Method 3', 'Method 4', 'Method 8')
title('Gain'), xlabel('Time [s]'), ylabel('Gain [dB]')

figure
plot(freq, Pxs_dB+GLdB)
hold on
plot(freq, Pe3_dB,'r')
plot(freq, Pe4_dB,'k')
plot(freq, Pe5_dB,'g')
plot(freq, Pe7_dB,'c')
plot(freq, Pxn_dB+GLdB,'m')
legend(['Speech signal +',num2str(GLdB),' dB'], 'Method 3', 'Method 4','Method 5',['Noise signal +',num2str(GLdB),' dB'], 'Method 5')
title('Frequency spectra'), xlabel('f [Hz]'), ylabel('H(f) [dB]')

figure
IMAX = 3; %3
JMAX = 3; %2
k=1;
load speech4
%speech4 = speech4 + eps;
for(i=1:IMAX)
    for(j=1:JMAX)
        subplot(IMAX, JMAX, k)
        nr = (i-1)*IMAX+j;
        nr = k+0;
        plot(time_sub,20*log10(A1(nr,:)))
        hold on
        plot(time_sub,20*log10(An1(nr,:)),'r--')
        plot(time_sub,20*log10(An3(nr,:)),'m')
        plot(time_sub, 20*log10(speech4(nr,:)),'k')
        %plot(time_sub, maxi_save4(nr,:),'k')
        title(['A_{n}(',num2str(nr),',:)']), ylabel('A(n) [dB]'), xlabel('Time [s]')
        if(k==1)
            legend('Short time','Method 1','Method 3','No speech detected','Location','SouthEast')
            %legend('Short time','Method 1','Method 3','Location','SouthEast')
        end
        k=k+1; % Increase plot counter
    end
end

figure
k=1;
for(i=1:IMAX)
    for(j=1:JMAX)
        subplot(IMAX, JMAX, k)
        nr = (i-1)*IMAX+j;
        nr = k+0;
        plot(time_sub,20*log10(G1(nr,:)))
        hold on
        plot(time_sub,20*log10(G3(nr,:)),'r')
        %plot(time_sub,20*log10(G4(nr,:)),'k')
        %plot(time_sub,20*log10(G5tot(nr,:)),'g')
        %plot(time_sub,20*log10(G7tot(nr,:)),'c')
        %plot(time_sub,20*log10(G8(nr,:)),'b')
        title(['G(',num2str(nr),',:)']), ylabel('G(n) [dB]'), xlabel('Time [s]')
        if(k==1)
            legend('Method 1','Method 3','Method 4','Method 5','Method 7','Method 8')
        end
        k=k+1; % Increase plot counter
    end
end

figure
k=1;
for(i=1:IMAX)
    for(j=1:JMAX)
        subplot(IMAX, JMAX, k)
        nr = (i-1)*IMAX+j;
        nr = k+0;
        plot(time_sub,20*log10(G5sub(nr,:)),'k')
        hold on
        plot(time_sub,20*log10(G5tot(nr,:)),'g')
        plot(time_sub,20*log10(G5full(nr,:)),'m')
        title(['G(',num2str(nr),',:)']), ylabel('G(n) [dB]'), xlabel('Time [s]')
        if(k==1)
            legend('Method 5_{sub}','Method 5_{tot}','Method 5_{full}')
        end
        k=k+1; % Increase plot counter
    end
end

figure
plot(freq, PD3xs_dB,'r')
hold on
plot(freq, PD4xs_dB,'k')
plot(freq, PD5xs_dB,'g')
plot(freq, PD7xs_dB,'c')
legend('Method 3', 'Method 4','Method 5','Method 7')
title('Frequency difference (compared to speech signal + 24dB'), xlabel('f [Hz]'), ylabel('H_x(f)-H_s(f) [dB]')

% figure
% imagesc(tspect, fspect, 20*log10(abs(spect4)) - 20*log10(abs(spect3))), axis xy; colormap(jet)
% colorbar

% figure
% v = [1:800];
% surf(tspect(v), fspect, 20*log10(abs(spect4(:,v)) - 20*log10(abs(spect3(:,v)))))


% Figure positions
% cd(DIR_FUNC)
X_nr = 3;
Y_nr = 3;
[x_pos, y_pos, fg_width, fg_heigth] = figure_position(X_nr, Y_nr);
% cd(DIR_HOME) % Return to this directory
% Set figure positions
set(figure(1),'Position',[x_pos(1) y_pos(1) fg_width fg_heigth])
set(figure(2),'Position',[x_pos(2) y_pos(1) fg_width fg_heigth])
set(figure(3),'Position',[x_pos(3) y_pos(1) fg_width fg_heigth])
set(figure(4),'Position',[x_pos(1) y_pos(2) fg_width fg_heigth])
set(figure(5),'Position',[x_pos(2) y_pos(2) fg_width fg_heigth])
set(figure(6),'Position',[x_pos(3) y_pos(2) fg_width fg_heigth])
set(figure(7),'Position',[x_pos(1) y_pos(3) fg_width fg_heigth])
set(figure(8),'Position',[x_pos(2) y_pos(3) fg_width fg_heigth])
set(figure(9),'Position',[x_pos(3) y_pos(3) fg_width fg_heigth])


% --- Save data ---
cd(DIR_RES)
disp(['-- Time: ',num2str(toc),' s. ','Writing .wav'])
% wavwrite(e_out1, fs, 'method1.wav');
% wavwrite(e_out2, fs, 'method2.wav');
% wavwrite(e_out3, fs, 'method3.wav');
% wavwrite(e_out4, fs, 'method4.wav');
% wavwrite(e_out5, fs, 'method5.wav');
% wavwrite(e_out6, fs, 'method6.wav');
% Write damped output to .wav. This means that maximum gain corresponds to
% 0 dB.
damp = 10^(-GLdB/20);
% wavwrite(e_out1*damp, fs, 'method1_damp.wav');
% wavwrite(e_out2*damp, fs, 'method2_damp.wav');
% wavwrite(e_out3*damp, fs, 'method3_damp.wav');
% wavwrite(e_out4*damp, fs, 'method4_damp.wav');
% wavwrite(e_out5*damp, fs, 'method5_damp.wav');
% wavwrite(e_out6*damp, fs, 'method6_damp.wav');

% Create audio file that compares different methods
cd(DIR_HOME)
sig = [xs, mic1, e_out3'*damp, e_out4'*damp];
result_wav(sig, fs, 24, 31, 1, [DIR_RES,'s_sn_m3_m4.wav']);
sig = [xs, mic1, e_out7'*damp, e_out5'*damp];
result_wav(sig, fs, 24, 35, 1, [DIR_RES,'s_sn_m7_m5.wav']);
% result_wav(sig, fs, 44, 55, 1, [DIR_RES,'s_sn_m7_m5.wav']);
sig = [xs, mic1, e_out4'*damp, e_out8'*damp];
%result_wav(sig, fs, 24, 31, 1, [DIR_RES,'s_sn_m4_m8.wav']);

disp(['-- Time: ',num2str(toc),' s. ','Finished'])

