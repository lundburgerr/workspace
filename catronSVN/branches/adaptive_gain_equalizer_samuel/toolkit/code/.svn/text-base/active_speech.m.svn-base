function [sInd, sAct] = active_speech(s, thresh, fs, hold)
%ACTIVE_SPEECH - Finds active region in speech signal by comparing with
%energy threshold
%
% Syntax:  [sInd, sAct] = active_speech(s, thresh, fs, hold)
%
% Inputs:
%    s - Speech signal
%    thresh - Energy threshold (typical 0.001)
%    fs - Sampling rate
%    hold - Hold time in s (typical 0.01)
%
%
% Outputs:
%    sInd - Position index in s where there is active speech
%    sAct - Vector with 1 (active) or 0 (non-active). Same length as s
%
% Example: 
%    Line 1 of example
%    Line 2 of example
%    Line 3 of example
%
% Other m-files required: smoothing.m
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2011-May-31
% Last modification - 2011-May-31

%------------- BEGIN CODE --------------

% ---<  >---
% Column vector
s = s(:);

% Find active speech regions
sInd = zeros(size(s));
gamma = 0.99;
sSmooth = smoothing(s, gamma, gamma);

sInd = find(sSmooth>thresh);
sAct = zeros(size(s));
sAct(sInd) = 1;

% ---< Fix so that samples after detection also is detected >---
actHold = hold*fs; %10 ms
for(i=(length(sAct)-actHold):-1:1)
    if(sAct(i))
        sAct(i:i+actHold-1) = ones(actHold, 1);
    end
end

% ---<  >---

%------------- END OF CODE --------------