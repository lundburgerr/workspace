% Generates binaural filters for directive audio
% NOTE: the file hrtf.mat is from the HRIR repository
% http://marl.smusic.nyu.edu/projects/HRIRrepository
% ("S112_marl-nyu.mat" somewhat re-structured)

clear all, close all

load hrtf

Fs = 16000; % Sampling frequency
Rf = 0.65; % Wall reflection coefficient
L = 512;

% Room dimensions, MIC & LS position
ROOM = [ 4 4 3 ]; % dx, dy, dz
MIC_POS = [ 2 2 1.75 ];

el = 0; % Elevation angle (degrees from -40 to 90, steps of 10)
az =90; % Azimuth angle (degrees from -180 to 180, steps of 5)

% Azimuth and elevations (degrees => radians)
az_r = (az/360)*2*pi;
el_r = (el/360)*2*pi;

% Calculate cartesian coordinates for RIR calculation
LS_POS = [ sin(az_r)*cos(el_r), cos(az_r)*cos(el_r), sin(el_r) ] + MIC_POS;

% Calculate RIR
IRl = findIR_reverb(44100, Rf, L*3, ROOM, MIC_POS - [0.1 0 0], LS_POS, hrtf_l);
IRr = findIR_reverb(44100, Rf, L*3, ROOM, MIC_POS + [0.1 0 0], LS_POS, hrtf_r);

% Resample to 16kHz sampling frequency
IRl16 = resample(IRl, 16000, 44100);
IRr16 = resample(IRr, 16000, 44100);

% Flip and truncate impulse responses
%IRl16 = flipud(IRl16(1:L));
%IRr16 = flipud(IRr16(1:L));

% figure, plot(IRl16), hold on, plot(IRr16, 'r')
% title('Binaural impulse responses')
% legend('Left filter', 'Right filter')


%Testing sound
[speech, fs_speech]  = wavread('2013-03-27 - Normal_Speech.wav');
speech16 = resample(speech, Fs, fs_speech);
sound([conv(speech16, IRl16), conv(speech16, IRr16)], Fs)
