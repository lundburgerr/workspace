function [f0, A, THD_tot] = thd_calc(x, fs, Nharm, f0low, f0high)
%THD_CALC - Calculates THD for a test signal consisting of a single input
%tone. Both stationary tones and tone sweeps are supported
%
% Syntax:  [f0, A] = thd_calc(x, fs, Nharm, f0low, f0high)
%
% Inputs:
%    x - Data with no quiet areas
%    fs - Sampling frequency
%    Nharm - Number of harmonics included in calculations
%    f0low - (Optional) Lower boundary for fundamental frequency [Hz]
%    f0high - (Optional) Upper boundary for fundamental frequency [Hz]
%
% Outputs:
%    f0 - Fundamental frequency vector
%    A - size N x 2 matrix with amplitude of fundamental frequency and
%        harmonics [dB]
%
% Example: 
%    [x, fs] = wavread('example.wav');
%    [f0, A] = thd_calc(x, fs, 4);
%    plot(f0, A(:,1)); % Fundamental frequency
%    hold on
%    plot(f0, A(:,2),'r'); % First harmonic
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2010-05-18
% Last modification - 2010-05-26

%------------- BEGIN CODE --------------

% For testing
% [f0, A] = thd_calc(y, fs, 4); A_fft = A; f0_fft =f0;
% [f0, A] = thd_calc(y, fs, 4); A_welch = A; f0_welch =f0;
% close all, n=4; plot(f0_welch,A_welch(:,n)), hold on, plot(f0_fft,A_fft(:,n),'r')


% ---< Input check >---
% Nr of argin within bounds
error(nargchk(3, 5, nargin, 'string'));
% Optional parameters
if(nargin<4)
    f0low = 300;
end
if(nargin<5)
    f0high = 7500;
end


% ---< Settings >---
% Datalength
L = length(x);
% FFT-size
NFFT = 2048;%1024; %16384
% NFFT = L;
% Measurement interval (USE NFFT!! to use all data. If larger, data will be omitted)
delta = floor(NFFT/10);

% Number of measurement points
M = floor((L-NFFT)/delta);
% Width of frequency peaks
wp = 10;


% ---< Scaling factors >---
% Window function
wind = window(@blackmanharris, NFFT);
% Window scaling factor
S_window = 1;%(sum(wind.^2)/length(wind));
% FFT scaling factor
S_FFT = 1/sqrt(NFFT);  % NFFT = length(ptemp)

% ---< Init >---
Atot = [];
ftot = [];
THD_tot = [];

for(m=1:M)
    % ---< Transform to frequency domain >---
    Is = (m-1)*delta+1;
    Ie = Is+NFFT-1;
    y = x(Is:Ie);

    % Find spectogram
%     [Pxx,f] = pwelch(y,[],[],NFFT,fs);
    % FFT version
    y = y.*wind;
    Pxx = fft(y,NFFT);
    
    % Scale
    Pxx = Pxx * S_FFT; % FFT-scaling
    Pxx  = Pxx * S_window; % Window scaling
    
    % Positive frequencies
    nn = floor(0.5*(NFFT +1 )) + 1;
    f = linspace(0, 0.5*fs, nn);
    Pxx = abs(Pxx(1:nn));

    % Data length
    LPxx = length(Pxx);

    % ---< Find fundamental frequency >---
    % Find highest peak
    [temp, H0_index] = max(Pxx);
    % Convert to frequency
    f0 = f(H0_index);

    % If out of bounds, move on to next block
    if(f0 < f0low)
        continue;
    end
    if(f0 > f0high)
        continue;
    end

    % Amplitude
    if(H0_index+wp < LPxx) && (H0_index-wp > 0)
        A0 = sum(Pxx(H0_index-wp:H0_index+wp));
    else
        A0 = 0;
    end
    warning off all
    % Convert to dB
    A0 = db(A0);
    warning on all

    % ---< Find harmonics >---

    % Go through all harmonics, find position and amplitude
    for(h=1:Nharm)
        % Harmonics position
        H_index(h) = (1+h)*H0_index;

        % Check if within max frequency
        if((H_index(h)+wp < LPxx) && (H_index(h)-wp > 0))
            fh(h) = f(H_index(h));
            % Amplitude
            Ah(h) = sum(Pxx(H_index(h)-wp : H_index(h)+wp));
            warning off all
            % Convert to dB
            Ah(h) = db(Ah(h));
            warning on all
        else
            fh(h) = 0;
            Ah(h) = -Inf;
        end
    end


    % ---< Calculate distortion >---
    THD = [];
    for(h=1:Nharm)
        THD = [THD, -(A0-Ah(h))];
    end

    % Store information
    ftot = [ftot; f0];
    temp = [A0, Ah(1:end)];
    Atot = [Atot; temp];
    THD_tot = [THD_tot; THD];
end


% Smooth curves
Nsmooth = 2;
Atot(:,1) = smooth(Atot(:,1),Nsmooth);
for(n=1:Nharm)
    Atot(:,1+n) = smooth(Atot(:,1+n),Nsmooth);
    THD_tot(:,n) = smooth(THD_tot(:,n),Nsmooth);
end

% ---<  >---
% Output
f0 = ftot;
A = Atot;


% ---<  >---

% ---<  >---

%------------- END OF CODE --------------