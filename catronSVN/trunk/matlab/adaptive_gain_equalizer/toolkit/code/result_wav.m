function [] = result_wav(X, fs, ti, tf, ts, name)
%RESULT_WAV - Creates a .wav-file with segments of the input data. The
%first input data is followed by a period of silence and then the second
%input data, et.c.
%
% Syntax:  [] = result_wav(X, fs, ti, tf, ts, name)
%
% Inputs:
%    X - Matrix with input data in columns. The number of
%    fs - Sampling frequency
%    ti - Beginning of desired section in speech data in seconds
%    tf - End of desired section in speech data in seconds
%    ts - Silence buffer length
%    name - Filename of output .wav
%
% Outputs:
%    output1 - Description
%    output2 - Description
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
% E-mail - markus.borgh@limestechnology.com
% Website - www.limestechnology.com
% Copyright - Limes Technology AB
% Created - 2009-11-18
% Last modification - 2009-11-19

%------------- BEGIN CODE --------------

% Input data length
L = size(X,1);
% Nr of audio signals
N = size(X,2);

% Input arguments check
if( (ti*fs > L) || (tf*fs > L) )
    disp('ERROR in result_wav: Time interval is out of bounds.')
    disp(['The length of the data is ',num2str(L/fs),' seconds'])
    return
elseif( ti>tf )
    disp('ERROR in result_wav: Time interval start is larger then end time')
    return
end

% Determine start and stop positions
ni = floor(ti*fs);
nf = floor(tf*fs);
% Length of desired signal
Ld = nf-ni;
% Length of silence in samples
Ls = floor(ts*fs);
% Combined length
Lt = Ld+Ls;


% Allocate memory
y = zeros(N*Lt,1);

% Split data and put in vector
for(i=1:N)
    y(Lt*(i-1)+1:Lt*i-Ls) = X(ni+1:nf,i);
end

% Write to .wav
wavwrite(y, fs, name);

%------------- END OF CODE --------------