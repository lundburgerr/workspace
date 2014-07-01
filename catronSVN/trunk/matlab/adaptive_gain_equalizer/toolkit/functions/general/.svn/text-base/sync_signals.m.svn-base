function [xOut, yOut, D] = sync_signals(x, y, T)
%SYNC_SIGNALS - Performs time alignment of two audio signals by calculating
%cross correlation and adjusting signals with a delay D. The output
%signals are D samples shorter than the input signals. The input signals
%must be of the same length.
%
% Syntax:  [xOut, yOut, D] = sync_signals(x, y, T)
%
% Inputs:
%    x - Input signal vector, same length as y
%    y - Input signal vector, same length as x
%    T - (Optional) Extra "lookahead" of signal x. A value higher than 0
%        causes an extra delay T samples of signal x compared with signal
%        y. Default setting is 0.
%
% Outputs:
%    xOut - Synced version of x. length(xOut) = length(x)-D
%    yOut - Synced version of y. length(yOut) = length(y)-D
%    D - Sync factor, the number of samples that x was ahead of y
%
% Example: 
%    t = [0:0.1:8*pi];
%    x = sin(t);
%    y = cos(t);
%    [xOut, yOut, D] = sync_signals(x, y, 0);
%    figure
%    subplot(2,1,1), plot(x), hold on, plot(y,'r'), legend('x','y'), title('Before sync')
%    subplot(2,1,2), plot(xOut), hold on, plot(yOut,'r'), legend('xOut','yOut'), title('After sync')
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2011-Aug-25
% Last modification - 2011-Aug-30

%------------- BEGIN CODE --------------

% ---< Input check >---
L = length(x);
if(length(y)~=L)
    error('Input vectors not of same length');
end
if(nargin<3)
    T = 0;
end

% ---< Find crosscorrelation >---
corrVec = xcorr(x, y);
Lcorr = (length(corrVec)-1)/2;
[temp, corrVecIndex] = max(abs(corrVec));

% Debug plotting
% figure
% plot([-Lcorr:Lcorr], corrVec);


% ---< Sync signals >---
% Estimate delay
D = corrVecIndex-Lcorr;
% Store delay value
D_store = D;
if(D+T>=0)
    % Adjust delay so that we know a small time ahead
    D = D+T;
    xOut = x(D+1:end);
    yOut = y(1:end-D);
else

    % Adjust delay so that we know a small time ahead
    D = D+T;
    xOut = x(1:end+D);
    yOut = y(1-D:end);

end

% ---< Output >---
D = D_store;

%------------- END OF CODE --------------