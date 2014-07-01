function [ave] = smoothing(x, ky_rise, ky_fall)
%smoothing - Smoothes absolute value of a signal
%
% Syntax:  [ave] = smoothing(x, ky_rise, ky_fall)
%
% Inputs:
%    x - Signal to be smoothed
%    ky_rise - Rise time parameter, range [0,1]
%    ky_fall - Fall time parameter, range [0,1]
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
% E-mail - markus.borgh@limestechnology.com
% Website - www.limestechnology.com
% Copyright - Limes Technology AB
% Created - 2009-10-29
% Last modification - 2009-10-29

%------------- BEGIN CODE --------------

% Absolute value
xabs = abs(x);

% Allocation
ave = zeros(size(x));

% First sample
ave(1) = xabs(1);

for(n=2:length(x))
    if(~isinf(xabs(n)) && ~isnan(xabs(n)))
        if(xabs(n)>ave(n-1))
            gamma = ky_rise;
        else
            gamma = ky_fall;
        end
        ave(n) = gamma*ave(n-1) + (1-gamma)*xabs(n);
    else
        ave(n) = ave(n-1);
    end
end

%------------- END OF CODE --------------