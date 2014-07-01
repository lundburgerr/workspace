function [pos, len] = find_tone_position(x, ft, fs)
%FIND_TONE_POSITION - Finds occurancies of a single stationary tone in
% audio data.
%
% Syntax:  [pos, len] = find_tone_position(x, ft, fs)
%
% Inputs:
%    x - Vector with signal data
%    ft - Frequency of the the tone [Hz]
%    fs - Sampling frequency [Hz]
%
% Outputs:
%    pos - Vector with occurancies of the tone [samples]
%    len - Length of positions in pos
%
% Example: 
%    [x, fs] = wavread('example.wav')
%    % Find occurancies of 500 Hz tone
%    [tone_start, tone_length] = min(find_tone_position(x, 500, fs));
%    % Plot first occurance of tone
%    plot(x(tone_start(1):tone_start(1)+tone_length(1)))
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

% ---< Settings >---
% FFT-size
NFFT = fs/8;% 16384;
% Datalength
L = length(x);
% Measurement interval
delta = fs/8;%6000;  % THIS SHOULD BE NFFT, OTHERWISE DATA WILL BE OMITTED
% Number of measurement points
M = floor(L/delta)-1;
% Tolerated frequency width [Hz]
f_tol = 10;

% ---< Init >---
tone_detected = zeros(M,1);
position = zeros(M,1);

% ---< Detection >---
for(m=1:M)
    % ---< Transform to frequency domain >---
    % Divide data
    Is = (m-1)*delta+1;
    Ie = Is+delta;
    y = x(Is:Ie);
    % Store position
    position(m) = Is;
    
    % Find spectogram
    [Pxx,f] = pwelch(y,[],[],NFFT,fs);

    % ---< Find fundamental frequency >---
    % Find highest peak
    [temp, ind] = max(Pxx);
    % Convert to frequency
    f0 = f(ind);
    % Tone detected?
    if((f0 < ft+f_tol) && (f0 > ft-f_tol))
        tone_detected(m) = 1;
    end

end

% ---< Post processing >---
% Init
pos = 1;
len = 0;
n = 0;

% Store start position and length of tone sequences
for(m=1:M)
    if(tone_detected(m))
        % If previous interval did not contain a tone, jump to next position
        if(m==1 || ~tone_detected(m-1))
            n = n+1;
            
            % Store new position
            pos(n) = position(m);
            len(n) = 0;
        end
        % Increase length of current tone sequence
        len(n) = len(n) + delta;
    end
end

% % Index of detected tones
% ind = find(tone_detected);
% % Position
% pos = position(ind);

%------------- END OF CODE --------------
