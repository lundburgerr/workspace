
clear vox;
%clear global;

global vadval;
vadval = [];

global blocksize; %has to be even
global NFFT;
global NPSD;
global fs;

global phiNN;
global phiXX;
global phiYY;
global phiXXsmooth;
global phiYYsmooth;
global P;
global Psmooth;
global scaling;
global Gain;
global w;

global s_hat;

global scalingPlot;
global specGainPlot;
global PLDPlot;


tic

s_hat = zeros(blocksize, 1);

blocksize = 256;
NFFT = 256; %has to be power of 2
fs = 8000;
phiNN = zeros(NPSD, 1);
phiXX = zeros(NPSD, 1);
phiYY = zeros(NPSD, 1);
phiXXsmooth = zeros(NPSD, 1);
phiYYsmooth = zeros(NPSD, 1);
P = zeros(NPSD, 1);
Psmooth = zeros(NPSD, 1);
scaling = 1;
Gain = ones(NPSD, 1);
if mod(NFFT,2) == 0
    NPSD = NFFT/2+1;
else
    NPSD = (NFFT+1)/2;
end
if mod(blocksize,2) == 0
    w = hann(blocksize, 'periodic');
else
    w = hann(blocksize, 'symmetric');
end


%Read in signal in fixed point, this will be buffer in DSP
[s, fs_file] = wavread('startMotorPrat.wav');
%[s, fs_file] = wavread('test0snr.wav');
%[s, fs_file] = wavread('test18snr.wav');
fs = 8000;
s = resample(s, fs, fs_file);
%TODO: make fixed point
x = s(:,1); %speech + noise
y = s(:,2); %noise
%The algorithm
N = length(x);
M = floor(length(x)/(blocksize/2));
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

scalingPlot= [];
specGainPlot = [];
PLDPlot = [];
%%%%%%


s_hatpf = zeros(length(x), 1);

for m=1:M-1
    t = 1+(m-1)*blocksize/2:(m+1)*blocksize/2;
    thalf = 1+(m-1)*blocksize/2:m*blocksize/2;
    tdone = 1:blocksize/2;
    voxNoise(x(t),y(t))
    
    s_hatpf(thalf) = s_hatpf(thalf) + s_hat(tdone);
    %sound(s_hat(tdone));
end

s_hatpf = 10*s_hatpf;

toc
figure; plot((1:N)/N, x, 'r'); hold on;
%plot((1:length(scalingPlot))/length(scalingPlot), scalingPlot, 'g');
plot((1:length(PLDPlot))/length(PLDPlot), PLDPlot./20, 'k');
%plot((1:length(specGainPlot))/length(specGainPlot), specGainPlot, 'm');

plot((1:N)/N, s_hatpf./10, 'b'); 
%plot((0:(M-2))/(M-1), vadval/100, 'g');