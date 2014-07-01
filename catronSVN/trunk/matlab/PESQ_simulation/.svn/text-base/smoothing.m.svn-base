function [ave] = smoothing(x, ky_rise, ky_fall)
%SMOOTHING - One pole IIR-filter
%
% Syntax:  [ave] = smoothing(x, ky_rise, ky_fall)
%
% Inputs:
%    x - Input audio signal
%    ky_rise - Gamma during rise
%    ky_fall - Gamma during fall
%
% Outputs:
%    ave - Smoothed signal
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
% Created - 2010-08-30
% Last modification - 2010-08-30

%------------- BEGIN CODE --------------

% ---< Settings >---
% Size of input
S = size(x);
L = max(S);

% ---< Init >---
ave = zeros(S);
% Absolute value
xabs = abs(x);
% First ave
ave(1) = xabs(1);
if(isnan(ave(1)))
    ave(1) = 0;
end

% ---< Do the thing >---
for k=2:L
    % NaN and Inf values are not included
    if(~isinf(xabs(k)) && ~isnan(xabs(k)))
        % Rise or fall?
        if(xabs(k) > ave(k-1))
            ky = ky_rise;
        else
            ky = ky_fall;
        end
        % Perform smoothing
        ave(k) = ky*ave(k-1) + (1-ky)*xabs(k);
    else
        ave(k) = ave(k-1);
    end
end

%------------- END OF CODE --------------