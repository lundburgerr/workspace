function [out, fs, part1, part2] = combine_signals(file1, file2, gain1, gain2, L)
%combine_signals - Open two .wav-files and adds them to one combined signal.
%The desired length of the output could be shorter or longer than the
%length of the input files. If shorter, the beginning of both files will be
%used. If longer, the signal will be repeated.
%
%If the signals have different sampling frequencies the sampling
%frequencies of file1 will be used.
%
% Syntax:  [out, fs, part1, part2] = combine_signals(file1, file2, gain1, gain2, L)
%
% Inputs:
%    file1 - File name for first wav-file
%    file2 - File name for second wav-file
%    gain1 - Damping/gain to be applied to signal 1 in dB
%    gain2 - Damping/gain to be applied to signal 2 in dB
%    L - Desired length of output file in seconds. If not specified the
%        length of file1 will be used as default.
%
% Outputs:
%    out - Combined signal
%    fs - Sampling frequency of output signal
%    part1 - (Optional) Input file1 with length L
%    part2 - (Optional) Input file2 with length L
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
% Created - 2009-11-18
% Last modification - 2009-11-19

%------------- BEGIN CODE --------------

% Load data
[x1, fs1] = wavread(file1);
[x2, fs2] = wavread(file2);

% Check data
if(fs1 ~= fs2)
    disp('WARNING in combine_signal: The signals have different sampling frequencies.')
    disp(['Resampling signals to ',num2str(fs1),' Hz.'])
    % Resample
    x2 = resample(x2, fs1, fs2);
end

% Set sampling frequency
fs = fs1;

% Input data lengths
N1 = length(x1);
N2 = length(x2);

% Decide length of output
if(nargin < 5)
    % If not specified use length of x1 as default
    L = N1/fs;
end

% Desired length in samples
N = floor(L*fs);

% Adjust signals to the desired length
y1 = fix_length(x1, N);
y2 = fix_length(x2, N);

% Adjust signals to the desired amplitude
y1 = y1.*10^(gain1/20);
y2 = y2.*10^(gain2/20);

% Combine signals
out = y1 + y2;

% Return partial signals with length L if output arguments are provided
if(nargout > 2)
    part1 = y1;
    if(nargout > 3)
        part2 = y2;
    end
end




% ######## LOCAL FUNCTIONS ########
function [y] = fix_length(x, Nd)
% Inputs:
%    x - Input signal
%    Nd - Desired length in samples
%
% Outputs:
%    y - Output signal

% Length of input
Ni = length(x);

% Memory allocation
y = zeros(Nd,1);

if(Ni > Nd)
    % Truncate signal
    y = x(1:Nd);
elseif(Ni < Nd)
    r = (Nd/Ni); % How much longer does the signal have to be?
    ru = ceil(r);
    % Loop signal
    for(n=1:ru)
        y(Ni*(n-1)+1:Ni*n) = x;
    end
    % Fix last segment
    y = y(1:Nd);
else
    % Correct length
    y = x;
end

return
% end of fix_length


%------------- END OF CODE --------------