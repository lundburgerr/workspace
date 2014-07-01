function [Y, X] = reverb_filtering(file_X, file_IR, file_Y)
%REVERB_FILTERING - Filters a signal with a recorded impulse response
%
% Syntax:  [Y, X] = reverb_filtering(file_X, file_IR, file_Y)
%
% Inputs:
%    file_X - Name of signal to be filtered
%    file_IR - Name of impulse response recording
%    file_Y - (Optional) Name of output file
%
% Outputs:
%    Y - Filtered signal
%    X - Original signal
%
% Example: 
%    Line 1 of example
%    Line 2 of example
%    Line 3 of example
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2010-02-08
% Last modification - 2010-02-08

%------------- BEGIN CODE --------------
% Load data
[X, fsX] = wavread(file_X);
[IR, fsIR] = wavread(file_IR);

if(fsX ~= fsIR)
    disp('WARNING in reverb_filtering: The signals have different sampling frequencies.')
    disp(['Resampling impulse response to ',num2str(fsX),' Hz.'])
    % Resample
    IR = resample(IR, fsX, fsIR);
end

% Set sampling frequency
fs = fsX;
% Data length
% LX = length(X);
% LIR = length(IR);
freqz(IR, 1, 512);

% Filter with impulse response to get room reverberation
Y = filter(IR, 1, X);

% Write output
wavwrite(Y, fs, file_Y);

%------------- END OF CODE --------------