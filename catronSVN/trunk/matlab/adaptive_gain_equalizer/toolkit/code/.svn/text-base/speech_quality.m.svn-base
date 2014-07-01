function [sdi, sr, nr, iSNR, oSNR] = speech_quality(s, sf, active, n, nf, initLength)
%SPEECH_QUALITY - Calculates speech quality entities such as 
%speech-distortion index (SDI)
%
% Syntax:  [sdi, sr, nr] = speech_quality(s, sf, active, n, nf, initLength)
%
% Inputs:
%    s - Speech signal
%    sf - Filtered speech signal
%    active - Active regions in speech signal, vector with 1 (active) or 0
%           (passive)
%    n - Noise signal
%    nf - Filtered noise signal
%    initLength - (optional) Normalized length of when calculations should
%                 begin. This introduces a time for the algorithms to
%                 adjust.
%                 0 - all data included [default]
%                 0.5 - Later half of data included
%
% Outputs:
%    sdi - Speech-Distortion Index
%    sr - Speech Reduction factor
%    nr - Noise Reduction factor
%    iSNR - Input Signal-to-Noise ratio
%    oSNR - Output Signal-to-Noise ratio
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
% Created - 2011-May-19
% Last modification - 2011-May-24

%------------- BEGIN CODE --------------

% ---< Input check >---
% Column vector
s = s(:);
sf = sf(:);
active = active(:);
n = n(:);
nf = nf(:);

% ---< Active regions >---
ind = find(active);
sAct = s(ind);
sfAct = sf(ind);
nAct = n(ind);
nfAct = nf(ind);
% Remove first part of signal to let algorithms adapt
if(nargin<6)
    initLength = 0;
end
Ltot = length(sAct);
Lstart = max(floor(initLength*Ltot), 1); % Index must be greater than 0
sAct = sAct(Lstart:Ltot);
sfAct = sfAct(Lstart:Ltot);
nAct = nAct(Lstart:Ltot);
nfAct = nfAct(Lstart:Ltot);

% ---< (Trace of) autocorrelation >---
% Original speech
Rs = sum(sAct.^2);
% Difference: Filtered - Original
Rd = sum((sfAct-sAct).^2);
% Filtered speech
Rsf = sum(sfAct.^2);
% Original noise
Rn = sum(nAct.^2);
% Filtered noise
Rnf = sum(nfAct.^2);


% ---< Speech quality measures >---
% Speech distortion
sdi = Rd/Rs;
% Speech reduction
sr = Rs/Rsf;
% Noise reduction
nr = Rn/Rnf;
% Input SNR
iSNR = Rs/Rn;
% Output SNR
oSNR = Rsf/Rnf;

% ---<  >---

% ---<  >---

% ---<  >---

%------------- END OF CODE --------------