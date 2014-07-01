%RESULT4_MAIN
% Produces figure of speech and noise signals

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2009-12-18
% Last modification - 2010-04-26

clear all, close all

% Start timer
tic;

DIR_HOME = pwd;
DIR_DATA = '.\signals\';
DIR_RES = '.\result\';

disp('_____________________________________________________________')
help result4_main


%time = toc;
disp(['-- Time: ',num2str(toc),' s. ','Loading audio signals'])
s1 = [DIR_DATA,'s0_16kHz_nlively_disc_REVERB_WARM2.wav'];
s2 = [DIR_DATA,'s0mod_16kHz.wav'];
s3 = [DIR_DATA,'s0_16kHz.wav'];
% s0 = [DIR_DATA,'s0_16kHz.wav'];
noise = [DIR_DATA,'ventillation.wav'];

% Gain (in dB) applied to speech and noise signal
sgain = 0;
ngain = -3; %-3

% Input data, combine speech and noise signal
[mic1, fs, xs1, xn] = combine_signals(s1, noise, sgain, ngain, 20);
[mic1, fs, xs2, xs3] = combine_signals(s2, s3, 0, 0, 20);
SIMU_LENGTH = length(mic1);


% --- Settings ---
% Subband filterbank settings
R = 32; % Decimation ratio

% Subband length
Lsub = ceil(SIMU_LENGTH/R);
time = [0:SIMU_LENGTH-1]./fs;


% --- Plot ---
linewidth = 0.5;

h = figure;
subplot(3,1,1)
ax1a = gca;
plot(time,xs1,'k','LineWidth', linewidth)

subplot(3,1,2)
ax1b = gca;
plot(time,xs2,'k','LineWidth', linewidth)

% subplot(4,1,3)
% ax1c = gca;
% plot(time,xs3,'k','LineWidth', linewidth)

subplot(3,1,3)
ax1d = gca;
plot(time,xn,'k','LineWidth', linewidth)
axis([0,20,-1,1])


% --- Make it pretty
fontsize = 16+4; %16
fontleg = 12; % Fontsize in legends

set(ax1a, 'fontsize', fontsize);
set(ax1b, 'fontsize', fontsize);
% set(ax1c, 'fontsize', fontsize);
set(ax1d, 'fontsize', fontsize);
% Axis
xlabel(ax1a,'Time [s]')
xlabel(ax1b,'Time [s]')
% xlabel(ax1c,'Time [s]')
xlabel(ax1d,'Time [s]')
ylabel(ax1a,'s(n)')
ylabel(ax1b,'s(n)')
% ylabel(ax1c,'s(m)')
ylabel(ax1d,'w(n)')
title(ax1a,'(a)')
title(ax1b,'(b)')
% title(ax1c,'(c)')
title(ax1d,'(c)')

% --- Screensize
% Fullscreen
fullscreen = get(0,'ScreenSize');
X1 = -0.1; % 0.3 if 4 plots
set(h,'Position',[0, -X1*fullscreen(4), 0.6*fullscreen(3), (1+X1)*fullscreen(4)]);
% Use actual look when saving
set(h, 'PaperPositionMode', 'auto')

% --- Results
% Save picture
cd(DIR_RES);
saveas(h,'result4','eps')
cd(DIR_HOME);

%close(h);
